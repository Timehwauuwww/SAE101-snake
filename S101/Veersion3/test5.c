#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define LARGEUR 80       
/** Largeur maximale du plateau. */
#define HAUTEUR 40      
/** Longueur maximale du plateau. */
const int COORDMIN = 1;          /** Coordonnée minimale sur le plateau. */
const int TAILLEPAVE = 5;        /** Taille des pavés (carrés). */
const int NBREPAVES = 4;         /** Nombre de pavés à placer sur le plateau. */
const int COORDXDEPART = 40;    /** Position de départ en X du serpent. */
const int COORDYDEPART = 20;    /** Position de départ en Y du serpent. */
const int TEMPORISATION = 200000; /** Temporisation entre les déplacements en microsecondes. */
const int COMPTEURFINJEU = 10;    /** Nombre de pommes nécessaires pour gagner. */
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
const char SERPENT 'O';

typedef struct {
    int x, y;
} Position;

typedef struct {
    Position segments[200];
    int longueur;
    char direction;
} Serpent;

typedef char Plateau[LARGEUR][HAUTEUR];
Plateau plateau;
Serpent serpent;
Position pomme;
int pommes_mangees = 0;

// Prototypes
void afficher(int x, int y, char c);
void effacer(int x, int y);
void initPlateau(int lesX[], int lesY[]);
void initPaves(int lesX[], int lesY[])
void afficherPlateau();
void ajouterPomme();
bool progresser(bool *pommeMagee);
void allongerSerpent();
void gererIssues(Position *pos);
void gotoXY(int x, int y);
int kbhit(void);
void disableEcho();
void enableEcho();


int main() {
    srand(time(NULL));
    int lesX[serpent.longueur], lesY[serpent.longueur];
    char touche = DROITE;
    char direction = DROITE;
    bool pommeMagee = false;
    initPlateau();
    system("clear");
    disableEcho();
    ajouterPomme();
    afficherPlateau();

    while (pommes_mangees < 10) {
        // Déplacement et gestion des pommes
        if (!progresser(&pommeMagee)) {
            printf("Le serpent s'est mordu ou est sorti du cadre !\n");
            break;
        }

        // Si une pomme est mangée
        if (pommeMagee) {
            allongerSerpent();
            ajouterPomme();
        }

        afficherPlateau();
        // Pause pour simuler la vitesse (ajoutez un délai selon votre environnement)
    }

    if (pommes_mangees >= 10)
        printf("Félicitations, vous avez gagné !\n");

    return 0;
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

void initPlateau(int lexX[], int lesY[]) {
    // Initialise le plateau avec un cadre
    for (int i = 0; i < HAUTEUR; i++) {
        for (int j = 0; j < LARGEUR; j++) {
            if (i == 0 || i == HAUTEUR - 1 || j == 0 || j == LARGEUR - 1) {
                plateau[i][j] = CADRE;
            } else {
                plateau[i][j] = VIDE;
            }
        }
    }

    // Ajout des "issues" au centre des côtés
    plateau[HAUTEUR / 2][0] = VIDE;
    plateau[HAUTEUR / 2][LARGEUR - 1] = VIDE;
    plateau[0][LARGEUR / 2] = VIDE;
    plateau[HAUTEUR - 1][LARGEUR / 2] = VIDE;

    // Initialisation du serpent
    serpent.longueur = 3;
    serpent.segments[0] = (Position){LARGEUR / 2, HAUTEUR / 2};
    serpent.segments[1] = (Position){LARGEUR / 2 - 1, HAUTEUR / 2};
    serpent.segments[2] = (Position){LARGEUR / 2 - 2, HAUTEUR / 2};
    serpent.direction = 'D';

    // Place le serpent sur le plateau
    for (int i = 0; i < serpent.longueur; i++) {
        plateau[serpent.segments[i].y][serpent.segments[i].x] = SERPENT;
    }

    // Place les pavé sur le plateau
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
        x = rand() % (LARGEUR - TAILLEPAVE - 6) + 3;
        y = rand() % (HAUTEUR - TAILLEPAVE - 6) + 3;

        for (int i = 0; i < serpent.longueur ; i++) {
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

void afficherPlateau() {
    system("clear"); 
    for (int i = 0; i < HAUTEUR; i++) {
        for (int j = 0; j < LARGEUR; j++) {
            putchar(plateau[i][j]);
        }
        putchar('\n');
    }
    printf("Pommes mangées : %d\n", pommes_mangees);
}

void ajouterPomme() {
    do {
        pomme.x = rand() % (LARGEUR - 2) + 1;
        pomme.y = rand() % (HAUTEUR - 2) + 1;
    } while (plateau[pomme.y][pomme.x] != VIDE);
    plateau[pomme.y][pomme.x] = POMME;
}

bool progresser(bool *pommeMagee) {
    Position nouvellePosition = serpent.segments[0];

    // Calcul de la nouvelle position en fonction de la direction
    if (serpent.direction == 'H') nouvellePosition.y--;
    if (serpent.direction == 'B') nouvellePosition.y++;
    if (serpent.direction == 'G') nouvellePosition.x--;
    if (serpent.direction == 'D') nouvellePosition.x++;

    // Gérer les issues
    gererIssues(&nouvellePosition);

    // Vérifie la collision avec le serpent lui-même
    for (int i = 0; i < serpent.longueur; i++) {
        if (nouvellePosition.x == serpent.segments[i].x &&
            nouvellePosition.y == serpent.segments[i].y) {
            return false;
        }
    }

    // Vérifie si une pomme est mangée
    *pommeMagee = (plateau[nouvellePosition.y][nouvellePosition.x] == POMME);

    // Déplace le serpent
    for (int i = serpent.longueur - 1; i > 0; i--) {
        serpent.segments[i] = serpent.segments[i - 1];
    }
    serpent.segments[0] = nouvellePosition;

    // Met à jour le plateau
    for (int i = 1; i < serpent.longueur; i++) {
        plateau[serpent.segments[i].y][serpent.segments[i].x] = SERPENT;
    }
    plateau[serpent.segments[serpent.longueur - 1].y][serpent.segments[serpent.longueur - 1].x] = VIDE;
    plateau[nouvellePosition.y][nouvellePosition.x] = SERPENT;

    if (*pommeMagee) {
        pommes_mangees++;
    }

    return true;
}

void allongerSerpent() {
    // Allonge le serpent en ajoutant un segment à sa fin
    serpent.longueur++;
}

void gererIssues(Position *pos) {
    if (pos->x == 0) pos->x = LARGEUR - 2;
    if (pos->x == LARGEUR - 1) pos->x = 1;
    if (pos->y == 0) pos->y = HAUTEUR - 2;
    if (pos->y == HAUTEUR - 1) pos->y = 1;
}
