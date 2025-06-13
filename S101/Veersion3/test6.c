/**
 * @file version4.c
 * @brief Implémentation de la version 4 du jeu Snake pour le projet SAE1.01.
 * 
 * Ce fichier intègre les nouvelles fonctionnalités de la version 4 :
 * - Apparition et gestion des pommes.
 * - Allongement du serpent et augmentation de la vitesse en cas de consommation de pomme.
 * - Sorties dans les bordures permettant de réapparaître sur le côté opposé.
 * - Fin de la partie après 10 pommes mangées.
 * 
 * @author Arthur CHAUVEL
 * @version 4.6.5
 * @date 24/11/2024
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
#define LONGUEURMAX 40
#define LARGEURMAX 80

const int COORDMIN = 1;
const int ZONEPROTECTIONSERPENT = 2;
const int TAILLEPAVE = 5;
const int NBREPAVES = 4;
const int COORDXDEPART = 40;
const int COORDYDEPART = 20;
const int TAILLEINIT = 10;  /** Taille initiale du serpent. */
const char CARBORDURE = '#';
const char VIDE = ' ';
const char TETE = 'O';
const char CORPS = 'X';
const char POMME = '6';   /** Caractère représentant une pomme. */
const char HAUT = 'z';
const char BAS = 's';
const char GAUCHE = 'q';
const char DROITE = 'd';
const char FINJEU = 'a';
const int VITESSE_INITIALE = 200000; /** Vitesse initiale du serpent (microsecondes). */
const int COMPTEURPOMMESVICTOIRE = 10;      /** Nombre de pommes nécessaires pour gagner. */
const int RETRAITVITESSE = 10000;

typedef char plateau_de_jeu[LARGEURMAX + 1][LONGUEURMAX + 1];
int pavesX[NBREPAVES];
int pavesY[NBREPAVES];

plateau_de_jeu plateau;

/* Prototypes des fonctions */
void afficher(int x, int y, char c);
void effacer(int x, int y);
void initPlateau(int lesX[], int lesY[]);
void initPaves(plateau_de_jeu plateau);
bool positionUnique(int x, int y, int *tempX, int *tempY, int taille);
void affichagePlateau(plateau_de_jeu plateau);
void dessinerSerpent(int lesX[], int lesY[], int taille);
void ajouterPomme();
void progresser(int lesX[], int lesY[], int *taille, char direction, bool *collision, bool *mangePomme);
void gotoXY(int x, int y);
int kbhit(void);
void disableEcho();
void enableEcho();

int main() {
    srand(time(NULL));
    if (COORDXDEPART < COORDMIN || COORDXDEPART > LARGEURMAX ||
        COORDYDEPART < COORDMIN || COORDYDEPART > LONGUEURMAX) {
        fprintf(stderr, "Erreur : les coordonnées de départ du serpent sont en dehors du plateau.\n");
        return EXIT_FAILURE;
    }
    int lesX[LARGEURMAX * LONGUEURMAX], lesY[LARGEURMAX * LONGUEURMAX];
    int tailleSerpent = TAILLEINIT, pommesMangees = 0, vitesse = VITESSE_INITIALE;
    char touche = DROITE, direction = DROITE;
    bool collision = false, mangePomme = false;

    initPlateau(lesX, lesY);
    system("clear");
    disableEcho();

    // Initialisation des coordonnées du serpent
    for (int i = 0; i < TAILLEINIT; i++) {
        lesX[i] = COORDXDEPART - i;
        lesY[i] = COORDYDEPART;
    }
    affichagePlateau(plateau);
    ajouterPomme();

    do {
        if (kbhit()) {
            touche = getchar();
        }

        if (touche == HAUT && direction != BAS) direction = HAUT;
        else if (touche == BAS && direction != HAUT) direction = BAS;
        else if (touche == GAUCHE && direction != DROITE) direction = GAUCHE;
        else if (touche == DROITE && direction != GAUCHE) direction = DROITE;

        progresser(lesX, lesY, &tailleSerpent, direction, &collision, &mangePomme);

        if (mangePomme) {
            pommesMangees++;
            ajouterPomme();
            vitesse = vitesse - RETRAITVITESSE;
            mangePomme = false;
        }

        usleep(vitesse);
    } while (touche != FINJEU && !collision && pommesMangees != COMPTEURPOMMESVICTOIRE);

    system("clear");
    gotoXY(1, 1);
    enableEcho();
    if (pommesMangees == COMPTEURPOMMESVICTOIRE) {
        printf("Félicitations, vous avez gagné !\n");
    } else {
        printf("Partie terminée.\n");
    }
    return EXIT_SUCCESS;
}

/* Implémentation des nouvelles fonctions et modifications */
void ajouterPomme() {
    int x, y;
    do {
        x = rand() % LARGEURMAX + 1;
        y = rand() % LONGUEURMAX + 1;
    } while (plateau[x][y] != VIDE || x == LARGEURMAX / 2 || y == LONGUEURMAX / 2);

    plateau[x][y] = POMME;
    afficher(x, y, POMME);
}

