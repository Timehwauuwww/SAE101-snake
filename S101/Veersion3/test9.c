#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>

#define LARGEURMAX 80
#define HAUTEURMAX 40

const int TAILLESERPENT = 10;
const int NBREPAVE = 4;
const int TAILLEPAVE = 5;
const int TEMPORISATION = 200000;
const int NBREPOMMESFINJEU = 10;
const int STARTSAFEZONEX = 15;
const int ENDSAFEZONEX = 48;
const int STARTSAFEZONEY = 5;
const int ENDSAFEZONEY = 7;
const int MAXTAILLESERPENT = 100;
const char TETE = 'O';
const char CORPS = 'X';
const char POMME = '6';
const char ARRET = 'a';
const char DROITE = 'd';
const char GAUCHE = 'q';
const char HAUT = 'z';
const char BAS = 's';


/* Déclaration des fonctions existantes */
void gotoXY(int x, int y);
void disableEcho();
void enableEcho();
int kbhit();
void afficher(int x, int y, char c);
void dessinerSerpent(int lesX[], int lesY[], int taille);
void progresser(int lesX[], int lesY[], char direction, bool *collision, bool *pommeMangee);
void gotoXY(int x, int y);
void initPlateau();
void dessinerPlateau(int lesX[], int lesY[]);
void placerPaves();
void ajouterPomme();

/* Plateau de jeu */
char plateau[HAUTEURMAX][LARGEURMAX];
int tailleSerpent = TAILLESERPENT;

/* Position de la pomme */
int posX_pomme = -1, posY_pomme = -1;

int main() {
    int lesX[MAXTAILLESERPENT] = {40, 39, 38, 37, 36, 35, 34, 33, 32, 31};
    int lesY[MAXTAILLESERPENT] = {20, 20, 20, 20, 20, 20, 20, 20, 20, 20};
    char direction = DROITE;
    bool collision = false;
    bool pommeMangee = false;
    bool forfait = false;
    int pommesMangees = 0;

    initPlateau();
    placerPaves();
    ajouterPomme();
    dessinerPlateau(lesX, lesY);

    disableEcho();
    while (!collision && pommesMangees < NBREPOMMESFINJEU) 
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
            if (touche == ARRET){
                forfait = true;
                break;
            }
        }
        progresser(lesX, lesY, direction, &collision, &pommeMangee);
        if (pommeMangee) 
        {
            pommesMangees++;
            ajouterPomme();
        }
        dessinerPlateau(lesX, lesY);
        usleep(TEMPORISATION - pommesMangees * 10000); // Augmenter la vitesse progressivement
    }
    enableEcho();

    if (collision) 
    {
        system("clear");
        printf("Collision détectée. Vous avez perdu.\n");
    }
    else if (pommesMangees >= NBREPOMMESFINJEU)
    {
        system("clear");
        printf("Vous avez gagné. Félicitations !\n");
    }
    else if (forfait)
    {
        system("clear");
        printf("Vous avez déclaré forfait. Dommage !\n");
    }
    return EXIT_SUCCESS;
}

/* Initialise le plateau avec les issues */
void initPlateau() {
    for (int i = 0; i < HAUTEURMAX; i++) 
    {
        for (int j = 0; j < LARGEURMAX; j++) 
        {
            if (i == 0 || i == HAUTEURMAX - 1) 
            {
                plateau[i][j] = (j == LARGEURMAX / 2) ? ' ' : '#';
            } 
            else if (j == 0 || j == LARGEURMAX - 1) 
            {
                plateau[i][j] = (i == HAUTEURMAX / 2) ? ' ' : '#';
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
        posX_pomme = rand() % (LARGEURMAX - 2) + 1;
        posY_pomme = rand() % (HAUTEURMAX - 2) + 1;
    }
    plateau[posY_pomme][posX_pomme] = POMME;
}

void progresser(int lesX[], int lesY[], char direction, bool *collision, bool *pommeMangee) {
    int X = lesX[tailleSerpent - 1];
    int Y = lesY[tailleSerpent - 1];
    plateau[Y][X] = ' ';

    for (int i = tailleSerpent - 1; i > 0; i--) 
    {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }
    
    if (direction == DROITE) lesX[0] = lesX[0] + 1;
    if (direction == GAUCHE) lesX[0] = lesX[0] - 1;
    if (direction == HAUT) lesY[0] = lesY[0] - 1;
    if (direction == BAS) lesY[0] = lesY[0] + 1;

    // Gestion des issues
    if (lesX[0] == 0 && lesY[0] == HAUTEURMAX / 2) lesX[0] = LARGEURMAX - 2; // Issue gauche
    else if (lesX[0] == LARGEURMAX - 1 && lesY[0] == HAUTEURMAX / 2) lesX[0] = 1; // Issue droite
    else if (lesY[0] == 0 && lesX[0] == LARGEURMAX / 2) lesY[0] = HAUTEURMAX - 2; // Issue haut
    else if (lesY[0] == HAUTEURMAX - 1 && lesX[0] == LARGEURMAX / 2) lesY[0] = 1; // Issue bas

    // Vérification des collisions avec les bordures non issues
    if ((lesX[0] == 0 && lesY[0] != HAUTEURMAX / 2) || 
        (lesX[0] == LARGEURMAX - 1 && lesY[0] != HAUTEURMAX / 2) || 
        (lesY[0] == 0 && lesX[0] != LARGEURMAX / 2) || 
        (lesY[0] == HAUTEURMAX - 1 && lesX[0] != LARGEURMAX / 2)) 
    {
        *collision = true;
        return;
    }

    // Vérification des collisions avec le corps ou les obstacles
    *collision = plateau[lesY[0]][lesX[0]] == '#' || plateau[lesY[0]][lesX[0]] == CORPS;

    // Vérification de la pomme
    if (lesX[0] == posX_pomme && lesY[0] == posY_pomme) {
        *pommeMangee = true;
        tailleSerpent++;
    } else {
        *pommeMangee = false;
    }
}


/* Dessine le plateau avec le serpent */
void dessinerPlateau(int lesX[], int lesY[]) {
    for (int i = 0; i < tailleSerpent; i++) {
        plateau[lesY[i]][lesX[i]] = (i == 0) ? TETE : CORPS;
    }
    system("clear");
    for (int i = 0; i < HAUTEURMAX; i++) {
        for (int j = 0; j < LARGEURMAX; j++) {
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

    for (int k = 0; k < NBREPAVE; k++) 
    {
        int x = rand() % (LARGEURMAX - 10) + 2;
        int y = rand() % (HAUTEURMAX - 10) + 2;
        while (y > STARTSAFEZONEY 
            && y + ENDSAFEZONEY 
            && x > STARTSAFEZONEX 
            && x < ENDSAFEZONEX)
        {
            x = rand() % (LARGEURMAX - 10) + 2;
            y = rand() % (HAUTEURMAX - 10) + 2;
        }

        for (int i = 0; i < TAILLEPAVE; i++) 
        {
            for (int j = 0; j < TAILLEPAVE; j++) 
            {
                plateau[y + i][x + j] = '#';
            }
        }
    }
}