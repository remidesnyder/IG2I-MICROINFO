/**
 * Desnyder R�mi / Videlaine Charles-Augustin
 * Objectifs: 
 * Non Fonctionnel
 * Limite : 
 * Tests: 
 * 
 * Cablage: 
 *  - RB3 sur LCD_E 
 *  - RB4 sur LCD_RW 
 *  - RB5 sur LCD_RS 
 *  - PORTD sur LCD_PORT
**/

/////////////////////////////////////////////////////////////////
// Fichier de base pour une communication par BUS I2C
// Quartz de 4MHz
/////////////////////////////////////////////////////////////////


/*		I N C L U D E S   S T A N D A R D S        							*/
/****************************************************************************/
#include <xc.h>

/****************************************************************************/
/*		C O N F I G U R A T I O N	                                        */
/****************************************************************************/
#pragma config OSC = HS
#pragma config WDT = OFF
#pragma config LVP = OFF


/****************************************************************************/
/*		C O N S T A N T E S  S Y M B O L I Q U E S		                   */
/****************************************************************************/
#define	LCD_E           PORTBbits.RB3	// LCD enable
#define	LCD_RW          PORTBbits.RB4	// LCD read/write line
#define	LCD_RS          PORTBbits.RB5	//LCD register select line
#define LCD_PORT        PORTD           // LCD donn?es ou commandes sur 8 bits

#define SDL             PORTBbits.RC3
#define SDA             PORTBbits.RC4

#define	LCD_E_DIR       TRISBbits.TRISB3	
#define	LCD_RW_DIR      TRISBbits.TRISB4	
#define	LCD_RS_DIR      TRISBbits.TRISB5
#define LCD_PORT_DIR    TRISD 

#define DUREE           -62465 // dur?e de l'interruption ; TMR0H=0x?? ; TMR0L=0x??
                               // valeur en s : ??s 

// D�finir les adresses des registres du DS1307
#define DS1307_ADDRESS 0b1101000 // Adresse I2C du DS1307
#define DS1307_ADDRESS_WRITE 0b11010000
#define DS1307_ADDRESS_READ 0b110100001
#define SEC_REG_ADDRESS 0x00     // Adresse du registre des secondes du DS1307
#define MIN_REG_ADDRESS 0x01     // Adresse du registre des minutes du DS1307
#define DS1307_CONTROL_REG_ADDRESS 0b10010000

/****************************************************************************/
/*		P R O T O T Y P E S   D E   F O N C T I O N S						*/
/****************************************************************************/
void Delay(long counter);
void i2c_init();// Fonction d'initialisation du module MSSP pour un fonctionnement en I2C
void i2c_idle();// attendre que le module I2C soit inactif
void i2c_start();//lancer une s?quence START
void i2c_restart();//lancer une s?quence RESTART
void i2c_stop();//lancer une s?quence STOP
void i2c_write(unsigned char data);//ecrire un octet sur la ligne SDA et g?rer l'acquittement de l'esclave
unsigned char i2c_read(unsigned char x);//lire un octet sur la ligne SDA et envoyer la valeur d'acquittement x
void I2C(char *donnee_haut,unsigned char *donnee_bas);//G?n?rer une trame pour lire deux octets successifs sur la ligne SDA
void convert(int x,int y);// Conversion de deux entiers en vue d'un affichage format? sur l'?cran LCD et affichage sur l'?cran LCD
void afficher_mot_lcd(char mot[]);// Afficher sur l'?cran LCD la chaine de caract?res mot[]]
void envoi_lcd(unsigned char temp);// Afficheren binaire sur l'?cran LCD  l'octet temp
void lcdread(unsigned char *temp);// Lire sur l'?cran LCD le caract?re Temp
void LCDBusy();// Tester si l'?cran LCD est occupp?
void d_write(unsigned char temp);// Envoyer la donn?e temp au LCD 
void i_write(unsigned char temp);// Envoyer l'instruction temps au LCD
void lcdinit();// Initialiser le fonctionnement de l'?cran LCD

void Init_DS1307(void); // Initialiser le DS1307
void I2C_secondes_minutes(char *secondes, char *minutes);
/****************************************************************************/
/*		V A R I A B L E S    G L O B A L E S			                    */
/****************************************************************************/
unsigned char temp_wr;
char x,y; // entiers pour stocker la valeur de temp?rature
int c=1; 
int numero;
int w='x'; // caract?re de test
char temp[11]="Temp = "; //premiers mots de l'afficheur


/****************************************************************************/
/*		T R A I T E M E N T S   D' I N T E R R U P T I O N                  */
/****************************************************************************/

