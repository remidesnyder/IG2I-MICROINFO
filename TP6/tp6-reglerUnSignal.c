/**
 * Desnyder Rémi / Videlaine Charles-Augustin
 * Objectifs: 
 *  - Régler un signal (une note) précisement
 * Fonctionnel
 * Limite : Aucune
 * Tests: 
 *  - T = 249 us
 *  - Th = 2.75 us
 *  - Vérifié à l'oscilloscope
 * 
 * Cablage: 
 *  - C2 sur Channel 1
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
#define C2 PORTCbits.RC2
/****************************************************************************/
/*		P R O T O T Y P E S   D E   F O N C T I O N S						*/
/****************************************************************************/
void init(void);
/****************************************************************************/
/*		V A R I A B L E S    G L O B A L E S			                    */
/****************************************************************************/
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
    }
    
    return;
}
	

/* Fin de la fonction main() */
/****************************************************************************/
/*		Z O N E   D E   S O U S   P R O G R A M M E S 						*/
/****************************************************************************/
void init() {	 
    
    // Init timer
    INTCONbits.GIE = 1;
    INTCONbits.TMR0IE = 1;
    
    // Régler T -> PR2 (à 249 us) 
    // Attention, il y a toujours un +1 automatiquement
    PR2 = 248;
    
    // Règler Th (2.75 us)
    // CCPR1L et DC1B1 / DC1B0
    CCPR1L = 0x2;
    DC1B1 = 1; // 0.5
    DC1B0 = 1; // 0.25
    
    // CCP1 -> RC2 (En sortie)
    TRISCbits.RC2 = 0;
    
    // Configurer T2CON
    T2CKPS1 = 0; // Pas de prescaler 
    T2CKPS0 = 0;
            
    // CCP1CON mode PWM
    CCP1M3 = 1;
    CCP1M2 = 1;
    CCP1M1 = 1;
    CCP1M0 = 1;
    
    TMR2ON = 1; // Activer le timer
}
