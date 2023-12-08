/**
 * Desnyder Rémi / Videlaine Charles-Augustin
 * Objectifs: Réaliser un chronométre sur 4 afficheurs 7 segments 
 * Fonctionnel
 * Limite : 99.99 s
 * Tests: 
 *  - Visualisation du compteur sur les afficheurs
 * 
 * Cablage: 
 *  - D0 à D7 sur a à g
 *  - B0 à B3 sur Dis0 à Dis3
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
#define portB0   PORTBbits.RB0 // Constance port b 0 sur portB0
#define portB1   PORTBbits.RB1
#define portB2   PORTBbits.RB2
#define portB3   PORTBbits.RB3

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
        
        
        TMR0IF = 0;
        
        TMR0H = 0xD8; // Bit haut
        TMR0L = 0xF0; // Bit bas
        
    }
}


/****************************************************************************/
/*		F O N C T I O N     P R I N C I P A L E 							*/
/****************************************************************************/

void main(void) {
    
    init();
    
while(1) {

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
}
    return;
}
	
/* Fin de la fonction main() */

/****************************************************************************/
/*		Z O N E   D E   S O U S   P R O G R A M M E S 						*/
/****************************************************************************/
void init() {	   
    TRISD = 0b00000000; // Tout est en sortie
    
    TRISBbits.RB0 = 0;
    TRISBbits.RB1 = 0;
    TRISBbits.RB2 = 0;
    TRISBbits.RB3 = 0;
    
    initTimer10ms();
    
    PORTD = 0b01111111;
}

void initTimer10ms() {
    // Init timer
    INTCONbits.GIE = 1;
    INTCONbits.TMR0IE = 1;
    T08BIT = 0; // Timer 16 bits (0 = 16bit)
    PSA = 1; // Prescaler activé (0 = on)
    T0CS = 0; // 0 clock interne
    
    // Décalage de départ
    //TMR0H = 0xD8; // Bit haut
    //TMR0L = 0xF0; // Bit bas
    TMR0H = 0xD8; // Bit haut
    TMR0L = 0xF0; // Bit bas
    
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
 * Pas besoin de prescalaire
 */


void afficherNombre(int seconde, int unite, int dizaine, int centaine) { 
    clearAfficheur();
    PORTB = 0b00001110; // Port B0 pour allumer l'afficheur le plus à gauche (0 = allumer, 1 = éteint)
    PORTD = chiffre[seconde]; // PortD prend valeur pour afficher seconde
    tempo();
    
    clearAfficheur();
    PORTB = 0b00001101; // Port B0 pour allumer l'afficheur le plus à gauche (0 = allumer, 1 = éteint)
    PORTD = chiffre[unite]; // PortD prend valeur pour afficher unite
    PORTDbits.RD0 = 0;
    tempo();
     
    clearAfficheur();
    PORTB = 0b00001011; // Port B0 pour allumer l'afficheur le plus à gauche (0 = allumer, 1 = éteint)
    PORTD = chiffre[dizaine]; // PortD prend valeur pour afficher dizaine
    tempo();
    
    clearAfficheur();
    PORTB = 0b00000111; // Port B0 pour allumer l'afficheur le plus à gauche (0 = allumer, 1 = éteint)
    PORTD = chiffre[centaine]; // PortD prend valeur pour afficher centaine
    tempo();
}

void clearAfficheur() {
    PORTB = 0b00000000; // Allumer les afficheurs
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