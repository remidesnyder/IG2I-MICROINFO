/**
 * Desnyder Rémi / Videlaine Charles-Augustin
 * Objectifs: Réaliser une conversion ADC en 10 bits et afficher le résultat sur un afficheur 7 segments sur 2 voies
 * Non Fonctionnel
 * Limite : Précision dûe à la carte qui envoie 5.12 ou 5.13V mais une correction a été appliquée
 * Tests: 
 *  - Test unique sur le capteur (Fonctionnels)
 * 
 * Cablage: 
 *  - D0 à D7 sur a à g
 *  - B0 à B3 sur Dis0 à Dis3
 *  - POT0 sur A0
 *  - GND capteur sur GND BNC
 *  - V0 Capteur sur Channel 2
 *  - Channel 2 sur A1
 *  - POT0 sur Channel 1
 *  - C0 à C7 sur Led 00 à Led 07
 *  - E0 à E1 sur Led 08 à Led 09
 * 
 * Etat:
 *  - Nous avons seulement eu le temps de tester le capteur mais nous n'avons pas eu le temps de mettre en place la double voie
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
#define portC0   PORTCbits.RC0 // Constance port C 0 sur portC0

/****************************************************************************/
/*		P R O T O T Y P E S   D E   F O N C T I O N S						*/
/****************************************************************************/
void init(void);
void initTimer600ms(void);
void initAnalog(void);
void traitementConvSegment(void);
void afficherSegment(int resultat);
void afficherNombre(int millier, int centaine, int dizaine, int unite);
void clearAfficheur(void);
int tempo(void);
/****************************************************************************/
/*		V A R I A B L E S    G L O B A L E S			                    */
/****************************************************************************/
unsigned char i;
int resultat1;
int flag = 0;

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
        if(flag) flag=0;            // Si  allumé alors éteindre
        else if(!flag) flag=1;      // Si éteinte alors allumé
               
        GO = 1;
        
        while (GO == 1) {}
        
        clearAfficheur();
        traitementConvSegment();
        
        TMR0IF = 0;
        
        TMR0H = 0x6D; // Bit haut
        TMR0L = 0x84; // Bit bas
        
    }
}


/****************************************************************************/
/*		F O N C T I O N     P R I N C I P A L E 							*/
/****************************************************************************/

void main(void) {
    
    init();
    
while(1) {

    clearAfficheur();
    afficherSegment(resultat1);

}
    return;
}
	
/* Fin de la fonction main() */

/****************************************************************************/
/*		Z O N E   D E   S O U S   P R O G R A M M E S 						*/
/****************************************************************************/
void init() {	   
    
    TRISAbits.RA0 = 1;
    TRISAbits.RA1 = 1;
    
    TRISD = 0b00000000; // Tout est en sortie
    TRISC = 0b00000000; // Tout est en sortie
    
    // Port B0 à B3 en sortie
    TRISBbits.RB0 = 0;
    TRISBbits.RB1 = 0;
    TRISBbits.RB2 = 0;
    TRISBbits.RB3 = 0;
    
    PORTD = 0b01111111;
    
    TRISEbits.RE0 = 0;
    TRISEbits.RE1 = 0;
    
    initAnalog();
    initTimer600ms();

}

void initAnalog() {
    ADON = 1; //A/D On bit
    // Analog Channel Select bits
    // Ici 000 = channel 0, (AN0)
    CHS2= 0; 
    CHS1= 0; 
    CHS0= 0;
    
    ADCS2 = 0;
    ADCS1 = 0;
    ADCS0 = 1;
    
    ADFM = 1;
    
    GO = 1;
    
    ADCON1bits.CHS2 = 0;
}

void initTimer600ms() {
    // Init timer
    INTCONbits.GIE = 1;
    INTCONbits.TMR0IE = 1;
    T08BIT = 0; // Timer 16 bits (0 = 16bit)
    PSA = 0; // Prescaler activé (0 = on)
    T0CS = 0; // 0 clock interne
    
    // Timer0 Prescaler Select bits
    // Ici 16 bits 
    T0PS2 = 0;
    T0PS1 = 1;
    T0PS0 = 1;
    
    // Décalage de départ
    TMR0H = 0x6D; // Bit haut
    TMR0L = 0x84; // Bit bas
    
    TMR0ON = 1; // Timer0 activé
}

/**
 * Calcul du délai
 * 
 * Max : 65 536 x 10^-3 ms
 * 
 * 65 536 - temps = decalage
 * 65 536 - temps = 55 536 -> D8F0
 * 
 * 600ms = 600 000 micros
 * 
 * Calcul prescalaire 
 * 600 000 / 65356 = 9 Donc presca de 16
 * 600 000 / 16 (Preca) = 37 500
 * 65 536 - 37 500 = 28 036 -> 6D84
 */

void afficherNombre(int millier, int centaine, int dizaine, int unite) { 
    clearAfficheur();
    PORTB = 0b00001110; // Port B0 pour allumer l'afficheur le plus à gauche (0 = allumer, 1 = éteint)
    PORTD = chiffre[millier]; // PortD prend valeur pour afficher 8
    tempo();            // Appel de tempo pour attendre
        
    clearAfficheur();
    PORTB = 0b00001101;
    PORTD = chiffre[centaine];
    tempo();
        
    clearAfficheur();
    PORTB = 0b00001011;
    PORTD = chiffre[dizaine];
    tempo(); 
                
    clearAfficheur();
    PORTB = 0b00000111; // Port B3 pour allumer l'afficheur le plus à droite (0 = allumer, 1 = éteint)
    PORTD = chiffre[unite];
    tempo();
}

void clearAfficheur() {
    PORTB = 0b00000000;
    PORTD = 0b11111111;
    tempo();
}

int tempo() {
    // 1 op de base = 1 micro seconde
    // i = 10 (4microseconde) en 8bits / 2 octets
    for (unsigned int y = 0; y < 80; y++) {
        int retard = 0;
    }
}

void traitementConvSegment() {
    resultat1 = ADRESH;
    resultat1 = (resultat1 << 8) + ADRESL; 
    
    resultat1 = ((resultat1 * 4.899472141)); //  5389... correspond à 5012/1023
    
    afficherSegment(resultat1);
    
    PORTE = ADRESH;
    PORTC = ADRESL;
    
}

void afficherSegment(int resultat) {
    
    int millier = 0;
    int centaine = 0;
    int dizaine = 0;
    int unite = 0;
    
    while (resultat >= 1000) {
        millier++;
        resultat = resultat - 1000;
    }
    
    while (resultat >= 100) {
        centaine++;
        resultat = resultat - 100;
    }
    
    while (resultat >= 10) {
        dizaine++;
        resultat = resultat - 10;
    }
    
    while (resultat >= 1) {
        unite++;
        resultat = resultat - 1;
    }
      
    clearAfficheur();
    afficherNombre(millier, centaine, dizaine, unite);
}
