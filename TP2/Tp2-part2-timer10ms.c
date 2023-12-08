/**
 * Desnyder Rémi / Videlaine Charles-Augustin
 * Objectifs: Réaliser un timer de 10ms
 * Fonctionnel
 * Limite : Aucune
 * Tests: 
 *  - Visualisation du délai de 10 ms sur l'oscilloscope (Changement de borne à chaque état 0 ou 1)
 * 
 * Cablage: 
 *  - D0 à D7 sur a à g
 *  - B0 à B3 sur Dis0 à Dis3
 *  - Dis0 sur Channel 1 (Pour l'oscilloscope)
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

/****************************************************************************/
/*		V A R I A B L E S    G L O B A L E S			                    */
/****************************************************************************/
unsigned char i;

/****************************************************************************/
/*		T R A I T E M E N T S   D' I N T E R R U P T I O N                  */
/****************************************************************************/

void  __interrupt(high_priority) myHighint(void)             // High priority interrupt
{
    if(TMR0IF) {                        // Si l'overflow du timer vaut 1
        if(portB0) portB0=0;            // Si led allumé alors éteindre
        else if(!portB0) portB0=1;      // Si led éteinte alors allumé
        
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