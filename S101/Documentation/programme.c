/**
 * @file programmec
 * @brief Jeu snake en console de terminal. Version finale
 * 
 * Ce programme implémente un jeu du serpent.
 * Le joueur contrôle un serpent qui se déplace sur un plateau.
 * Le but est de manger un certain nombre de pommes
 * tout en évitant les obstacles
 * et les collisions avec les bordures (sauf les issues) 
 * ou avec le corps du serpent.
 * Le jeu se termine en cas de collision, de victoire (toutes les pommes mangées),
 * ou si le joueur déclare forfait.
 * 
 *  * @details
 * - Déplacement avec les touches : 'z' (haut), 'q' (gauche), 's' (bas), 'd' (droite).
 * - Une pomme apparaît aléatoirement sur le plateau et accélère le jeu lorsqu'elle est mangée.
 * - Le jeu se termine lorsque le serpent heurte un mur, un obstacle, ou lui-même.
 * - Les bordures et des pavés (obstacles fixes) rendent le jeu plus difficile.
 *
 * @author 
 * Arthur CHAUVEL
 * 
 * @version 4.0
 * @date 
 * 01/12/24
 *
 */

/*****************************************************
*               IMPORT DES BIBLIOTHEQUES             *
*****************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>

/*****************************************************
*DEFINITIONS CONSANTES/ VARIABLES GLOBALES/ FONCTIONS*
*****************************************************/


/*
 * @defgroup Constante du jeu
 * 
 */
/** Largeur maximale du plateau de jeu. */
#define LARGEURMAX 80
/** Hauteur maximale du plateau de jeu. */
#define HAUTEURMAX 40
/** Taille maximale que le serpent peut atteindre. */
#define MAXTAILLESERPENT 100 

const int COORDMIN = 1; /** Coordonnée minimale utilisée sur le plateau. */
const int TAILLESERPENT = 10; /** Taille initiale du serpent. */
const int NBREPAVE = 4; /** Nombre de pavés d'obstacles à placer. */
const int TAILLEPAVE = 5; /** Taille d'un pavé d'obstacle. */
const int TEMPORISATION = 200000; /** Temps de pause entre deux déplacements */
const int NBREPOMMESFINJEU = 10; /** Nombre de pommes à manger pour gagner. */
const int COORDXDEPART = 40; /** Coordonnée X de départ du serpent. */
const int COORDYDEPART = 20; 
const int STARTSAFEZONEX = 22; /** Début de la zone de sécurité en X. */
const int ENDSAFEZONEX = 43; /** Fin de la zone de sécurité en X. */
const int STARTSAFEZONEY = 12; /** Début de la zone de sécurité en Y. */
const int ENDSAFEZONEY = 23; /** Fin de la zone de sécurité en Y. */
const int AUGMENTATIONVITESSE = 15000; /** augmentation de la vitesse */
const char TETE = 'O'; /** Caractère représentant la tête du serpent. */
const char CORPS = 'X'; /** Caractère représentant le corps du serpent. */
const char POMME = '6'; /** Caractère représentant une pomme. */
const char ARRET = 'a'; /** Caractère permettant d'arrêter le jeu. */
const char DROITE = 'd'; /** Direction : droite. */
const char GAUCHE = 'q'; /** Direction : gauche. */
const char HAUT = 'z'; /** Direction : haut. */
const char BAS = 's'; /** Direction : bas. */
const char VIDE = ' '; /** Caractère représentant une case vide. */
const char CARBORDURE = '#'; /** Caractère représentant une bordure ou un obstacle. */

/** @typedef Plateau_de_jeu
* @brief Définition du plateau de jeu comme une matrice de caractères.
*/
char plateau[HAUTEURMAX +1][LARGEURMAX +1];


/** @brief Variables globales modifiables en cours de jeu. */
int tailleSerpent = TAILLESERPENT;
int temporisation = TEMPORISATION;
int posX_pomme = -1, posY_pomme = -1;

/** D&claration des fonctions */
void afficher(int x, int y, char c);
void effacer(int x, int y);
void initPlateau();
void ajouterPomme();
void placerPaves();
void dessinerPlateau(int lesX[], int lesY[]);
void progresser(int lesX[], int lesY[], char direction, bool *collision, bool *pommeMangee);
void gotoXY(int x, int y);
void disableEcho();
void enableEcho();
int kbhit();

/*****************************************************
*               PROGRAMME PRINCIPAL                  *
*****************************************************/

/*
 * @defgroup Porgramme principal
 * 
 */
