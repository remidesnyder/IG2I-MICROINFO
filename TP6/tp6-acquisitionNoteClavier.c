/**
 * Desnyder Rémi / Videlaine Charles-Augustin
 * Objectifs: 
 *  - Acquisition d'une note au clavier
 * Fonctionnel
 * Limite : Une gamme de tons sur le clavier
 * Tests: 
 *  - Note de 1 à 8 sur le clavier
 * Cablage: 
 *  - C2 sur Buzzer
 *  - Buzzer sur Channel 1
 * 
 *  Clavier
 *  - ROW1 sur A1
 *  - ROW2 sur A2
 *  - ROW3 sur A3
 *  - ROW4 sur A0
 * 
 *  - C1 sur E2
 *  - C2 sur E1
 *  - C3 sur E0
 *  - C4 sur C0
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

#define ROW1 PORTAbits.RA3
#define ROW2 PORTAbits.RA2
#define ROW3 PORTAbits.RA1
#define ROW4 PORTAbits.RA0

#define C1Clavier PORTEbits.RE2
#define C2Clavier PORTEbits.RE1
#define C3Clavier PORTEbits.RE0
#define C4Clavier PORTCbits.RC0
/****************************************************************************/
/*		P R O T O T Y P E S   D E   F O N C T I O N S						*/
/****************************************************************************/
void init(void);
void initClavier(void);
void initTimer(void);
void jouerNote(int);
int traitementClavier(void);
/****************************************************************************/
/*		V A R I A B L E S    G L O B A L E S			                    */
/****************************************************************************/
double noteEnT[8] = {
    238.5,
    212.5,
    189.25,
    179,
    159.5,
    142,
    126.5,
    119.25
};
int indiceNote = 0;
/****************************************************************************/
/*		T R A I T E M E N T S   D' I N T E R R U P T I O N                  */
/****************************************************************************/
void  __interrupt(high_priority) myHighint(void)             // High priority interrupt
{
    if(TMR0IF) {                        // Si l'overflow du timer vaut 1
        
        jouerNote(-1);
        
        TMR0IF = 0;
        
        TMR0H = 0x0B; // Bit haut
        TMR0L = 0xDC; // Bit bas
        
    }
}
/****************************************************************************/
/*		F O N C T I O N     P R I N C I P A L E 							*/
/****************************************************************************/

void main(void) {

    int valeurClavier;
    
    init();
    
    while(1) {
        valeurClavier = traitementClavier();
        jouerNote(noteEnT[valeurClavier]);
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
    
    initClavier();
    
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
    
    //TMR0ON = 1; // Timer0 activé
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

void initClavier() {
    TRISAbits.RA0 = 0; // En sortie
    TRISAbits.RA1 = 0; // En sortie
    TRISAbits.RA2 = 0; // En sortie
    TRISAbits.RA3 = 0; // En sortie
    
    ADCON1= 0x06; // On met tout en numérique
    
    TRISEbits.TRISE0 = 1; // En entrée
    TRISEbits.TRISE1 = 1; // En entrée
    TRISEbits.TRISE2 = 1; // En entrée
    TRISCbits.TRISC0 = 1; // En entrée
}

int traitementClavier() {
    
    while(1) {
        // Traitement ligne 1
        ROW1 = 1; // On donne 5V à Row1

        if (C1Clavier == 1) { while(C1Clavier == 1); return 6; }
        else if (C2Clavier == 1) { while(C2Clavier == 1); return 7; }
        //else if (C3Clavier == 1) { while(C3Clavier == 1); return 2; }

        ROW1 = 0;

        // Traitement ligne 2
        ROW2 = 1; // On donne 5V à Row1

        if (C1Clavier == 1) { while(C1Clavier == 1); return 3; }
        else if (C2Clavier == 1) { while(C2Clavier == 1); return 4; }
        else if (C3Clavier == 1) { while(C3Clavier == 1); return 5; }

        ROW2 = 0;

        // Traitement ligne 3
        ROW3 = 1; // On donne 5V à Row1

        if (C1Clavier == 1) { while(C1Clavier == 1); return 0; }
        else if (C2Clavier == 1) { while(C2Clavier == 1); return 1; }
        else if (C3Clavier == 1) { while(C3Clavier == 1); return 2; }
        //else if (C4 == 1) { while(C4 == 1); return 'C'; }

        ROW3 = 0;
        
        /*
        // Traitement ligne 4
        ROW4 = 1; // On donne 5V à Row1

        if (C1 == 1) { while(C1 == 1); return '*'; }
        else if (C2 == 1) { while(C2 == 1); return '0'; }
        else if (C3 == 1) { while(C3 == 1); return '#'; }
        else if (C4 == 1) { while(C4 == 1); return 'D'; }

        ROW4 = 0;
        */
    }
}

void jouerNote(int note) {
    
    if (note >= 0) {
        
        TMR0ON = 0; // Timer0 désactivé
        TMR0H = 0x0B; // Bit haut
        TMR0L = 0xDC; // Bit bas
        TMR0ON = 1; // Timer0 activé
        
        PR2 = note - 1;
        CCPR1L = ((note - 1) / 2);
    } else {
        PR2 = 0;
        CCPR1L = 0;
        TMR0ON = 0; // Timer0 désactivé
        TMR0H = 0x0B; // Bit haut
        TMR0L = 0xDC; // Bit bas
    }
    
      
    
}