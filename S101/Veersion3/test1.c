/**
 * @file version3.c
 * @brief Implémentation de la version 4 du jeu Snake pour le projet SAE1.01.
 * 
 * Ce fichier contient le code de la troisième version du jeu Snake. 
 * Les fonctionnalités incluent :
 * - Affichage des bordures.
 * - Placement de pavés aléatoires dans la zone de jeu.
 * - Gestion des collisions (bordures, pavés, serpent lui-même).
 * - Déplacement initial du serpent vers la droite jusqu'à appui sur une touche directionnelle.
 * - Lise en places d'un systeme de pommes e donc de fin jeu
 * 
 * Le jeu se termine si le joueur appuie sur 'a' ou si une collision est détectée.
 * 
 * @author Arthur CHAUVEL
 * @version 4
 * @date 15/11/24
 */
 /** 
  * import des bilbliothèques nécéssaires au bon fonctionnement du code
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdbool.h>
#include <time.h>

/** @brief Constantes globales. */

const int COORDMIN = 1;          /**< Coordonnée minimale sur le plateau. */
const int LARGEURMAX = 80;       /**< Largeur maximale du plateau. */
const int LONGUEURMAX = 40;      /**< Longueur maximale du plateau. */
const int TAILLEPAVE = 5;        /**< Taille des pavés (carrés). */
const int NBREPAVES = 4;         /**< Nombre de pavés à placer sur le plateau. */
const int COORDXDEPART = 40;    /**< Position de départ en X du serpent. */
const int COORDYDEPART = 20;    /**< Position de départ en Y du serpent. */
const int TAILLESERPENT = 10;    /**< Taille initiale du serpent. */
const int TEMPORISATION = 200000; /**< Temporisation entre les déplacements en microsecondes. */
const int COMPTEURFINJEU = 10;    /** compteur de nombre de pommes que le serpent doit manger pour que la patie sot gagné */
const int COORDCENTREX = 20;    /** coordonée x du centre des cotés haut et bas du plateau */
const int COORDCENTREY = 40;    /** coordonnée y du centre des cotés droite et gauche du plateau */
const char POMME = '6';     /** caractere représentant une pomme */
const char CARBORDURE = '#';     /**< Caractère utilisé pour afficher les bordures et pavés. */
const char VIDE = ' ';            /**< Caractère représentant une case vide. */
const char TETE = 'O';            /**< Caractère représentant la tête du serpent. */
const char CORPS = 'X';           /**< Caractère représentant le corps du serpent. */
const char HAUT = 'z';            /**< Touche pour déplacer le serpent vers le haut. */
const char BAS = 's';             /**< Touche pour déplacer le serpent vers le bas. */
const char GAUCHE = 'q';          /**< Touche pour déplacer le serpent à gauche. */
const char DROITE = 'd';          /**< Touche pour déplacer le serpent à droite. */
const char FINJEU = 'a';         /**< Touche pour arrêter le jeu. */

/** @typedef plateau_de_jeu
 * @brief Définition du plateau de jeu comme une matrice de caractères.
 */
typedef char plateau_de_jeu[LARGEURMAX + 1][LONGUEURMAX + 1];

plateau_de_jeu plateau; /**< Plateau de jeu global. */
int compteurPomme = 0; /**variable compteur de nombre de pommes mangés global */
int tailleSerpent = TAILLESERPENT;
int temporisation = TEMPORISATION;

/* Prototypes des fonctions */
void afficher(int x, int y, char c);
void effacer(int x, int y);
void initPlateau(int lesX[], int lesY[]);
void initPaves(int lesX[], int lesY[]);
void affichagePlateau(plateau_de_jeu plateau);
void dessinerSerpent(int lesX[], int lesY[]);
void progresser(int lesX[], int lesY[], char direction, bool *colision, bool *mangerPomme);
void ajouterPomme(int lesX[], int lesY[]);
void gotoXY(int x, int y);
int kbhit(void);
void disableEcho();
void enableEcho();

/**
 * @brief Programme principal du jeu Snake.
 * 
 * Initialise le plateau de jeu, place le serpent et les pavés, puis lance la boucle principale.
 * 
 * @return EXIT_SUCCESS en cas de succès.
 */
