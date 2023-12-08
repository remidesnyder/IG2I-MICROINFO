/**
 * Desnyder Rémi / Videlaine Charles-Augustin
 * Objectifs: 
 *  - Jouer une musique
 * Fonctionnel
 * Limite : Aucune
 * Tests: 
 *  - Jouer la musique et vérifications à l'oreille
 * Cablage: 
 *  - C2 sur Buzzer
 *  - Buzzer sur Channel 1
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
#define portC0 PORTCbits.RC0
/****************************************************************************/
/*		P R O T O T Y P E S   D E   F O N C T I O N S						*/
/****************************************************************************/
void init(void);
void initTimer(void);
void jouerNote(int);
void jouerMelodie(void);
/****************************************************************************/
/*		V A R I A B L E S    G L O B A L E S			                    */
/****************************************************************************/
double noteEnT[9] = {
    253,
    238.5,
    212.5,
    189.5,
    179,
    159.5,
    142,
    126.5,
    119.25
};
int indiceNote = 0;
int flag = 0;
int flagMelo = 0;
/****************************************************************************/
/*		T R A I T E M E N T S   D' I N T E R R U P T I O N                  */
/****************************************************************************/
void  __interrupt(high_priority) myHighint(void)             // High priority interrupt
{
    if(TMR0IF) {                        // Si l'overflow du timer vaut 1
        
        flag = 0;

        TMR0ON = 0;
        
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
    //jouerMelodie(); 
    
    while(1) {
        if (flagMelo == 0) jouerMelodie();  
    }
    
    return;
}
	

/* Fin de la fonction main() */
/****************************************************************************/
/*		Z O N E   D E   S O U S   P R O G R A M M E S 						*/
/****************************************************************************/
void init() {	 
    
    // Init timer
    initTimer();
    
    // Régler T -> PR2 (à 249 us) 
    // Attention, il y a toujours un +1 automatiquement
    PR2 = 0;
    
    // Règler Th (2.75 us)
    // CCPR1L et DC1B1 / DC1B0
    CCPR1L = 2;
    DC1B1 = 1; // 0.5
    DC1B0 = 1; // 0.25
    
    // CCP1 -> RC2 (En sortie)
    TRISCbits.RC2 = 0;
    TRISCbits.RC0 = 0;
    
    // Configurer T2CON
    T2CKPS1 = 1; // Prescaler de 16
    T2CKPS0 = 0;
            
    // CCP1CON mode PWM
    CCP1M3 = 1;
    CCP1M2 = 1;
    CCP1M1 = 1;
    CCP1M0 = 1;
    
    TMR2ON = 1; // Activer le timer
}

void initTimer() {	
    // Init timer
    INTCONbits.GIE = 1;
    INTCONbits.TMR0IE = 1;
    T08BIT = 0; // Timer 16 bits (0 = 16bit)
    PSA = 0; // Prescaler activé (0 = on)
    T0CS = 0; // 0 clock interne
    
    // Timer0 Prescaler Select bits
    // Ici 8 bits 
    T0PS2 = 0;
    T0PS1 = 1;
    T0PS0 = 0;
    
    // Décalage de départ
    TMR0H = 0x0B; // Bit haut
    TMR0L = 0xDC; // Bit bas
    
    
}
/**
 * Calcul du délai
 * 
 * Max: 65 536 x 10^-3 ms
 * 
 * 0.5s = 500 000 microSecondes
 * 
 * Calcul prescalaire
 * 500 000 / 65 536 = 7.62 Donc presca de 8
 * 500 000 / 8 = 62 500
 * 65 536 - 62 500 = 3 036 -> 0BDC
 * 
*/

void jouerNote(int note) {
    if (note >= 0) {
        note = noteEnT[note];
        PR2 = note - 1;
        CCPR1L = ((note - 1) / 2);
        TMR0IF = 0;
        TMR0H = 0x0B; // Bit haut
        TMR0L = 0xDC; // Bit bas
        TMR0ON = 1; // Timer0 activé
        flag = 1;
    } else {
        PR2 = 0;
        CCPR1L = 0;
        TMR0ON = 0; // Timer0 désactivé
        TMR0H = 0x0B; // Bit haut
        TMR0L = 0xDC; // Bit bas
    }
}

void jouerMelodie() {
    flagMelo = 1;
    jouerNote(5);
    while (flag == 1);
    jouerNote(3);
    while (flag == 1);
    jouerNote(2);
    while (flag == 1);
    jouerNote(3);
    while (flag == 1);
    jouerNote(2);
    while (flag == 1);
    jouerNote(1);
    while (flag == 1);
    jouerNote(2);
    while (flag == 1);
    jouerNote(1);
    while (flag == 1);
    jouerNote(3);
    while (flag == 1);
    jouerNote(1);
    while (flag == 1);
    jouerNote(0);
    while (flag == 1);
    jouerNote(1);
    while (flag == 1);
    jouerNote(0);
    
    while (flag == 1);
    jouerNote(-1);
    flagMelo = 0;
}