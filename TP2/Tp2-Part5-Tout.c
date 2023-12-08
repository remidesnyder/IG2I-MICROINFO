/**
 * Desnyder R�mi / Videlaine Charles-Augustin
 * Objectifs: R�aliser un chronom�tre sur 4 afficheurs 7 segments avec un interrupteur marche / arret et remise � zero / temps intermediaire
 * Partiellement fonctionnel
 * Limite : 
 *  - 99.99 s
 *  - Cr�ation de deux variables donc erreur lors d'une interruption et d'un arret
 *  - Lorsque le compteur est en mode intermediaire et qu'on arrete, l'�tat ne reviens pas en marche
 *  - Solution: Cr�er une variable �tat (0 = �tat initial; 1 = �tat marche; 2 = �tat arret; 3 = �tat intermediaire)
 * Tests: 
 *  - Etat intial : 00.00
 *  - Visualisation du compteur sur les afficheurs
 *  - Arret et marche apr�s action sur l'interrupteur
 *  - Remise � 0 si compteur arret�
 *  - Temps intermediaire si compteur en marche
 *  - Tests qui a men� a une erreur (Cas limite)
 * 
 * Cablage: 
 *  - D0 � D7 sur a � g
 *  - C0 � C3 sur Dis0 � Dis3
 *  - I0 sur B0
 *  - I1 sur B1
**/

/****************************************************************************/
/*		I N C L U D E S   S T A N D A R D S        							*/
/****************************************************************************/
#include <xc.h>


/****************************************************************************/
/*		C O N F I G U R A T I O N	                                        */
/****************************************************************************/
#pragma config OSC = XT         // Oscillator Selection bits (XT oscillator)
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled #pragma config OSC = ECIO

/****************************************************************************/
/*		C O N S T A N T E S  S Y M B O L I Q U E S		                   */
/****************************************************************************/
#define portC0   PORTCbits.RC0 // Constance port c 0 sur portB0
#define portC1   PORTCbits.RC1
#define portC2   PORTCbits.RC2
#define portC3   PORTCbits.RC3

/****************************************************************************/
/*		P R O T O T Y P E S   D E   F O N C T I O N S						*/
/****************************************************************************/
void init(void);
void initTimer10ms(void);
void afficherNombre(int seconde, int unite, int dizaine, int centaine);
void clearAfficheur(void);
int tempo(void);
/****************************************************************************/
/*		V A R I A B L E S    G L O B A L E S			                    */
/****************************************************************************/
unsigned char i;

int flag = 0;
int centaine = 0;
int dizaine = 0;
int unite = 0;
int seconde = 0;

int centaineStock = 0;
int dizaineStock = 0;
int uniteStock = 0;
int secondeStock = 0;

int active = 0;
int intermediaire = 0;

int chiffre[10] = {
    0b00000011,  // 0
    0b10011111,  // 1
    0b00100101,  // 2
    0b00001101,  // 3
    0b10011001,  // 4
    0b01001001,  // 5
    0b01000001,  // 6
    0b00011111,  // 7
    0b00000001,  // 8
    0b00001001   // 9
};

/****************************************************************************/
/*		T R A I T E M E N T S   D' I N T E R R U P T I O N                  */
/****************************************************************************/

void  __interrupt(high_priority) myHighint(void)             // High priority interrupt
{
    if(TMR0IF) {                        // Si l'overflow du timer vaut 1
        
        if(flag) {
            flag = 0;
            centaine++;
        } else if(!flag) {
            flag = 1;
            centaine++;
        }
        
        TMR0IF = 0; // overflow = 0
        
        TMR0H = 0xD8; // Bit haut
        TMR0L = 0xF0; // Bit bas
    }
    
    if (INT0IF)  {
        INT0IF = 0; // Flag interrupteur = 0
        
        if(INTEDG0) INTCON2bits.INTEDG0 = 0;
        else if(!INTEDG0) INTCON2bits.INTEDG0 = 1;
        
        if(active) { // Si l'�tat est actif
            active = 0; // etat off
            TMR0ON = 0; // Timer0 d�sactiv�
        } else {    // sinon
            active = 1; // etat on
            TMR0ON = 1; // Timer0 activ�
        }
    }
    
    if(INT1IF) {
        INT1IF = 0; // Flag interrupteur = 0
        
        if(INTEDG1) INTCON2bits.INTEDG1 = 0;
        else if(!INTEDG1) INTCON2bits.INTEDG1 = 1;
        
        if (active) {                               // Si l'�tat est actif
            if (!intermediaire) {                   // Et que nous sommes pas en interm. 
                intermediaire = 1;                  // inter actif
                centaineStock = centaine;           // Suite d'action pour stock� les valeurs actuel
                dizaineStock = dizaine;
                uniteStock = unite;
                secondeStock = seconde;  
            } else {
                intermediaire = 0;                                      // inter off
                clearAfficheur();
                afficherNombre(seconde, unite, dizaine, centaine);
            }

        } else {
            seconde = 0;
            unite = 0;
            dizaine = 0;
            centaine = 0;
        }
    }
    
}


