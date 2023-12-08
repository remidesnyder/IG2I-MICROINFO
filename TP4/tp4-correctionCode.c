/**
 * Desnyder Rémi / Videlaine Charles-Augustin
 * Objectifs: 
 *  - Taper un code à 4 chiffres et le vérifier (Code bon ou code faux)
 *  - Faire * pour corriger le dernier chiffre
 *  - Faire # pour recommencer
 * Fonctionnel
 * Limite : 
 *  - Code à 4 chiffres
 * Tests: 
 *  - Différents codes dont le bon (Validé)
 *  - * pour corriger le dernier caractère
 *  - # pour recommencer
 * 
 * Cablage: 
 *  - D0 à D7 (PIC) sur D0 à D7 (LCD)
 *  - B3 sur E (LCD)
 *  - B4 sur RW (LCD)
 *  - B5 sur RS (LCD)
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
#define E PORTBbits.RB3 // Constance E sur B3
#define RW PORTBbits.RB4 // Constance RW sur B4
#define RS PORTBbits.RB5 // Constance RS sur B5

#define ROW1 PORTAbits.RA3
#define ROW2 PORTAbits.RA2
#define ROW3 PORTAbits.RA1
#define ROW4 PORTAbits.RA0

#define C1 PORTEbits.RE2
#define C2 PORTEbits.RE1
#define C3 PORTEbits.RE0
#define C4 PORTCbits.RC0
/****************************************************************************/
/*		P R O T O T Y P E S   D E   F O N C T I O N S						*/
/****************************************************************************/
void initLCD(void);
void initClavier(void);
char lecture(void);
void busy(void);
void ecrire(unsigned char, unsigned char);
char traitementClavier(void);
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
    
    char valeurClavier; // Valeur recuperée
    char code[4];       // code défini
    char codeEssai[4];  // code rentré
    int essai = 0;      // essai pour tableau
    
    code[0] = '1';
    code[1] = '2';
    code[2] = '3';
    code[3] = '4';
    
    initLCD();
    initClavier();
    
    // Texte accueil
    ecrire('C', 1);
    ecrire('o', 1);
    ecrire('d', 1);
    ecrire('e', 1);
    ecrire(' ', 1);
    ecrire('?', 1);
    ecrire(' ', 1);
    
    while(1) {
        valeurClavier = traitementClavier(); // Recup valeur
        //ecrire(valeurClavier, 1);
        
        if (valeurClavier != '#' && valeurClavier != '*') {
            ecrire('*', 1);
            codeEssai[essai] = valeurClavier; // On remplit le tableau
            essai++;

            if (essai == 4) {
                for (int t = 0; t < 29; t++) {
                    ecrire(' ', 1); // Pour passer à la 2eme ligne
                }
                if (
                        (codeEssai[0] == code[0]) &&
                        (codeEssai[1] == code[1]) &&
                        (codeEssai[2] == code[2]) &&
                        (codeEssai[3] == code[3])
                    ) {
                    ecrire('C', 1);
                    ecrire('o', 1);
                    ecrire('d', 1);
                    ecrire('e', 1);
                    ecrire(' ', 1);
                    ecrire('b', 1);
                    ecrire('o', 1);
                    ecrire('n', 1);
                } else {
                    ecrire('C', 1);
                    ecrire('o', 1);
                    ecrire('d', 1);
                    ecrire('e', 1);
                    ecrire(' ', 1);
                    ecrire('f', 1);
                    ecrire('a', 1);
                    ecrire('u', 1);
                    ecrire('x', 1);
                }
            }
        } else if (valeurClavier == '*') {
            ecrire(0x01, 0); // Efface l'affichage
            
            // Texte accueil
            ecrire('C', 1);
            ecrire('o', 1);
            ecrire('d', 1);
            ecrire('e', 1);
            ecrire(' ', 1);
            ecrire('?', 1);
            ecrire(' ', 1);
            
            essai--;
            
            for(int i = 0; i < essai; i++) {
                ecrire('*', 1);
            }
        } else if (valeurClavier == '#'){
            ecrire(0x01, 0); // Efface l'affichage
            
            // Texte accueil
            ecrire('C', 1);
            ecrire('o', 1);
            ecrire('d', 1);
            ecrire('e', 1);
            ecrire(' ', 1);
            ecrire('?', 1);
            ecrire(' ', 1);
            
            essai = 0;
        }
       
        
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

char traitementClavier() {
    
    while(1) {
        // Traitement ligne 1
        ROW1 = 1; // On donne 5V à Row1

        if (C1 == 1) { while(C1 == 1); return '1'; }
        else if (C2 == 1) { while(C2 == 1); return '2'; }
        else if (C3 == 1) { while(C3 == 1); return '3'; }
        else if (C4 == 1) { while(C4 == 1); return 'A'; }

        ROW1 = 0;

        // Traitement ligne 2
        ROW2 = 1; // On donne 5V à Row1

        if (C1 == 1) { while(C1 == 1); return '4'; }
        else if (C2 == 1) { while(C2 == 1); return '5'; }
        else if (C3 == 1) { while(C3 == 1); return '6'; }
        else if (C4 == 1) { while(C4 == 1); return 'B'; }

        ROW2 = 0;

        // Traitement ligne 3
        ROW3 = 1; // On donne 5V à Row1

        if (C1 == 1) { while(C1 == 1); return '7'; }
        else if (C2 == 1) { while(C2 == 1); return '8'; }
        else if (C3 == 1) { while(C3 == 1); return '9'; }
        else if (C4 == 1) { while(C4 == 1); return 'C'; }

        ROW3 = 0;
        
        // Traitement ligne 4
        ROW4 = 1; // On donne 5V à Row1

        if (C1 == 1) { while(C1 == 1); return '*'; }
        else if (C2 == 1) { while(C2 == 1); return '0'; }
        else if (C3 == 1) { while(C3 == 1); return '#'; }
        else if (C4 == 1) { while(C4 == 1); return 'D'; }

        ROW4 = 0;
    }
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
