/**
 * Desnyder Rémi / Videlaine Charles-Augustin
 * Objectifs: Afficher une tension correspondante sur un afficheur 7 segments
 * Fonctionnel
 * Limite : Nombre maximale possible 9999
 * Tests: 
 *  ADRESH = 1 -> Affiche 19mV
 *  ADRESH = 8 -> Affiche 156mV
 *  ADRESH = 64 -> Affiche 1254mV
 *  ADRESH = 255 -> Affiche 5000mV
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
void afficherNombre(int millier, int centaine, int dizaine, int unite);
void clearAfficheur(void);
int tempo(void);
void initTimer10ms(void);
void traitementConvSegment(void);
void afficherSegment(int resultat);
/****************************************************************************/
/*		V A R I A B L E S    G L O B A L E S			                    */
/****************************************************************************/
unsigned char i;

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
}


/****************************************************************************/
/*		F O N C T I O N     P R I N C I P A L E 							*/
/****************************************************************************/

void main(void) {
    
    init();
    
while(1) {
    
    clearAfficheur();
    traitementConvSegment();
}
    return;
}
	
/* Fin de la fonction main() */

/****************************************************************************/
/*		Z O N E   D E   S O U S   P R O G R A M M E S 						*/
/****************************************************************************/
void init() {	   
    TRISD = 0b00000000; // Tout est en sortie
    
    // Port B0 à B3 en sortie
    TRISBbits.RB0 = 0;
    TRISBbits.RB1 = 0;
    TRISBbits.RB2 = 0;
    TRISBbits.RB3 = 0;
    
    PORTD = 0b01111111;
}

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
    int resultat1;
    int nombre = 128;
    
    resultat1 = ((nombre * 19.60784314)); // 19.607.... correspond à 5000/255
    
    afficherSegment(resultat1);
    
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