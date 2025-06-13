#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>

#define LARGEUR_ECRAN 80
#define HAUTEUR_ECRAN 40
#define TAILLE_SERPENT 10
#define MAX_TAILLE_SERPENT 100
#define NB_PAVES 4
#define TAILLE_PAVES 5
#define LIMITE_ECRAN 0
#define TETE 'O'
#define CORPS 'X'
#define POMME '6'
#define ARRET 'a'
#define TEMPORISER 200000
#define DROITE 'd'
#define GAUCHE 'q'
#define HAUT 'z'
#define BAS 's'
#define MAX_POMMES 10
#define START_SAFE_ZONE_X 15
#define END_SAFE_ZONE_X 48
#define START_SAFE_ZONE_Y 5
#define END_SAFE_ZONE_Y 7


/* Déclaration des fonctions existantes */
void disableEcho();
void enableEcho();
int kbhit();
void afficher(int x, int y, char c);
void dessinerSerpent(int lesX[], int lesY[], int taille);
void progresser(int lesX[], int lesY[], char direction, bool *collision, bool *pommeMangee, int *tailleSerpent);
void gotoXY(int x, int y);
void initPlateau();
void dessinerPlateau(int lesX[], int lesY[], int tailleSerpent);
void placerPaves();
void ajouterPomme();

/* Plateau de jeu */
char plateau[HAUTEUR_ECRAN][LARGEUR_ECRAN];

/* Position de la pomme */
int posX_pomme = -1, posY_pomme = -1;

int main() {
    int lesX[MAX_TAILLE_SERPENT] = {40, 39, 38, 37, 36, 35, 34, 33, 32, 31};
    int lesY[MAX_TAILLE_SERPENT] = {20, 20, 20, 20, 20, 20, 20, 20, 20, 20};
    int tailleSerpent = TAILLE_SERPENT;
    char direction = DROITE;
    bool collision = false, pommeMangee = false;
    int pommesMangees = 0;

    initPlateau();
    placerPaves();
    ajouterPomme();
    dessinerPlateau(lesX, lesY, tailleSerpent);

    disableEcho();
    while (!collision && pommesMangees < MAX_POMMES) 
    {
        if (kbhit()) 
        {
            char touche = getchar();
            if ((touche == DROITE && direction != GAUCHE) ||
                (touche == GAUCHE && direction != DROITE) ||
                (touche == HAUT && direction != BAS) ||
                (touche == BAS && direction != HAUT)) 
                {
                direction = touche;
            }
            if (touche == ARRET) break;
        }
        progresser(lesX, lesY, direction, &collision, &pommeMangee, &tailleSerpent);
        if (pommeMangee) 
        {
            pommesMangees++;
            ajouterPomme();
        }
        dessinerPlateau(lesX, lesY, tailleSerpent);
        usleep(TEMPORISER - pommesMangees * 10000); // Augmenter la vitesse progressivement
    }
    enableEcho();

    if (collision) 
    {
        printf("Collision détectée. Fin du jeu.\n");
    }
    else if (pommesMangees >= MAX_POMMES)
    {
        printf("Vous avez gagné ! Félicitations !\n");
    }
    else 
    {
        printf("Jeu arrêté par l'utilisateur.\n");
    }
    return 0;
}

/* Initialise le plateau avec les issues */
void initPlateau() {
    for (int i = 0; i < HAUTEUR_ECRAN; i++) 
    {
        for (int j = 0; j < LARGEUR_ECRAN; j++) 
        {
            if (i == 0 || i == HAUTEUR_ECRAN - 1) 
            {
                plateau[i][j] = (j == LARGEUR_ECRAN / 2) ? ' ' : '#';
            } 
            else if (j == 0 || j == LARGEUR_ECRAN - 1) 
            {
                plateau[i][j] = (i == HAUTEUR_ECRAN / 2) ? ' ' : '#';
            } 
            else 
            {
                plateau[i][j] = ' ';
            }
        }
    }
}

/* Place une pomme aléatoirement sur une case vide */
void ajouterPomme() {
    srand(time(NULL));
    while (plateau[posY_pomme][posX_pomme] != ' ')
    {
        posX_pomme = rand() % (LARGEUR_ECRAN - 2) + 1;
        posY_pomme = rand() % (HAUTEUR_ECRAN - 2) + 1;
    }
    plateau[posY_pomme][posX_pomme] = POMME;
}