void  __interrupt(high_priority) myHighint(void)             // High priority interrupt
{if (INTCONbits.TMR0IF)
	{
		// Gestion du timer0
		INTCONbits.TMR0IF=0;// flag remis ? z?ro
	    TMR0=DUREE;
		// D?but de l'affichage 
		i_write(0x80); // curseur en d?but de premi?re ligne
		//Delay(100); //attente pour le bon fonctionnement de l'afficheur
		afficher_mot_lcd(temp);// affichage du premier mot

		// Lecture de deux octets
        I2C(&x,&y);
        
		// Suite de l'affichage
		envoi_lcd(0x96); // affichage en binaire de l'octet 0x96 pass? en param?tre de la proc?dure
		// Pour faire clignoter un caract?re ? chaque fois que le programme passe par l'interruption	
			if (c==1)
				{
				 c=0 ; 
				 d_write('B');
				}
			else
				{
				 c=1;
				 d_write('b');
				}
		// Affichage sur la deuxi?me ligne
		i_write(0xc0); // curseur en d?but de seconde ligne
		//Delay(100); //attente pour le bon fonctionnement de l'afficheur
		convert(x,y); // affichage sur LCD : pour l'instant on n'affiche que xx.xxxx C ou yy.yyyy C
		
		Nop();// pour le bon fonctionnement de l'ensemble 
	}	
   
}
/****************************************************************************/
/*		F O N C T I O N     P R I N C I P A L E 							*/
/****************************************************************************/
void main(void) {
    // Initialisations diverses
	TRISB=0; // port B en sortie 
	lcdinit(); // initialisation de l'afficheur
	Delay(100); // attente pour le bon fonctionnement de l'afficheur
	ADCON1=7;// PortA en num?rique

	// Initialisation du timer pour 1 conversion toutes les  ? ms
	T0CON=0x83;	// prescaler de ?
	TMR0=DUREE; // valeur de la dur?e de l'interruption en s : ???s
	
	// Autorisation des interruptions
	INTCONbits.TMR0IE=1;
	INTCONbits.GIE=1;
    
    //......Initialisation du module MSSP pour un fonctionnement en I2C
    i2c_init();
	

	//Ecrire ici les lignes de code pour initialiser un premier esclave I2C
    Init_DS1307();
	Nop(); // Utile pour les points d'arr?t
	//Ecrire ici les lignes de code pour initialiser un second esclave I2C
	Nop(); // Utile pour les points d'arr?t
	//Ecrire ici les lignes de code pour initialiser un troisi?me esclave I2C
	Nop(); // Utile pour les points d'arr?t
   // ....
	    
while(1) 
{

 // La suite du programme principal s'effectue en boucle sans fin

}
    return;
}
	
  /* Fin de la fonction main() */


/****************************************************************************/
/*		Z O N E   D E   S O U S   P R O G R A M M E S 							*/
/****************************************************************************/


//////////////////////////////////////////////////////////////
// Fonction d?lai : counter contient la valeur du d?lai
//////////////////////////////////////////////////////////////

void Delay(long counter)
{
	while (counter>0)
	{
		counter--;
	}
}

/////////////////////////////////////////////////////////////////
// Fonctions de base pour construire des trames I2C
//////////////////////////////////////////////////////////////////

void i2c_init()
{
    // Fonction de parm?trage du module MSSP pour un fonctionnement en I2C
    // ......Ecrire ici la configuration de SSPCON1 (param?trage statique des communications)
	// .....Choisir ici le taux de transfert
	//......Configuration de SSPSTAT (param?tres g?n?raux des ?tats)

	SSPSTATbits.CKE = 0; 	// D�sactive le bus
	SSPADD = 10; 			// 100kHz
	SSPCON1 = 0b00101000;	
	// Du bits le plus faible au plus fort :
	// Des bits 0 � 3 : SSPM3:SSPM0 : 1000 Mode maitre I2C, clock = FOSC/(4 * (SSPADD + 1))
	// 0010 -> x / Reception normale / RC3 et RC4 configur�s en SCL et SDA I2C / X
	//SSPSTATbits.SMP = 1; // Controle du slew rate -> 1 = d�sactiv� (Standard Speed mode)
    SSPSTATbits.SMP = 1;
	SSPSTATbits.CKE = 1; // Active le bus

}