void initPlateau(int lesX[], int lesY[]) {
    for (int lig = 0; lig <= LARGEURMAX; lig++) {
        for (int col = 0; col <= LONGUEURMAX; col++) {
            if (lig == COORDMIN || lig == LARGEURMAX) {
                plateau[lig][col] = (col == LONGUEURMAX / 2) ? VIDE : CARBORDURE;
            } else if (col == COORDMIN || col == LONGUEURMAX) {
                plateau[lig][col] = (lig == LARGEURMAX / 2) ? VIDE : CARBORDURE;
            } else {
                plateau[lig][col] = VIDE;
            }
        }
    }
    for (int i = 0; i < NBREPAVES; i++) {
        initPaves(plateau);
    }
}

void progresser(int lesX[], int lesY[], int *taille, char direction, bool *collision, bool *mangePomme) {
    effacer(lesX[*taille - 1], lesY[*taille - 1]);

    for (int i = *taille - 1; i > 0; i--) {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }

    if (direction == DROITE) lesX[0]++;
    else if (direction == GAUCHE) lesX[0]--;
    else if (direction == HAUT) lesY[0]--;
    else if (direction == BAS) lesY[0]++;

    // Gestion des sorties via les issues
    if (lesX[0] > LARGEURMAX) lesX[0] = COORDMIN;           // Sortie droite → Réapparaît à gauche
    else if (lesX[0] < COORDMIN) lesX[0] = LARGEURMAX;      // Sortie gauche → Réapparaît à droite

    if (lesY[0] > LONGUEURMAX) lesY[0] = COORDMIN;          // Sortie bas → Réapparaît en haut
    else if (lesY[0] < COORDMIN) lesY[0] = LONGUEURMAX;     // Sortie haut → Réapparaît en bas

    // Vérification des collisions
    if (plateau[lesX[0]][lesY[0]] == CARBORDURE || plateau[lesX[0]][lesY[0]] == CORPS) {
        *collision = true;
    }

    // Vérification si le serpent mange une pomme
    if (plateau[lesX[0]][lesY[0]] == POMME) {
        *mangePomme = true;
        lesX[*taille] = lesX[*taille - 1];
        lesY[*taille] = lesY[*taille - 1];
        (*taille)++;
    }

    dessinerSerpent(lesX, lesY, *taille);
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
 * @brief Place un pavé aléatoire sur le plateau tout en évitant la position du serpent.
 * 
 * @param lesX Plateau des coordonnées X du serpent.
 * @param lesY Plateau des coordonnées Y du serpent.
 */
void initPaves(plateau_de_jeu plateau) {
    srand(time(NULL));

    for (int p = 0; p < NBREPAVES; p++) 
    {
        int x, y;

        do {
            x = rand() % (LARGEURMAX - 2 * TAILLEPAVE -2) + 2;
            y = rand() % (LONGUEURMAX - 2 * TAILLEPAVE -2) + 2;
        } while (plateau[y][x] == CARBORDURE || 
                 ((x >= COORDXDEPART - 10) ||
                  (x <= COORDXDEPART + 10) || 
                  (y >= COORDYDEPART -10) || 
                  (y <= COORDYDEPART + 10)));

        for (int i = 0; i < TAILLEPAVE; i++)
        {
            for (int j = 0; j < TAILLEPAVE; j++) 
            {
                plateau[y + i][x + j] = CARBORDURE;
            }
        }
    }
}

// Fonction pour vérifier si une paire (x, y) existe déjà
bool positionUnique(int x, int y, int *tempX, int *tempY, int taille) {
    bool statut = true;
    for (int i = 0; i < taille; i++) {
        if (tempX[i] == x && tempY[i] == y) {
            statut = false; // La position est déjà utilisée
            break;
        }
    }
    return statut;
}

/**
 * @brief Affiche le plateau de jeu.
 * 
 * @param plateau Plateau de jeu à afficher.
 */
void affichagePlateau(plateau_de_jeu plateau) {
    for (int lig = 1; lig <= LARGEURMAX; lig++) {
        for (int col = 1; col <= LONGUEURMAX; col++) {
            afficher(lig, col, plateau[lig][col]);
        }
    }
}

/**
 * @brief Dessine le serpent sur le plateau.
 * 
 * @param lesX Plateau des coordonnées X du serpent.
 * @param lesY Plateau des coordonnées Y du serpent.
 * @param taille Taille actuelle du serpent.
 */
void dessinerSerpent(int lesX[], int lesY[], int taille) {
    afficher(lesX[0], lesY[0], TETE);
    for (int i = 1; i < taille; i++) {
        afficher(lesX[i], lesY[i], CORPS);
    }
    fflush(stdout);
}

/**
 * @brief Déplace le curseur du terminal à une position donnée.
 * 
 * @param x Coordonnée en X.
 * @param y Coordonnée en Y.
 */
void gotoXY(int x, int y) {
    printf("\033[%d;%df", y, x);
}

/**
 * @brief Vérifie si une touche a été pressée.
 * 
 * @return 1 si une touche a été pressée, 0 sinon.
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
 * @brief Désactive l'écho des entrées au clavier dans le terminal.
 */
void disableEcho() {
    struct termios tty;

    if (tcgetattr(STDIN_FILENO, &tty) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    tty.c_lflag &= ~ECHO;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Réactive l'écho des entrées au clavier dans le terminal.
 */
void enableEcho() {
    struct termios tty;

    if (tcgetattr(STDIN_FILENO, &tty) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    tty.c_lflag |= ECHO;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}