/* Fait progresser le serpent */
void progresser(int lesX[], int lesY[], char direction, bool *collision, bool *pommeMangee, int *tailleSerpent) {
    int ancienneQueueX = lesX[*tailleSerpent - 1];
    int ancienneQueueY = lesY[*tailleSerpent - 1];
    plateau[ancienneQueueY][ancienneQueueX] = ' ';

    for (int i = *tailleSerpent - 1; i > 0; i--) 
    {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }
    
    if (direction == DROITE) lesX[0] = (lesX[0] + 1);
    if (direction == GAUCHE) lesX[0] = (lesX[0] - 1);
    if (direction == HAUT) lesY[0] = (lesY[0] - 1);
    if (direction == BAS) lesY[0] = (lesY[0] + 1);

    // Gestion des issues
    if (lesX[0] == 0) lesX[0] = LARGEUR_ECRAN - 2;
    if (lesX[0] == LARGEUR_ECRAN - 1) lesX[0] = 1;
    if (lesY[0] == 0) lesY[0] = HAUTEUR_ECRAN - 2;
    if (lesY[0] == HAUTEUR_ECRAN - 1) lesY[0] = 1;

    // Vérification des collisions
    *collision = plateau[lesY[0]][lesX[0]] == '#' || plateau[lesY[0]][lesX[0]] == CORPS;

    // Vérification de la pomme
    if (lesX[0] == posX_pomme && lesY[0] == posY_pomme) {
        *pommeMangee = true;
        (*tailleSerpent)++;
    } else {
        *pommeMangee = false;
    }
}


/* Dessine le plateau avec le serpent */
void dessinerPlateau(int lesX[], int lesY[], int tailleSerpent) {
    for (int i = 0; i < tailleSerpent; i++) {
        plateau[lesY[i]][lesX[i]] = (i == 0) ? TETE : CORPS;
    }
    system("clear");
    for (int i = 0; i < HAUTEUR_ECRAN; i++) {
        for (int j = 0; j < LARGEUR_ECRAN; j++) {
            putchar(plateau[i][j]);
        }
        putchar('\n');
    }
}

void disableEcho() {
    struct termios tty;
    if (tcgetattr(STDIN_FILENO, &tty) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
    tty.c_lflag &= ~ECHO; // Désactive le flag ECHO
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

void enableEcho() {
    struct termios tty;
    if (tcgetattr(STDIN_FILENO, &tty) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
    tty.c_lflag |= ECHO; // Active le flag ECHO
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

int kbhit() {
    int unCaractere = 0;
    struct termios oldt, newt;
    int ch;
    int oldf;

    // Obtenir les attributs du terminal
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    // Passer le terminal en mode non-bloquant
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    // Restaurer le mode du terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        unCaractere = 1;
    }

    return unCaractere; // Retourne 1 si un caractère est présent, sinon 0
}

/**
* @brief Permet de placer des pavés aléatoirement et dans le cardre du jeu
* @brief tout en respectant une zone de sécurité autour du serpent 
* @return Remplace les cases vides du plateau de jeu par des # composant le pavé
*/
void placerPaves() {
    srand(time(NULL));

    // Définir une zone sécurisée autour de la position de départ du serpent

    for (int k = 0; k < NB_PAVES; k++) 
    {
        int x = rand() % (LARGEUR_ECRAN - 10) + 2;
        int y = rand() % (HAUTEUR_ECRAN - 10) + 2;
        while (y > START_SAFE_ZONE_Y 
            && y + END_SAFE_ZONE_Y 
            && x > START_SAFE_ZONE_X 
            && x < END_SAFE_ZONE_X)
        {
            x = rand() % (LARGEUR_ECRAN - 10) + 2;
            y = rand() % (HAUTEUR_ECRAN - 10) + 2;
        }

        for (int i = 0; i < TAILLE_PAVES; i++) 
        {
            for (int j = 0; j < TAILLE_PAVES; j++) 
            {
                plateau[y + i][x + j] = '#';
            }
        }
    }
}