int main(){

    srand(time(NULL));
    int x, y;
    int lesX[tailleSerpent], lesY[tailleSerpent];
    char touche = DROITE;
    char direction = DROITE;

    initPlateau(lesX, lesY);
    system("clear");
    disableEcho();

    bool collision = false;
    bool mangerPomme = false;

    x = COORDXDEPART;
    y = COORDYDEPART;
    for (int i = 0; i < tailleSerpent; i++)
    {
        lesX[i] = x--;
        lesY[i] = y;
    }
    affichagePlateau(plateau);
    ajouterPomme(lesX, lesY);

    do
    {
        if (kbhit())
        {
            touche = getchar();
        }

        if (touche == HAUT && direction != BAS) {
            direction = HAUT;
        }

        else if ((touche == BAS) && (direction != HAUT)) {
            direction = BAS;
        }

        else if ((touche == GAUCHE) && (direction != DROITE)) {
            direction = GAUCHE;
        }

        else if ((touche == DROITE) && (direction != GAUCHE)) {
            direction = DROITE;
        }

        progresser(lesX, lesY, direction, &collision ,&mangerPomme);
        usleep(TEMPORISATION);

    } while ((touche != FINJEU) && (collision != true));

    if (compteurPomme > COMPTEURFINJEU){
        if ((lesX[tailleSerpent - 1] == LONGUEURMAX) && (lesY[tailleSerpent - 1] == COORDCENTREY)){
            system("clear");
            printf("Bravo vouus avez gagné !\n");
        }
        else if((lesX[tailleSerpent - 1] == COORDMIN) && (lesY[tailleSerpent - 1] == COORDCENTREY)){
            system("clear");
            printf("Bravo vouus avez gagné !\n");
        }
        else if ((lesX[tailleSerpent - 1] == COORDCENTREX) && (lesY[tailleSerpent - 1] == LARGEURMAX)){
            system("clear");
            printf("Bravo vouus avez gagné !\n");
        }
        else if ((lesX[tailleSerpent - 1] == COORDCENTREX) && (lesY[tailleSerpent - 1] == COORDMIN)){
            system("clear");
            printf("Bravo vouus avez gagné !\n");
        }
    }

    enableEcho();
    system("clear");
    return EXIT_SUCCESS;
}

/**
 * @brief Affiche un caractère à une position donnée sur le terminal.
 * 
 * @param x Coordonnée en X.
 * @param y Coordonnée en Y.
 * @param c Caractère à afficher.
 */
void afficher(int x, int y, char c) {
    if (((y >= COORDMIN) && (y <= LONGUEURMAX + 1)) && ((x >= COORDMIN) && (x <= LARGEURMAX + 1))) {
        gotoXY(x, y);
        printf("%c", c);
    }
}

/**
 * @brief Efface un caractère à une position donnée sur le terminal.
 * 
 * @param x Coordonnée en X.
 * @param y Coordonnée en Y.
 */
void effacer(int x, int y) {
    afficher(x, y, VIDE);
}

/**
 * @brief Initialise le plateau de jeu avec des bordures, des cases vides, et des pavés.
 * 
 * @param plateau Plateau de jeu à initialiser.
 * @param lesX plateau des coordonnées X du serpent.
 * @param lesY plateau des coordonnées Y du serpent.
 */
void initPlateau(int lesX[], int lesY[]) {
    for (int lig = 0; lig <= LARGEURMAX; lig++) {
        for (int col = 0; col <= LONGUEURMAX; col++) {
            if (((lig == COORDMIN) || (lig == LARGEURMAX)) || ((col == COORDMIN) || (col == LONGUEURMAX))) {
                plateau[lig][col] = CARBORDURE;
            } else {
                plateau[lig][col] = VIDE;
            }
        }
    }

    for (int i = 0; i < NBREPAVES; i++) {
        initPaves(lesX, lesY);
    }
}

/**
 * @brief Place un pavé aléatoire sur le plateau tout en évitant la position du serpent.
 * 
 * @param plateau Plateau de jeu à modifier.
 * @param lesX plateau des coordonnées X du serpent.
 * @param lesY plateau des coordonnées Y du serpent.
 */
void initPaves(int lesX[], int lesY[]) {
    int x, y;
    bool chevauchement;

    do {
        chevauchement = false;

        // Générer une position aléatoire pour le pavé, en évitant les bords
        x = rand() % (LARGEURMAX - TAILLEPAVE - 1) + 1;  // Placer à au moins 3 cases des bords
        y = rand() % (LONGUEURMAX - TAILLEPAVE - 1) + 1; // Placer à au moins 3 cases des bords

        // Vérifier que le pavé ne chevauche pas le corps du serpent
        for (int i = 0; i < tailleSerpent; i++) {
            if ((lesX[i] >= x && lesX[i] < x + TAILLEPAVE) &&
                (lesY[i] >= y && lesY[i] < y + TAILLEPAVE)) {
                chevauchement = true;
                break;
            }
        }
    } while (chevauchement == true);

    // Placer le pavé sur le plateau
    for (int i = y; i < TAILLEPAVE + y; i++) {
        for (int j = x; j < TAILLEPAVE + x; j++) {
            plateau[j][i] = CARBORDURE;
        }
    }
}

