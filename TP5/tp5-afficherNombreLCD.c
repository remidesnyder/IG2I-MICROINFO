/**
 * Desnyder Rémi / Videlaine Charles-Augustin
 * Objectifs: 
 *  - Afficher un nombre sur un ecran LCD
 * Fonctionnel
 * Limite : 99 999
 * Tests: 
 *  - 0 affiche 00000
 *  - 105 Affiche 00105
 *  - 9905 Affiche 09905
 *  - 99999 Affiche 99999
 * 
 * Cablage: 
 *  - D0 à D7 (PIC) sur D0 à D7 (LCD)
 *  - B3 sur E (LCD)
 *  - B4 sur RW (LCD)
 *  - B5 sur RS (LCD)
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
/****************************************************************************/
/*		P R O T O T Y P E S   D E   F O N C T I O N S						*/
/****************************************************************************/
void initLCD(void);
char lecture(void);
void busy(void);
void ecrire(unsigned char, unsigned char);
void afficherNombreLCD(double);
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
    
    initLCD();
    
    double nombre = 12345;
    
    afficherNombreLCD(nombre);
 
    while(1) {
    }
    
    return;
}
	
/* Fin de la fonction main() */

/****************************************************************************/
/*		Z O N E   D E   S O U S   P R O G R A M M E S 						*/
/****************************************************************************/
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
    
    char chiffre[10] = {
        '0',
        '1',
        '2',
        '3',
        '4',
        '5',
        '6',
        '7',
        '8',
        '9'
    };
    
    ecrire(chiffre[dmillier], 1);
    ecrire(chiffre[millier], 1);
    ecrire(chiffre[centaine], 1);
    ecrire(chiffre[dizaine], 1);
    ecrire(chiffre[unite], 1);
    
}