/**
 * @brief Fonction principale du jeu.
 *
 * Initialise le jeu, gère la boucle principale, les événements clavier, 
 * et les conditions de fin de jeu. Met à jour le plateau et le serpent 
 * à chaque itération.
 *
 * @return Retourne EXIT_SUCCESS après l'arrêt du jeu.
 */
int main() {

    // Déclaration des variables
    int lesX[MAXTAILLESERPENT] = {40, 39, 38, 37, 36, 35, 34, 33, 32, 31};
    int lesY[MAXTAILLESERPENT] = {20, 20, 20, 20, 20, 20, 20, 20, 20, 20};
    char direction = DROITE;
    bool collision = false;
    bool pommeMangee = false;
    bool forfait = false;
    int pommesMangees = 0;

    //Appel des fonctions pour l'affichage du plateau, des pavés et de la première pomme 
    initPlateau();
    placerPaves();
    ajouterPomme();
    dessinerPlateau(lesX, lesY);

    disableEcho();

    // Boucle principale 
    while (!collision && pommesMangees < NBREPOMMESFINJEU) {
        if (kbhit()) {
            char touche = getchar();
            if ((touche == DROITE && direction != GAUCHE) ||
                (touche == GAUCHE && direction != DROITE) ||
                (touche == HAUT && direction != BAS) ||
                (touche == BAS && direction != HAUT)) {
                direction = touche;
            }
            if (touche == ARRET){
            forfait = true;
            break;
            }
        }

        progresser(lesX, lesY, direction, &collision, &pommeMangee);
        // gestion des modification lorqu'une pomme est mangée
        if (pommeMangee) {
            pommesMangees++;
            temporisation = temporisation - AUGMENTATIONVITESSE;
            tailleSerpent++;
            ajouterPomme();
        }
        dessinerPlateau(lesX, lesY);
        usleep(temporisation);
    }
    enableEcho();

    // Phrase de fin de jeu en fonction de l'issue de la partie
    if (collision) { //si collision 
        system("clear");
        printf("Collision détectée. Vous avez perdu.\n");
    }
    else if (pommesMangees == NBREPOMMESFINJEU) { // si victoire
        system("clear");
        printf("Vous avez gagné. Félicitations !\n");
    } 
    else if (forfait){ // si appui sur la touche de fin
        system("clear");
        printf("Vous avez déclaré forfait. Dommage !\n");
    }

    return EXIT_SUCCESS;
}

/*****************************************************
*               FONCTIONS/PROCEDURES                *
*****************************************************/

/*
 * @defgroup Fonctions et procédure "boite blanche"
 * 
 */
/**
 * @brief Affiche un caractère à une position donnée sur le terminal.
 * @param x Coordonnée en X.
 * @param y Coordonnée en Y.
 * @param c Caractère à afficher.
 */
void afficher(int x, int y, char c) {
    /** Sep placer aux coordonées x,y du terminal
     * puis écrire le caractère voulu à la bonne position
     */
    gotoXY(x, y);
    printf("%c", c);
}

/**
 * @brief Efface un caractère à une position donnée sur le terminal.
 * @param x Coordonnée en X.
 * @param y Coordonnée en Y.
 */
void effacer(int x, int y) {
    /** remplace le caractère present aux coordonnées x,y 
     * par du vide
     */
    afficher(x, y, VIDE);
}

/**
 * @brief Initialise le plateau avec les bordures et les issues.
 */
void initPlateau() {
    /** Double boucle for permettant de se déplacer sur la bordure du tableau 
     * en largeur et en hauteur et afficher la bordure 
     * sauf si le curseur est au milieu de la bordure
     */
    for (int i = 0; i < HAUTEURMAX; i++) {
        for (int j = 0; j < LARGEURMAX; j++) {
            if (i == 0 || i == HAUTEURMAX - 1) {
                plateau[i][j] = (j == LARGEURMAX / 2) ? VIDE : CARBORDURE;
            } else if (j == 0 || j == LARGEURMAX - 1) {
                plateau[i][j] = (i == HAUTEURMAX / 2) ? VIDE : CARBORDURE;
            } else {
                plateau[i][j] = VIDE;
            }
        }
    }
}

/**
 * @brief Place une pomme sur une case vide aléatoire.
 */
