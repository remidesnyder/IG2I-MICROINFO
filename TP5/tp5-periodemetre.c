/**
 * Desnyder Rémi / Videlaine Charles-Augustin
 * Objectifs: 
 *  - Réaliser un periodemetre qui affiche toutes les secondes le résultats du compteur stocker en ram
 * Fonctionnel
 * Limite : 
 *  - Décalage de 7 microS mais corrigé avec l'ajout de 7 microS dans le programme
 *  - 65 ms maximun
 * Tests: 
 *  - De 0 à 65 ms -> Affichage correct
 *  - Au dessus, de 65 ms le compteur n'affiche plus les bonnes valeurs
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
int tempo(void);
/****************************************************************************/
/*		V A R I A B L E S    G L O B A L E S			                    */
/****************************************************************************/
int flagTimer = 0;
int flagTimerD = 0;
int compteurFrequence = 0;
unsigned int microCompteur = 0;
int microCompteurH = 0;
int microCompteurL = 0;
/****************************************************************************/
/*		T R A I T E M E N T S   D' I N T E R R U P T I O N                  */
/****************************************************************************/
void  __interrupt(high_priority) myHighint(void)             // High priority interrupt
{
    
    
    if(INT0IF) {

        microCompteurH = TMR1H;
        microCompteurL = TMR1L;
        
        TMR1H = 0x00;
        TMR1L = 0x00;
        
        INT0IF = 0;
        
    }
}

/****************************************************************************/
/*		F O N C T I O N     P R I N C I P A L E 							*/
/****************************************************************************/
int tempo() {
    // 1 op de base = 1 micro seconde
    // i = 10 (4microseconde) en 8bits / 2 octets
    for (unsigned int y = 0; y < 9500; y++) {
        int retard = 0;
    }
}

void main(void) {

    init();
    
    while(1) {tempo();           // Si  allumé alors éteindre
        
        microCompteur = microCompteurH;
        microCompteur = (microCompteur << 8) + microCompteurL + 7;
    
        afficherNombreLCD(microCompteur);
        microCompteur = 0;
        
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
    // Init timer 0
    INTCONbits.GIE = 1;
    INTCONbits.TMR0IE = 0;
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
    
    
    
    
    // Init timer 1
   
    //RD16 = 1; // Timer 16 bits (0 = 16bit)

    TMR1CS = 0; // 0 clock interne
    RD16 = 0;
    
    // Timer0 Prescaler Select bits
    // Ici 0 bits 
    T1CKPS1 = 0;
    T1CKPS0 = 0;
            
    // Décalage de départ
    TMR1H = 0x00; // Bit haut
    TMR1L = 0x00; // Bit bas
    T1CON=0;
    TMR1ON = 1; // Timer0 activé
}

/**
 * Calcul du délai du timer 1 (1S) 
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

/**
 * Calcul du délai du timer 2 (1 ms) 
 * 
 * Max: 65 536 x 10^-3 ms
 * 
 * 1 micros
 * 
 * Calcul prescalaire
 * 1 / 65 536 = 0 Donc pas de presca
 * 1 / 1 = 1
 * 65 536 - 1 = 65 535 -> FFFF
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
    
    ecrire(dmillier + 48, 1);
    ecrire(millier + 48, 1);
    ecrire(' ', 1);
    ecrire(centaine + 48, 1);
    ecrire(dizaine + 48, 1);
    ecrire(unite + 48, 1);
    
    ecrire(' ', 1);
    ecrire('M', 1);
    ecrire('i', 1);
    ecrire('c', 1);
    ecrire('r', 1);
    ecrire('o', 1);
    ecrire('S', 1);
}