/**
 * @file version4.c
 * @brief Implémentation de la version 4 du jeu Snake pour le projet SAE1.01.
 * 
 * Ce fichier contient le code de la quatrième version du jeu Snake. 
 * Les fonctionnalités incluent :
 * - Affichage des bordures avec des "issues".
 * - Placement de pavés aléatoires dans la zone de jeu.
 * - Gestion des collisions (bordures, pavés, serpent lui-même).
 * - Déplacement initial du serpent vers la droite jusqu'à appui sur une touche directionnelle.
 * - Système de pommes à manger, allongement du serpent et augmentation de la vitesse.
 * - Passage à travers les issues (réapparition sur le côté opposé).
 * 
 * Le jeu se termine lorsque 10 pommes sont mangées ou si le joueur appuie sur 'a'.
 * 
 * @author Arthur CHAUVEL
 * @version 4.5.3
 * @date 15/11/24
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

#define LARGEURMAX 80       
/** Largeur maximale du plateau. */
#define LONGUEURMAX 40      
/** Longueur maximale du plateau. */
const int COORDMIN = 1;          /** Coordonnée minimale sur le plateau. */
const int TAILLEPAVE = 5;        /** Taille des pavés (carrés). */
const int NBREPAVES = 4;         /** Nombre de pavés à placer sur le plateau. */
const int COORDXDEPART = 40;    /** Position de départ en X du serpent. */
const int COORDYDEPART = 20;    /** Position de départ en Y du serpent. */
const int TAILLESERPENT = 10;    /** Taille initiale du serpent. */
const int TEMPORISATION = 200000; /** Temporisation entre les déplacements en microsecondes. */
const int COMPTEURFINJEU = 10;    /** Nombre de pommes nécessaires pour gagner. */
const int COORDCENTREX = LONGUEURMAX/2;    /** Coordonnée X du centre des côtés haut et bas du plateau. */
const int COORDCENTREY = LARGEURMAX/2;    /** Coordonnée Y du centre des côtés gauche et droit du plateau. */
const int RETRAITTEMPORISATION = 10000; /** Nombre retiré à TEMPORISATION a chaque fois que le serpent mange une pomme */
const char POMME = '6';         /** Caractère représentant une pomme. */
const char CARBORDURE = '#';    /** Caractère utilisé pour afficher les bordures et pavés. */
const char VIDE = ' ';          /** Caractère représentant une case vide. */
const char TETE = 'O';          /** Caractère représentant la tête du serpent. */
const char CORPS = 'X';         /** Caractère représentant le corps du serpent. */
const char HAUT = 'z';          /** Touche pour déplacer le serpent vers le haut. */
const char BAS = 's';           /** Touche pour déplacer le serpent vers le bas. */
const char GAUCHE = 'q';        /** Touche pour déplacer le serpent à gauche. */
const char DROITE = 'd';        /** Touche pour déplacer le serpent à droite. */
const char FINJEU = 'a';        /** Touche pour arrêter le jeu. */

/** @typedef plateau_de_jeu
 * @brief Définition du plateau de jeu comme une matrice de caractères.
 */
typedef char plateau_de_jeu[LARGEURMAX + 1][LONGUEURMAX + 1];

plateau_de_jeu plateau; /** Plateau de jeu global. */
int compteurPomme = 0; /** Variable compteur de pommes mangées. */
int tailleSerpent = TAILLESERPENT;
int temporisation = TEMPORISATION;