void Init_DS1307(void) {
    // Configurer le registre de contr�le pour activer le DS1307
    i2c_start();               // D�marrer la s�quence I2C
    i2c_write(DS1307_ADDRESS_WRITE); // S�lectionner le DS1307 en �criture
    i2c_write(SEC_REG_ADDRESS); // S�lectionner le registre des secondes
    i2c_write(0b00000000);
    i2c_write(0b00010010);
    i2c_write(0b00010100);
    i2c_write(0b00000010);
    i2c_write(0b00010010);
    i2c_write(0b00010010);
    i2c_write(0b00100011);
    i2c_write(DS1307_CONTROL_REG_ADDRESS); // S�lectionner le registre de contr�le
    i2c_stop();                // Arr�ter la s�quence I2C
}

void i2c_idle()
{
    // attendre que le module I2C soit inactif
	while (SSPSTATbits.R_W || SSPCON2bits.SEN || SSPCON2bits.RSEN || SSPCON2bits.PEN || SSPCON2bits.ACKEN);
	// SSPSTATbits.R_W : 1 = lecture en cours, 0 = �criture en cours
	// SSPCON2bits.SEN : 1 = start en cours
	// SSPCON2bits.RSEN : 1 = restart en cours
	// SSPCON2bits.PEN : 1 = stop en cours
	// SSPCON2bits.ACKEN : 1 = acquittement en cours
}

void i2c_start()
{
	// Fonction pour demander au controleur I2C de g?n?rer le bit de start
	SSPCON2bits.SEN = 1; // SEN = 1 -> Commence une s�quence de start
	while (SSPCON2bits.SEN); // Attend que le bit SEN soit remis � 0
}

void i2c_restart()
{
 	// Fonction pour demander au controleur I2C de g?n?rer le bit de restart	
	SSPCON2bits.RSEN = 1;	// RSEN = 1 -> Commence une s�quence de restart
	while (SSPCON2bits.RSEN); // Attend que le bit RSEN soit remis � 0
}

void i2c_stop()
{
 	// Fonction pour demander au controleur I2C de g?n?rer le bit de stop
	SSPCON2bits.PEN = 1; // PEN = 1 -> Commence une s�quence de stop
	while (SSPCON2bits.PEN); // Attend que le bit PEN soit remis � 0
}

void i2c_write(unsigned char data)
{
  	// Fonction pour ?crire l'octet data sur la ligne SDA et g?rer l'acquittement du r?cepteur
	i2c_idle();
	SSPBUF = data; // Ecriture de l'octet data dans le buffer
	while (SSPSTATbits.R_W); // Attend que le bit R/W soit remis � 0
	while (SSPCON2bits.ACKSTAT); // Attend que le bit ACKSTAT soit remis � 0
}

unsigned char i2c_read(unsigned char x) 
{
	// Fonction pour lire un octet sur la ligne SDA et g?rer l'acquittement du PIC
   i2c_idle();
   SSPCON2bits.RCEN = 1; 		// RCEN = 1 -> Commence une s�quence de lecture
   while (SSPCON2bits.RCEN); 	// Attend que le bit RCEN soit remis � 0
   SSPCON2bits.ACKEN = x;		// ACKEN = 1 -> Envoie un acquittement
   SSPCON2bits.ACKEN = 1;		
   while (SSPCON2bits.ACKEN);	// Attend que le bit ACKEN soit remis � 0
   return SSPBUF;				
}

////////////////////////////////////////////////////////////////////////////////////
// Proc?dure de lecture de deux octets : donnee_haut et donne-bas
////////////////////////////////////////////////////////////////////////////////////

void I2C(char *donnee_haut,unsigned char *donnee_bas)
{
    
    I2C_secondes_minutes(donnee_haut, donnee_bas);
    
    // Ecrire ici les lignes de code pour g?n?rer la trame de lecture de 2 octets
    // l'octet de poids fort (MS Byte) sera r?cup?r? dans *donnee_haut
    // l'octet de poids faible (LS Byte) sera r?cup?r? dans *donnee_bas
	Nop(); // Attente 

	Nop(); // Attente
	
}
////////////////////////////////////////////////////////////////////////////////////////////
// Proc?dure de conversion des deux octets x et y pour un affichage en d?cimal :
// 	Tout est ? faire...
//	Ce qui est actuellement ?crit ne fait qu'afficher xx.xxxx C ou yy.yyyy C
////////////////////////////////////////////////////////////////////////////////////////////
 
void convert(int x,int y)
	// Les mots d'origine x et y sont des octets
	// Objectif de la fonction : Il faut les transformer en caract?res pour les afficher
	{
		if (w=='x') w='y'; else w='x';
		d_write((unsigned char)w);
		d_write((unsigned char)w);
		d_write((unsigned char)'.');
		d_write((unsigned char)w);
		d_write((unsigned char)w);
		d_write((unsigned char)w);
		d_write((unsigned char)w);
		d_write((unsigned char)' ');
        d_write((unsigned char)'C');
}

