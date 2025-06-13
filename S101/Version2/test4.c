/**
 * @file version4.c
 * @brief Implémentation de la version 4 du jeu Snake pour le projet SAE1.01.
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
#define LONGUEURMAX 40
const int COORDMIN = 1;
const int TAILLEPAVE = 5;
const int NBREPAVES = 4;
const int COORDXDEPART = 40;
const int COORDYDEPART = 20;
const int TAILLESERPENT = 10;
const int TEMPORISATION = 200000;
const int COMPTEURFINJEU = 10;
const int RETRAITTEMPORISATION = 10000;
const char POMME = '6';
const char CARBORDURE = '#';
const char VIDE = ' ';
const char TETE = 'O';
const char CORPS = 'X';
const char HAUT = 'z';
const char BAS = 's';
const char GAUCHE = 'q';
const char DROITE = 'd';
const char FINJEU = 'a';

typedef char plateau_de_jeu[LARGEURMAX + 1][LONGUEURMAX + 1];

plateau_de_jeu plateau;
int compteurPomme = 0;
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
 * @brief Programme principal.
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

        if (touche == HAUT && direction != BAS) direction = HAUT;
        else if (touche == BAS && direction != HAUT) direction = BAS;
        else if (touche == GAUCHE && direction != DROITE) direction = GAUCHE;
        else if (touche == DROITE && direction != GAUCHE) direction = DROITE;

        progresser(lesX, lesY, direction, &collision, &mangerPomme);

        if (mangerPomme) {
            compteurPomme++;
            tailleSerpent++;
            temporisation -= RETRAITTEMPORISATION;
            mangerPomme = false;
        }

        usleep(temporisation);

    } while (touche != FINJEU && !collision && compteurPomme < COMPTEURFINJEU);

    enableEcho();
    system("clear");

    if (compteurPomme >= COMPTEURFINJEU) {
        printf("Bravo, vous avez gagné en mangeant 10 pommes !\n");
    } else if (collision) {
        printf("Vous avez perdu !\n");
    } else if (touche == FINJEU) {
        printf("Vous avez déclaré forfait. Dommage !\n");
    }

    return EXIT_SUCCESS;
}

void afficher(int x, int y, char c) {
    gotoXY(x, y);
    printf("%c", c);
}

void effacer(int x, int y) {
    afficher(x, y, VIDE);
}

/**
 * @brief Initialise le plateau avec des issues.
 */
void initPlateau(int lesX[], int lesY[]) {
    for (int lig = 0; lig <= LARGEURMAX; lig++) {
        for (int col = 0; col <= LONGUEURMAX; col++) {
            if ((lig == COORDMIN || lig == LARGEURMAX) && col != LONGUEURMAX / 2) {
                plateau[lig][col] = CARBORDURE;
            } else if ((col == COORDMIN || col == LONGUEURMAX) && lig != LARGEURMAX / 2) {
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
 * @brief Ajoute une pomme à une position aléatoire.
 */
void ajouterPomme(int lesX[], int lesY[]) {
    int x, y;
    bool emplacementValide;
    do {
        x = rand() % (LARGEURMAX - 1) + 1;
        y = rand() % (LONGUEURMAX - 1) + 1;
        emplacementValide = plateau[x][y] == VIDE;
    } while (!emplacementValide);
    plateau[x][y] = POMME;
    afficher(x, y, POMME);
}

/**
 * @brief Déplace le serpent avec gestion des issues.
 */
void progresser(int lesX[], int lesY[], char direction, bool *collision, bool *mangerPomme) {
    effacer(lesX[tailleSerpent - 1], lesY[tailleSerpent - 1]);

    for (int i = tailleSerpent - 1; i > 0; i--) {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }

    if (direction == DROITE) lesX[0]++;
    else if (direction == GAUCHE) lesX[0]--;
    else if (direction == HAUT) lesY[0]--;
    else if (direction == BAS) lesY[0]++;

    if (lesX[0] > LARGEURMAX) lesX[0] = COORDMIN;
    else if (lesX[0] < COORDMIN) lesX[0] = LARGEURMAX;
    if (lesY[0] > LONGUEURMAX) lesY[0] = COORDMIN;
    else if (lesY[0] < COORDMIN) lesY[0] = LONGUEURMAX;

    if (plateau[lesX[0]][lesY[0]] == CARBORDURE) *collision = true;
    for (int i = 1; i < tailleSerpent; i++) {
        if (lesX[0] == lesX[i] && lesY[0] == lesY[i]) *collision = true;
    }

    if (plateau[lesX[0]][lesY[0]] == POMME) {
        *mangerPomme = true;
        plateau[lesX[0]][lesY[0]] = VIDE;
        ajouterPomme(lesX, lesY);
    }

    dessinerSerpent(lesX, lesY);
}