/****************************************************************************/
/*		F O N C T I O N     P R I N C I P A L E 							*/
/****************************************************************************/

void main(void) {
    
    init();
    
while(1) {
    
    if (!intermediaire) {
        if(centaine > 9) { 
            centaine = 0;
            dizaine++;
        }
        
        if(dizaine > 9) { 
            dizaine = 0;
            unite++;
        }

        if(unite > 9) { 
            unite = 0;
            seconde++;
        }

        if (seconde > 9) {
            seconde = 0;
        }

        clearAfficheur();
        afficherNombre(seconde, unite, dizaine, centaine);
    } else {
        if(centaine > 9) { 
            centaine = 0;
            dizaine++;
        }
        
        if(dizaine > 9) { 
            dizaine = 0;
            unite++;
        }

        if(unite > 9) { 
            unite = 0;
            seconde++;
        }

        if (seconde > 9) {
            seconde = 0;
        }
        
        if(centaineStock > 9) { 
            centaineStock = 0;
            dizaineStock++;
        }
        
        if(dizaineStock > 9) { 
            dizaineStock = 0;
            uniteStock++;
        }

        if(uniteStock > 9) { 
            uniteStock = 0;
            secondeStock++;
        }

        if (secondeStock > 9) {
            secondeStock = 0;
        }

        clearAfficheur();
        afficherNombre(secondeStock, uniteStock, dizaineStock, centaineStock);
    }

}
    return;
}
	
/* Fin de la fonction main() */

/****************************************************************************/
/*		Z O N E   D E   S O U S   P R O G R A M M E S 						*/
/****************************************************************************/
void init() {	   
    TRISD = 0b00000000; // Tout est en sortie
    
    TRISCbits.RC0 = 0;
    TRISCbits.RC1 = 0;
    TRISCbits.RC2 = 0;
    TRISCbits.RC3 = 0;
    
    TRISBbits.RB0 = 1;
    
    INTCONbits.GIE = 1;
    INTCONbits.INT0IE = 1; // Activ� l'inte
    INTCON3bits.INT1IE = 1; // Activ� l'inte
    
    initTimer10ms();
    
    PORTD = 0b01111111;
}

void initTimer10ms() {
    // Init timer
    INTCONbits.GIE = 1;
    INTCONbits.TMR0IE = 1;
    T08BIT = 0; // Timer 16 bits (0 = 16bit)
    PSA = 1; // Prescaler activ� (0 = on)
    T0CS = 0; // 0 clock interne
    
    // D�calage de d�part
    //TMR0H = 0xD8; // Bit haut
    //TMR0L = 0xF0; // Bit bas
    TMR0H = 0xD8; // Bit haut
    TMR0L = 0xF0; // Bit bas
    
    TMR0ON = 0; // Timer0 activ�
}

/**
 * Calcul du d�lai
 * 
 * Max : 65 536 x 10^-3 ms
 * 
 * 65 536 - temps = decalage
 * 65 536 - temps = 55 536 -> D8F0
 * 
 * Pas besoin de prescalaire
 */


void afficherNombre(int seconde, int unite, int dizaine, int centaine) { 
    clearAfficheur();
    PORTC = 0b00001110; // Port B0 pour allumer l'afficheur le plus � gauche (0 = allumer, 1 = �teint)
    PORTD = chiffre[seconde]; // PortD prend valeur pour afficher seconde
    tempo();
    
    clearAfficheur();
    PORTC = 0b00001101; // Port B0 pour allumer l'afficheur le plus � gauche (0 = allumer, 1 = �teint)
    PORTD = chiffre[unite]; // PortD prend valeur pour afficher unite
    PORTDbits.RD0 = 0;
    tempo();
     
    clearAfficheur();
    PORTC = 0b00001011; // Port B0 pour allumer l'afficheur le plus � gauche (0 = allumer, 1 = �teint)
    PORTD = chiffre[dizaine]; // PortD prend valeur pour afficher dizaine
    tempo();
    
    clearAfficheur();
    PORTC = 0b00000111; // Port B0 pour allumer l'afficheur le plus � gauche (0 = allumer, 1 = �teint)
    PORTD = chiffre[centaine]; // PortD prend valeur pour afficher centaine
    tempo();
}

void clearAfficheur() {
    PORTC = 0b00000000; // Allumer les afficheurs
    PORTD = 0b11111111; // Eteindre les segments
    tempo();
}

int tempo() {
    // 1 op de base = 1 micro seconde
    // i = 10 (4microseconde) en 8bits / 2 octets
    for (unsigned int y = 0; y < 80; y++) {
        int retard = 0;

    }
}
