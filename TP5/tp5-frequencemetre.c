/**
 * Desnyder Rémi / Videlaine Charles-Augustin
 * Objectifs: 
 *  - Réaliser un frequencemetre qui affiche toutes les secondes le résultats du compteur stocker en ram
 * Fonctionnel
 * Limite : 14 200 Hz pour garder une précision
 * Tests: 
 *  - De 0 à 14 200 Hz (Affiche la fréquence correct)
 *  - Au dessus de 14 200 Hz, la fréquence n'est pas correct
 * 
 * Cablage: 
 *  - D0 à D7 (PIC) sur D0 à D7 (LCD)
 *  - B3 sur E (LCD)
 *  - B4 sur RW (LCD)
 *  - B5 sur RS (LCD)
 *  - B0 sur Channel 2
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
#define E PORTBbits.RB3 // Constance E sur B3
#define RW PORTBbits.RB4 // Constance RW sur B4
#define RS PORTBbits.RB5 // Constance RS sur B5

#define B0 PORTBbits.RB0 // Constance E0 sur E0
/****************************************************************************/
/*		P R O T O T Y P E S   D E   F O N C T I O N S						*/
/****************************************************************************/
void init(void);
void initTimer(void);
void initInterupt(void);
void initLCD(void);
char lecture(void);
void busy(void);
void ecrire(unsigned char, unsigned char);
void afficherNombreLCD(double);
/****************************************************************************/
/*		V A R I A B L E S    G L O B A L E S			                    */
/****************************************************************************/
int flagTimer = 0;
int compteurFrequence = 0;
/****************************************************************************/
/*		T R A I T E M E N T S   D' I N T E R R U P T I O N                  */
/****************************************************************************/

void  __interrupt(high_priority) myHighint(void)             // High priority interrupt
{
    if(TMR0IF) {                        // Si l'overflow du timer vaut 1
        if(flagTimer) flagTimer=0;            // Si  allumé alors éteindre
        else if(!flagTimer) flagTimer=1;      // Si éteinte alors allumé
        
        afficherNombreLCD(compteurFrequence);
        compteurFrequence = 0;
        
        TMR0IF = 0;
        
        TMR0H = 0x0B; // Bit haut
        TMR0L = 0xDC; // Bit bas
        
    }
    
    if(INT0IF) {
        INT0IF = 0; // Flag interrupteur = 0
        
        if(INTEDG0) { 
            INTCON2bits.INTEDG0 = 0; 
            compteurFrequence++;
        }
        else if(!INTEDG0) INTCON2bits.INTEDG0 = 1;
    }
}


/****************************************************************************/
/*		F O N C T I O N     P R I N C I P A L E 							*/
/****************************************************************************/

void main(void) {
    
    init();
    
    afficherNombreLCD(compteurFrequence);
       
    while(1) {
    }
    
    return;
}
	
/* Fin de la fonction main() */

/****************************************************************************/
/*		Z O N E   D E   S O U S   P R O G R A M M E S 						*/
/****************************************************************************/
void init() {	   
    
    initLCD();
    initInterupt();
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

void initInterupt() {
    
    INTCONbits.GIE = 1;
    INTCONbits.INT0IE = 1; // Activé l'inte
    
    TRISBbits.RB0 = 1; // En entrée
}

void initLCD() {	
    TRISB = 0; // Tout est en sortie
    ecrire(0x38, 2); // MODE 8 bits, 2 lignes
    // Détails : 
    // Fonction -> D7 et D6 = 0, D5 = 1, D4 = DL = 1 (8 bits), D3 = N = 1 (2 lignes)
    // D2 = F = 0 (defaut), D1 = D0 = *
    ecrire(0x01, 0); // Efface l'affichage
    ecrire(0x0E, 0); // Met en marche l'affichage et le curseur
    // Détails :
    // MARCHE/ARRET DE L'AFFICHAGE DU CURSEUR   
    // 0 De D7 à D4
    // D3 = 1, D2 = 1 (Affichage ON); D1 = 1 (Curseur ON); D0 = 0 (Clignotement OFF)
    ecrire(0x06, 0); // Sens curseur, affichage
    // Détails :
    // MANIERE DE VISUALISER LES CARACTERES
    // 0 De D7 à D3
    // D2 = 1, D1 = ID = 1 (Déplacement vers la droite), D0 = S = 1 (Affichage décalé)
}

void ecrire(unsigned char valeur, unsigned char param) {
    // Si param = 0 -> Ecrire commande
    // Si param = 1 -> Ecrire une donnée
    // Si param = 2 -> Ecrire commande sans busy
    if (param != 2) { 
        busy(); 
    }
    
    if (param == 2) { param = 0; }
    
    RS = param;
    RW = 0; // 0 = ecriture
    // Attendre 140ns (Pas besoin car fréquence basse)
    TRISD = 0; // Tout est en sortie
    PORTD = valeur;
    E = 1;
    E = 1;
    E = 1;
    // Attendre 450ns (Pas besoin car fréquence basse)
    E = 0;
}

void busy() {
    // Permet d'attendre que l'écran à fini la commande précédente
    char valeur = 0x80;
    while (valeur == 0x80) {
        valeur = lecture();
        valeur = valeur & 0x80;
    }
}

char lecture() {
    // Lecture d'une commande seulement
    char valeur;
    RS = 0; // 0 = commande
    RW = 1; // 1 = lecture
    // Attendre 140ns (Pas besoin car fréquence basse)
    TRISD = 0b11111111; // Tout est en entrée
    E = 1;
    E = 1;
    valeur = PORTD;
    E = 0;
    
    return valeur;
}

void afficherNombreLCD(double resultat) {
    
    int dmillier = 0;
    int millier = 0;
    int centaine = 0;
    int dizaine = 0;
    int unite = 0;
    
    while (resultat >= 10000) {
        dmillier++;
        resultat = resultat - 10000;
    }
    
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

    ecrire(1, 0);
    
    //ecrire(chiffre[dmillier], 1);
    //ecrire(chiffre[millier], 1);
    //ecrire(chiffre[centaine], 1);
    //ecrire(chiffre[dizaine], 1);
    //ecrire(chiffre[unite], 1);
    
    ecrire(dmillier + 48, 1);
    ecrire(millier + 48, 1);
    ecrire(' ', 1);
    ecrire(centaine + 48, 1);
    ecrire(dizaine + 48, 1);
    ecrire(unite + 48, 1);
    
    ecrire(' ', 1);
    ecrire('H', 1);
    ecrire('z', 1);
}