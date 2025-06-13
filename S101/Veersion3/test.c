/**
 * @file version4.c
 * @brief Implémentation de la version 4 du jeu Snake.
 * 
 * Ce fichier contient le code du jeu Snake avec les modifications suivantes :
 * - Les issues apparaissent seulement après que le serpent a mangé 10 pommes.
 * - Le jeu se termine uniquement lorsque le serpent sort par une issue après avoir mangé 10 pommes.
 * - Le serpent s'allonge avec chaque pomme mangée et la vitesse augmente.
 * - Les pavés sont placés aléatoirement et restent sur le plateau.
 * 
 * @author Arthur
 * @version 4
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

const int COORDMIN = 1;          /**< Coordonnée minimale sur le plateau. */
const int LARGEURMAX = 80;       /**< Largeur maximale du plateau. */
const int LONGUEURMAX = 40;      /**< Longueur maximale du plateau. */
const int TAILLEPAVE = 5;        /**< Taille des pavés (carrés). */
const int NBREPAVES = 4;         /**< Nombre de pavés à placer sur le plateau. */
const int COORDXDEPART = 40;    /**< Position de départ en X du serpent. */
const int COORDYDEPART = 20;    /**< Position de départ en Y du serpent. */
const int TAILLESERPENT = 10;    /**< Taille initiale du serpent. */
const int TEMPORISATION = 200000; /**< Temporisation entre les déplacements en microsecondes. */
const int TEMPORISATION_MIN = 50000; /**< Temporisation minimale pour éviter une vitesse excessive. */
const int COMPTEURFINJEU = 10;    /**< Nombre de pommes nécessaires pour déclencher les issues. */
const int COORDCENTREX = 20;    /**< Coordonnée X du centre des côtés haut et bas du plateau. */
const int COORDCENTREY = 40;    /**< Coordonnée Y du centre des côtés gauche et droit du plateau. */
const char POMME = '6';         /**< Caractère représentant une pomme. */
const char CARBORDURE = '#';    /**< Caractère utilisé pour afficher les bordures et pavés. */
const char VIDE = ' ';          /**< Caractère représentant une case vide. */
const char TETE = 'O';          /**< Caractère représentant la tête du serpent. */
const char CORPS = 'X';         /**< Caractère représentant le corps du serpent. */
const char HAUT = 'z';          /**< Touche pour déplacer le serpent vers le haut. */
const char BAS = 's';           /**< Touche pour déplacer le serpent vers le bas. */
const char GAUCHE = 'q';        /**< Touche pour déplacer le serpent à gauche. */
const char DROITE = 'd';        /**< Touche pour déplacer le serpent à droite. */
const char FINJEU = 'a';        /**< Touche pour arrêter le jeu. */

/** @typedef plateau_de_jeu
 * @brief Définition du plateau de jeu comme une matrice de caractères.
 */
typedef char plateau_de_jeu[LARGEURMAX + 1][LONGUEURMAX + 1];

plateau_de_jeu plateau; /**< Plateau de jeu global. */
int compteurPomme = 0; /**< Variable compteur de pommes mangées. */
int tailleSerpent = TAILLESERPENT;
int temporisation = TEMPORISATION;
bool issuesActives = false; /**< Indicateur pour savoir si les issues sont activées. */

/* Prototypes des fonctions */
void afficher(int x, int y, char c);
void effacer(int x, int y);
void initPlateau(int lesX[], int lesY[]);
void activerIssues();
void initPaves(int lesX[], int lesY[]);
void affichagePlateau(plateau_de_jeu plateau);
void dessinerSerpent(int lesX[], int lesY[]);
void progresser(int lesX[], int lesY[], char direction, bool *collision, bool *mangerPomme, bool *sortieIssue);
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
    bool sortieIssue = false;

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

        progresser(lesX, lesY, direction, &collision, &mangerPomme, &sortieIssue);

        // Active les issues lorsque 10 pommes sont mangées
        if (compteurPomme >= COMPTEURFINJEU && !issuesActives) {
            activerIssues();
            issuesActives = true;
        }

        usleep(temporisation);

    } while (touche != FINJEU && !collision && !sortieIssue);

    enableEcho();
    system("clear");

    if (sortieIssue) {
        printf("Bravo, vous avez gagné en sortant par une issue après avoir mangé 10 pommes !\n");
    } else if (collision) {
        printf("Game Over : Vous avez percuté un obstacle !\n");
    }

    return EXIT_SUCCESS;
}