void ajouterPomme() {
    srand(time(NULL));
    while (plateau[posY_pomme][posX_pomme] != VIDE) {
        /** génère aléatoirement une coordonnée X */
        posX_pomme = rand() % (LARGEURMAX - 2) + 1; 
        /** génère aléatoirment une coordonnées Y */
        posY_pomme = rand() % (HAUTEURMAX - 2) + 1; 
    }
    /** place la pomme si les coordonnées sont valides */
    plateau[posY_pomme][posX_pomme] = POMME;
}

/**
 * @brief Place des pavés d'obstacles sur le plateau 
 * en dehors de la zone de sécurité.
 */
void placerPaves() {
    srand(time(NULL));
    for (int k = 0; k < NBREPAVE; k++) {
        int x, y;
        do {
            /** génère aléatoirement une coordonnée X */
            x = rand() % (LARGEURMAX - 10) + 2; 
            /** génère aléatoirement une coordonnées Y */
            y = rand() % (HAUTEURMAX - 10) + 2;
        } while (x > STARTSAFEZONEX && x < ENDSAFEZONEX && 
                 y > STARTSAFEZONEY && y < ENDSAFEZONEY);
        /** vérifie que le pavé ne va pas chevaucher le serpent */
        
        /** place le pavé si les coordonnées sont valide */
        for (int i = 0; i < TAILLEPAVE; i++) {
            for (int j = 0; j < TAILLEPAVE; j++) {
                plateau[y + i][x + j] = CARBORDURE;
            }
        }
    }
}

/**
 * @brief Dessine le plateau entier avec le serpent et les obstacles.
 * @param lesX Tableau des coordonnées x du serpent.
 * @param lesY Tableau des coordonnées y du serpent.
 */
void dessinerPlateau(int lesX[], int lesY[]) {
    /** affiche le serpent dans le terminal */
    for (int i = 0; i < tailleSerpent; i++) {
        plateau[lesY[i]][lesX[i]] = (i == 0) ? TETE : CORPS;
    }
    system("clear");
    /** affiche le plateau déja initialisé dans le terminal de jeu */
    for (int i = 0; i < HAUTEURMAX; i++) {
        for (int j = 0; j < LARGEURMAX; j++) {
            putchar(plateau[i][j]);
        }
        putchar('\n');
    }
}

/**
 * @brief Fait progresser le serpent d'une étape.
 * @param lesX Tableau des coordonnées X du serpent.
 * @param lesY Tableau des coordonnées Y du serpent.
 * @param direction Direction actuelle du serpent.
 * @param collision Indique si une collision a été détectée.
 * @param pommeMangee Indique si une pomme a été mangée.
 */
void progresser(int lesX[], int lesY[], char direction, bool *collision, bool *pommeMangee) {
    /** effacer le dernier segment du seprent
     * pour monter qu'il avance
     */
    int X = lesX[tailleSerpent - 1];
    int Y = lesY[tailleSerpent - 1];
    plateau[Y][X] = VIDE;

    for (int i = tailleSerpent - 1; i > 0; i--) {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }
    /** change la coordonnée adéquate du seprent en fonction de la  direction*/
    if (direction == DROITE) lesX[0]++;
    if (direction == GAUCHE) lesX[0]--;
    if (direction == HAUT) lesY[0]--;
    if (direction == BAS) lesY[0]++;

    /** gestion de la réapparition du seprent lorsqu'il emprunte une issue */
    if (lesX[0] == 0 && lesY[0] == HAUTEURMAX / 2) lesX[0] = LARGEURMAX - 2;
    else if (lesX[0] == LARGEURMAX - 1 && lesY[0] == HAUTEURMAX / 2) lesX[0] = 1;
    else if (lesY[0] == 0 && lesX[0] == LARGEURMAX / 2) lesY[0] = HAUTEURMAX - 2;
    else if (lesY[0] == HAUTEURMAX - 1 && lesX[0] == LARGEURMAX / 2) lesY[0] = 1;

    /** Gestion des collisions avec le plateau, les pavés et le corps du serpent */
    *collision = plateau[lesY[0]][lesX[0]] == CARBORDURE || 
        plateau[lesY[0]][lesX[0]] == CORPS;
    *pommeMangee = (lesX[0] == posX_pomme && lesY[0] == posY_pomme);
}

/*****************************************************
*            FONCTIONS "BOITES NOIRES"               *
*****************************************************/

/*
 * @defgroup Fonction et procédure "Boite noire"
 * 
 */
void gotoXY(int x, int y) {
    printf("\033[%d;%df", y, x);
}

void disableEcho() {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

void enableEcho() {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

int kbhit() {
    struct termios oldt, newt;
    int ch, oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
} 