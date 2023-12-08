/**
 * Desnyder Rémi / Videlaine Charles-Augustin
 * Objectifs: 
 *  - Timer d'une seconde
 * Fonctionnel
 * Limite : Aucune
 * Tests: 
 *  - Visualisation du délai de 1 s sur l'oscilloscope (Changement de borne à chaque état 0 ou 1)
 * 
 * Cablage: 
 *  - C0 sur Channel 1
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
void initTimer(void);
/****************************************************************************/
/*		V A R I A B L E S    G L O B A L E S			                    */
/****************************************************************************/

/****************************************************************************/
/*		T R A I T E M E N T S   D' I N T E R R U P T I O N                  */
/****************************************************************************/

void  __interrupt(high_priority) myHighint(void)             // High priority interrupt
{
    if(TMR0IF) {                        // Si l'overflow du timer vaut 1
        if(portC0) portC0=0;            // Si  allumé alors éteindre
        else if(!portC0) portC0=1;      // Si éteinte alors allumé
        
        TMR0IF = 0;
        
        TMR0H = 0x0B; // Bit haut
        TMR0L = 0xDC; // Bit bas
        
    }
}


/****************************************************************************/
/*		F O N C T I O N     P R I N C I P A L E 							*/
/****************************************************************************/

void main(void) {
    
    init();
    
    while(1) {
    }
    
    return;
}
	
/* Fin de la fonction main() */

/****************************************************************************/
/*		Z O N E   D E   S O U S   P R O G R A M M E S 						*/
/****************************************************************************/
void init() {	   
    
    TRISCbits.RC0 = 0;
    
    initTimer();

}

void initTimer() {	
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
    TMR0H = 0x0B; // Bit haut
    TMR0L = 0xDC; // Bit bas
    
    TMR0ON = 1; // Timer0 activé
}

/**
 * Calcul du délai
 * 
 * Max: 65 536 x 10^-3 ms
 * 
 * 1s = 1 000 000 microSecondes
 * 
 * Calcul prescalaire
 * 1 000 000 / 65 536 = 15 Donc presca de 16
 * 1 000 000 / 16 = 62 500
 * 65 536 - 62 500 = 3 036 -> 0BDC
 * 
*/