/**
 * @brief Affiche un caractère à une position donnée sur le terminal.
 */
void afficher(int x, int y, char c) {
    gotoXY(x, y);
    printf("%c", c);
}

/**
 * @brief Efface un caractère à une position donnée sur le terminal.
 */
void effacer(int x, int y) {
    afficher(x, y, VIDE);
}

/**
 * @brief Initialise le plateau de jeu avec des bordures, des cases vides, et des pavés.
 * Les issues ne sont pas activées au départ.
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
    for (int i = 0; i < NBREPAVES; i++){
        initPaves(lesX, lesY);
    }
}

/**
 * @brief Active les issues au centre de chaque côté du plateau.
 */
void activerIssues() {
    plateau[COORDMIN][COORDCENTREY] = VIDE;  // Issue en haut
    plateau[LARGEURMAX][COORDCENTREY] = VIDE;  // Issue en bas
    plateau[COORDCENTREX][COORDMIN] = VIDE;  // Issue à gauche
    plateau[COORDCENTREX][LONGUEURMAX] = VIDE;  // Issue à droite
    affichagePlateau(plateau);
}

/**
 * @brief Place un pavé aléatoire sur le plateau en évitant le serpent.
 */
void initPaves(int lesX[], int lesY[]) {
    int x, y;
    bool chevauchement;

    do {
        chevauchement = false;
        x = rand() % (LARGEURMAX - TAILLEPAVE - 1) + 1;
        y = rand() % (LONGUEURMAX - TAILLEPAVE - 1) + 1;

        for (int i = 0; i < tailleSerpent; i++) {
            if (lesX[i] >= x && lesX[i] < x + TAILLEPAVE && lesY[i] >= y && lesY[i] < y + TAILLEPAVE) {
                chevauchement = true;
                break;
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
 * @brief Affiche le plateau de jeu sur le terminal.
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
 * @brief Déplace le serpent, gère les collisions, les pommes, et les issues.
 */
void progresser(int lesX[], int lesY[], char direction, bool *collision, bool *mangerPomme, bool *sortieIssue) {
    effacer(lesX[tailleSerpent - 1], lesY[tailleSerpent - 1]);

    for (int i = tailleSerpent - 1; i > 0; i--) {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }

    if (direction == DROITE) lesX[0]++;
    else if (direction == GAUCHE) lesX[0]--;
    else if (direction == HAUT) lesY[0]--;
    else if (direction == BAS) lesY[0]++;

    // Détection des issues actives
    if (issuesActives) {
        if ((lesX[0] == COORDMIN && lesY[0] == COORDCENTREY) ||
            (lesX[0] == LARGEURMAX && lesY[0] == COORDCENTREY) ||
            (lesY[0] == COORDMIN && lesX[0] == COORDCENTREX) ||
            (lesY[0] == LONGUEURMAX && lesX[0] == COORDCENTREX)) {
            *sortieIssue = true;
            return;
        }
    }

    // Vérification des collisions
    if (plateau[lesX[0]][lesY[0]] == CARBORDURE) *collision = true;
    for (int i = 1; i < tailleSerpent; i++) {
        if (lesX[0] == lesX[i] && lesY[0] == lesY[i]) *collision = true;
    }

    // Gestion des pommes
    if (plateau[lesX[0]][lesY[0]] == POMME) {
        *mangerPomme = true;
        compteurPomme++;
        tailleSerpent++;
        temporisation = (temporisation > TEMPORISATION_MIN) ? temporisation - 10000 : TEMPORISATION_MIN;
        plateau[lesX[0]][lesY[0]] = VIDE;
        ajouterPomme(lesX, lesY);
    }

    dessinerSerpent(lesX, lesY);
}

/**
 * @brief Ajoute une pomme à une position aléatoire.
 */
void ajouterPomme(int lesX[], int lesY[]) {
    int x, y;
    bool emplacementValide;
    int tentatives = 0;

    do {
        x = rand() % (LARGEURMAX - 1) + 1;
        y = rand() % (LONGUEURMAX - 1) + 1;

        emplacementValide = (plateau[x][y] == VIDE);
        tentatives++;
    } while (!emplacementValide && tentatives < 1000);

    if (tentatives < 1000) {
        plateau[x][y] = POMME;
        afficher(x, y, POMME);
    }
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