/**
 * @brief Affiche le plateau de jeu.
 * 
 * @param plateau Plateau de jeu à afficher.
 */
void affichagePlateau(plateau_de_jeu plateau) {
    for (int lig = 1; lig <= LARGEURMAX; lig++)
    {
        for (int col = 1; col <= LONGUEURMAX; col++)
        {
            afficher(lig, col, plateau[lig][col]);
        }
    }
}

/**
 * @brief Dessine le serpent sur le plateau.
 * 
 * @param lesX plateau des coordonnées X du serpent.
 * @param lesY plateau des coordonnées Y du serpent.
 */
void dessinerSerpent(int lesX[], int lesY[]) {
    afficher(lesX[0], lesY[0], TETE);
    for (int i = 1; i < tailleSerpent; i++)
    {
        afficher(lesX[i], lesY[i], CORPS);
    }
    fflush(stdout);
}

/**
 * @brief Fait avancer le serpent dans une direction donnée et vérifie les collisions.
 * 
 * @param lesX plateau des coordonnées X du serpent.
 * @param lesY plateau des coordonnées Y du serpent.
 * @param direction Direction de déplacement.
 * @param colision Pointeur vers un booléen indiquant si une collision a été détectée.
 */
void progresser(int lesX[], int lesY[], char direction, bool *colision, bool *mangerPomme) {
    
    effacer(lesX[tailleSerpent - 1], lesY[tailleSerpent - 1]); 

    for (int i = tailleSerpent - 1; i > 0; i--) {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }

    if (direction == DROITE) {
        lesX[0]++;
    } else if (direction == GAUCHE) {
        lesX[0]--;
    } else if (direction == HAUT) {
        lesY[0]--;
    } else if (direction == BAS) {
        lesY[0]++;
    }

    // Vérification des collisions avec les bordures
    if (plateau[lesX[0]][lesY[0]] == CARBORDURE) {
        *colision = true;
    }

    // Vérification des collisions avec le corps du serpent
    for (int i = 1; i < (tailleSerpent+compteurPomme); i++) {
        if ((lesX[0] == lesX[i]) && (lesY[0] == lesY[i])) {
            *colision = true;
        }
    }

    do {
        if (plateau[lesX[0]][lesY[0]] == POMME) {
            *mangerPomme = true; // "le serpent à mangé la pomme"
            plateau[lesX[0]][lesY[0]] = VIDE; // Enlever la pomme
            ajouterPomme(lesX, lesY); // Ajouter une nouvelle pomme
        }

        if (*mangerPomme == true){
            compteurPomme++;  // Incrémentation du compteur de pommes
            tailleSerpent++; // la queue du serpent augmente de 1 
            *mangerPomme = false; // le booléen revient à false tant que le serpent n'aura pas mangé la pomme suivante
            temporisation = temporisation - 1000; // Réduire la temporisation pour augmenter la vitesse
        }
    } while(compteurPomme < COMPTEURFINJEU);
    

    dessinerSerpent(lesX, lesY);
}

void ajouterPomme(int lesX[], int lesY[]){
    int x,y;
    bool emplacementValide;

    do {
        x = rand() % (LARGEURMAX - 1) + 1;
        y = rand() % (LONGUEURMAX - 1) + 1;

        emplacementValide = (plateau[x][y] == VIDE);  // Condition correcte
    } while (!emplacementValide);

    plateau[x][y] = POMME;
    afficher(x, y, POMME);
}

/**
* Les procédures/fonction qui suivent sont des "boites noires" données dans l'énoncé de chaque version en nécéssitant l'usage,
* il n'y a donc pas de commentaires car il n'est pas nécéssaire de comprendre ce qu'elles font.
*/
void gotoXY(int x, int y)
{
    printf("\033[%d;%df", y, x);
}

int kbhit()
{
    int unCaractere = 0;
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        unCaractere = 1;
    }
    return unCaractere;
}

void disableEcho()
{
    struct termios tty;

    if (tcgetattr(STDIN_FILENO, &tty) == -1)
    {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    tty.c_lflag &= ~ECHO;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1)
    {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

void enableEcho()
{
    struct termios tty;

    if (tcgetattr(STDIN_FILENO, &tty) == -1)
    {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    tty.c_lflag |= ECHO;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1)
    {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
} 