/* Prototypes des fonctions */
void afficher(int x, int y, char c);
void effacer(int x, int y);
void initPlateau(int lesX[], int lesY[]);
void initPaves(int lesX[], int lesY[]);
void affichagePlateau(plateau_de_jeu plateau);
void dessinerSerpent(int lesX[], int lesY[]);
void progresser(int lesX[], int lesY[], char direction, bool *collision, bool *mangerPomme);
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
int main() {
    srand(time(NULL));
    int lesX[tailleSerpent], lesY[tailleSerpent];
    char touche = DROITE;
    char direction = DROITE;

    initPlateau(lesX, lesY);
    system("clear");
    disableEcho();

    bool collision = false;
    bool mangerPomme = false;

    for (int i = 0; i < tailleSerpent; i++) {
        lesX[i] = COORDXDEPART - i;
        lesY[i] = COORDYDEPART;
    }
    affichagePlateau(plateau);
    ajouterPomme(lesX, lesY);

do {
    if (kbhit()) {
        touche = getchar();
    }

    if (touche == HAUT && direction != BAS) {
        direction = HAUT;
    }
    else if (touche == BAS && direction != HAUT) {
        direction = BAS;
    }
    else if (touche == GAUCHE && direction != DROITE) {
        direction = GAUCHE;
    }
    else if (touche == DROITE && direction != GAUCHE) {
        direction = DROITE;
    }

    progresser(lesX, lesY, direction, &collision, &mangerPomme);

    // Gestion de la pomme mangée
    if (mangerPomme) {
        compteurPomme++;
        tailleSerpent++;
        temporisation =temporisation - RETRAITTEMPORISATION; // Augmentation de la vitesse
        mangerPomme = false; // Réinitialiser le flag
    }

    usleep(temporisation);

} while (touche != FINJEU && !collision && compteurPomme < COMPTEURFINJEU);

    if (compteurPomme >= COMPTEURFINJEU) {
        enableEcho();
        system("clear");
        printf("Bravo, vous avez gagné en mangeant 10 pommes !\n");
    }
    else if (collision == true){
        enableEcho();
        system("clear");
        printf("Vous avez perdu !\n");
    }
    else if (touche == FINJEU){
        enableEcho();
        system("clear");
        printf("Vous avez déclaré forfait. Dommage !\n");
    }

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
    gotoXY(x, y);
    printf("%c", c);
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
 * @brief Initialise le plateau de jeu avec des bordures, des cases vides, des pavés, et des "issues".
 * 
 * @param lesX Coordonnées X du serpent.
 * @param lesY Coordonnées Y du serpent.
 */
void initPlateau(int lesX[], int lesY[]) {
    for (int lig = 0; lig <= LARGEURMAX; lig++) {
        for (int col = 0; col <= LONGUEURMAX; col++) {
            if (lig == COORDMIN || lig == LARGEURMAX || col == COORDMIN || col == LONGUEURMAX) {
                plateau[lig][col] = CARBORDURE;  // Bordure
            } else {
                plateau[lig][col] = VIDE;  // Case vide
            }
        }
    }
    for (int i = 0; i < NBREPAVES; i++) {
        initPaves(lesX, lesY);
    }
}

/**
 * @brief Place un pavé aléatoire sur le plateau en évitant le serpent.
 * 
 * @param lesX Coordonnées X du serpent.
 * @param lesY Coordonnées Y du serpent.
 */
void initPaves(int lesX[], int lesY[]) {
    int x, y;
    bool chevauchement;

    do {
        chevauchement = false;
        x = rand() % (LARGEURMAX - TAILLEPAVE - 6) + 3;
        y = rand() % (LONGUEURMAX - TAILLEPAVE - 6) + 3;

        for (int i = 0; i < tailleSerpent; i++) {
            if (lesX[i] >= x && lesX[i] < x + TAILLEPAVE && lesY[i] >= y && lesY[i] < y + TAILLEPAVE) {
                chevauchement = true;
            }
        }
    } while (chevauchement);

    for (int i = y; i < TAILLEPAVE + y; i++) {
        for (int j = x; j < TAILLEPAVE + x; j++) {
            plateau[j][i] = CARBORDURE;
        }
    }
}

/**
 * @brief Affiche le plateau.
 */
void affichagePlateau(plateau_de_jeu plateau) {
    for (int lig = 1; lig <= LARGEURMAX; lig++) {
        for (int col = 1; col <= LONGUEURMAX; col++) {
            afficher(lig, col, plateau[lig][col]);
        }
    }
}

/**
 * @brief Dessine le serpent.
 */
void dessinerSerpent(int lesX[], int lesY[]) {
    afficher(lesX[0], lesY[0], TETE);
    for (int i = 1; i < tailleSerpent; i++) {
        afficher(lesX[i], lesY[i], CORPS);
    }
    fflush(stdout);
}

/**
 * @brief Déplace le serpent, gère les collisions et les pommes.
 */
void progresser(int lesX[], int lesY[], char direction, bool *collision, bool *mangerPomme) {
    effacer(lesX[tailleSerpent - 1], lesY[tailleSerpent - 1]);

    for (int i = tailleSerpent - 1; i > 0; i--) {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }

    if (direction == DROITE){
        lesX[0]++;
    }
    else if (direction == GAUCHE){
        lesX[0]--;
    }
    else if (direction == HAUT){
        lesY[0]--;
    }
    else if (direction == BAS){
        lesY[0]++;
    }

    // Vérification des collisions
    if (plateau[lesX[0]][lesY[0]] == CARBORDURE){
        *collision = true;
    }
    for (int i = 1; i < tailleSerpent; i++) {
        if (lesX[0] == lesX[i] && lesY[0] == lesY[i]){
             *collision = true;
        }
    }

    // Gestion des pommes
    if (plateau[lesX[0]][lesY[0]] == POMME) {
        *mangerPomme = true;
        plateau[lesX[0]][lesY[0]] = VIDE;
        ajouterPomme(lesX, lesY);
    }

    dessinerSerpent(lesX, lesY);
}

bool estCaseLibre(int x, int y, int lesX[], int lesY[]) {
    if (plateau[x][y] != VIDE) {
        return false; // Case non vide
    }
    for (int i = 0; i < tailleSerpent; i++) {
        if (lesX[i] == x && lesY[i] == y) {
            return false; // Case occupée par le serpent
        }
    }
    return true;
}

/**
 * @brief Ajoute une pomme à une position aléatoire.
 */
void ajouterPomme(int lesX[], int lesY[]) {
    int x, y;
    bool emplacementValide;

    do {
        x = rand() % (LARGEURMAX - 1) + 1;
        y = rand() % (LONGUEURMAX - 1) + 1;

        emplacementValide = estCaseLibre(x, y, lesX, lesY);
    } while (!emplacementValide);
    plateau[x][y] = POMME;
    afficher(x, y, POMME);
}

/**
 * @brief Place le curseur à une position donnée dans le terminal.
 */
void gotoXY(int x, int y) {
    printf("\033[%d;%df", y, x);
}

/**
 * @brief Vérifie si une touche a été pressée.
 */
int kbhit() {
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

    if (ch != EOF) {
        ungetc(ch, stdin);
        unCaractere = 1;
    }
    return unCaractere;
}

/**
 * @brief Désactive l'affichage des entrées clavier.
 */
void disableEcho() {
    struct termios tty;
    if (tcgetattr(STDIN_FILENO, &tty) == -1) exit(EXIT_FAILURE);
    tty.c_lflag &= ~ECHO;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) exit(EXIT_FAILURE);
}

/**
 * @brief Réactive l'affichage des entrées clavier.
 */
void enableEcho() {
    struct termios tty;
    if (tcgetattr(STDIN_FILENO, &tty) == -1) exit(EXIT_FAILURE);
    tty.c_lflag |= ECHO;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) exit(EXIT_FAILURE);
}