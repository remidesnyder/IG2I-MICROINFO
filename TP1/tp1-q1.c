/**
 *	\file		tp1-q1.c
 *	\brief		
  *	\date		6er sept 2023
 *	\author		Desnyder Rémi / Videlaine Charmes-Augustin
 *	\version	0.1
 *
 *	\Note		
 * 
 * \ cablage
 */

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
#define pot0		PORTAbits.RA0

#define d0          PORTDbits.RD0
#define d1          PORTDbits.RD1         
#define d2          PORTDbits.RD2
#define d3          PORTDbits.RD3
#define d4          PORTDbits.RD4

#define pos1        PORTBbits.RB0




/****************************************************************************/
/*		P R O T O T Y P E S   D E   F O N C T I O N S						*/
/****************************************************************************/
void init(void);



/****************************************************************************/
/*		V A R I A B L E S    G L O B A L E S			                    */
/****************************************************************************/
	
unsigned char i;
char unite,dizaine,centaine,millier,dmillier;
char etat,flag,test;
unsigned int temps,affich;
/****************************************************************************/
/*		T R A I T E M E N T S   D' I N T E R R U P T I O N                  */
/****************************************************************************/

void  __interrupt(high_priority) myHighint(void)             // High priority interrupt
{
    if (INT0IF)  {
        
        if(d4) d4=0;
        else if(!d4) d4=1;
        
        INT0IF = 0;
        
        if(INTEDG0) INTCON2bits.INTEDG0 = 0;
        else if(!INTEDG0) INTCON2bits.INTEDG0 = 1;
        
    }
    
    if(TMR0IF) {
        if(d2) d2=0;
        else if(!d2) d2=1;
        
        TMR0IF = 0;
        
        TMR0H = 0x0B;
        TMR0L = 0xDC;
        
    }
    
}



/****************************************************************************/
/*		F O N C T I O N     P R I N C I P A L E 							*/
/****************************************************************************/
void main(void) {
    
    init();
    
while(1) {

    // La suite du programme s'effectue en boucle
    //PORTD = 0b00000001; // Allumer la LED D0
    d0 = 1;
    
    led1sec();

}
    return;
}
	
  /* Fin de la fonction main() */


/****************************************************************************/
/*		Z O N E   D E   S O U S   P R O G R A M M E S 							*/
/****************************************************************************/

	/*Mettre ici les sous programmes*/


/* Fin de la zone de sous programmes */
void init()
{	

    TRISD = 0b00000000; // Tout est en sortie
    
    d0 = 0;
    d1 = 0;
    d2 = 0;
    d3 = 0;
    d4 = 0;
    
    // Init interupteur
    TRISBbits.RB0 = 1; // Port 0 entrée
    INTCONbits.GIE = 1; // Activé global
    INTCONbits.INT0IE = 1; // Activé l'inte
    
    // Init timer
    INTCONbits.GIE = 1;
    INTCONbits.TMR0IE = 1;
    T08BIT = 0; // Timer 16 bits
    PSA = 0;
    T0CS = 0;
    T0PS2 = 0;
    T0PS1 = 1;
    T0PS0 = 0;
    
    TMR0H = 0x0B;
    TMR0L = 0xDC;
    
    TMR0ON = 1;
    
    
}

int tempo1sec() {
    // 1 op de base = 1 micro seconde
    // i = 10 (4microseconde) en 8bits / 2 octets
    for (unsigned int y = 0; y < 40000; y++) {
        int retard = 0;
    }
}

int led1sec() {
    tempo1sec();
    d1 = 1;
    tempo1sec();
    d1 = 0;
}