//////////////////////////////////////////////////////////////////////////////////
// Proc?dure d'affichage de 7 caract?res de la chaine de caract?re mot
//////////////////////////////////////////////////////////////////////////////////

void afficher_mot_lcd(char mot[]) // affichage de caract?res
{
	int i;
	for (i=0;i<=6;i++)
	{
		d_write(mot[i]);
	}
}

////////////////////////////////////////////////////////////////////////////////
// Proc?dure d'affichage sur l'?cran LCD de l'octet temp en binaire
////////////////////////////////////////////////////////////////////////////////

void envoi_lcd(unsigned char temp) // affichage en binaire de l'octet temp
{
	int i;
	for (i=0;i<8;i++)
	{
		if (temp&0x80) // ne teste que le bit de poids fort
			d_write(0x31); // afficher 1 si ce bit est ? 1
		else
			d_write(0x30); // afficher 0 si ce bit est ? 0
		temp=temp<<1; // d?cale de 1 bit ? gauche
	}
}


////////////////////////////////////////////////////////////////////////////////
// Proc?dures utiles pour g?rer l'afficheur
////////////////////////////////////////////////////////////////////////////////

void lcdread(unsigned char *temp)
{
	*temp=0;
	LCD_PORT_DIR=0xff;	// bus en entr?e
	LCD_RS=0; // mode commande			
	LCD_RW=1; // mode lecture
	Nop();
	Nop();
	LCD_E=1; // validation
	Nop();
	Nop();
	*temp=LCD_PORT;
	Nop();
	Nop();
	LCD_E=0; // non validation
}
void LCDBusy() //test BF reste tant que BF=1 , attend affichage termin?
{
unsigned char temp_rd;
	do
	{
	lcdread(&temp_rd);
	}
	while ((temp_rd & 0x80)==0x80);	
}

void d_write(unsigned char temp) // ?crire la donn?e (caract?re temp) sur l'?cran LCD
{
	LCDBusy(); //v?rifie affichage termin?
	LCD_PORT_DIR=0; // portD en sortie
	LCD_RS=1; // donn?e
	LCD_RW=0; // ?criture
	LCD_PORT=temp;
	LCD_E=1;
	Nop();
	LCD_E=0;
	Nop();	
}
void i_write(unsigned char temp) // envoyer une instruction (caract?re temp) ? l'?cran LCD
{
	LCDBusy(); // v?rifie affichage termin?
	LCD_PORT_DIR=0; // portD en sortie
	LCD_RS=0; // instruction
	LCD_RW=0; // mode ?criture
	LCD_PORT=temp;
	LCD_E=1;
	Nop();
	LCD_E=0;
	Nop();	
}

void lcdinit() // initialiser l'?cran LCD
{	
	unsigned char temp_wr;
	int i;
	temp_wr=0x38;// affichage 8 bits sur 2 lignes
	LCD_PORT_DIR=0;
	LCD_E_DIR=0; 	
	LCD_RW_DIR=0; //met en position d'?criture
	LCD_RS_DIR=0;
	LCD_RS=0;
	LCD_RW=0;
    LCD_PORT=temp_wr;
	LCD_E=1; //activation
	Nop();
	LCD_E=0; //desactivation
	Nop();	

	temp_wr=0x01;// effacer afficheur
	i_write(temp_wr);
	//for(i=1;i<=500;i++);// boucle d'attente
	temp_wr=0x0F;// mzt curseur et affichage en fonction
	i_write(temp_wr);
	
	temp_wr=0x06; // d?placement ? droite + ne bouge pas
	i_write(temp_wr);			
}

void I2C_secondes_minutes(char *secondes, char *minutes) {
    // Lire les registres des secondes et des minutes du DS1307
 
    // Lire les secondes
    i2c_start();               // D�marrer la s�quence I2C
    i2c_write(DS1307_ADDRESS_WRITE); // S�lectionner le DS1307 en �criture
    i2c_write(SEC_REG_ADDRESS); // S�lectionner le registre des secondes
    i2c_restart();             // Red�marrer la s�quence I2C
    i2c_write(DS1307_ADDRESS_READ); // S�lectionner le DS1307 en lecture
    *secondes = i2c_read(0);  // Lire les secondes avec acquittement
    *minutes = i2c_read(1);  // Lire les minutes sans acquittement
    i2c_stop();                // Arr�ter la s�quence I2C*/
}
