/**
* @file version2.c
* @brief code de la version 2 du projet de SAE1.01, un jeu snake
* @author Arthur CHAUVEL
* @version 2.2.7
* @date 07/11/24

* deuxième version du programme permettant de déplacer le serpent dans la direction que l'on souhaite,
*
* une collision entre la tete du serpent et sa queue ne stope pas le programme,
*
* si on n'appuye sur aucune touche alors le serpent se déplace indéfiniment dans la direction donnée, initialement vers la droite.
*/

/*
* importation des bibliothèques nécessaires pour le fonctionnement du code
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

/*
* définition de la taille du serpent à 10
*/
#define TAILLE_SERPENT 10

/*
* déclaration des variables nécessaires pour le bon fonctionnement du code
*/
const char FIN_JEU = 'a';
const char CORPS = 'X';
const char TETE = 'O';
const char HAUT = 'z';
const char DROITE = 'd';
const char BAS = 's';
const char GAUCHE = 'q';
const int COORD_MIN = 1;
const int COORD_MAX = 40;
const int COORD_DEPARTX = 20;
const int COORD_DEPARTY = 20;
const int TEMPORISATION = 200000;

/*
* initialisation des procédures/fonctions 
*/
void gotoXY(int x, int y);
int kbhit();
void disableEcho();
void enableEcho();
void afficher(int x, int y, char c);
void effacer(int x, int y);
void dessinerSerpent(int lesX[], int lesY[]);
void progresser(int lesX[], int lesY[], char direction);

/*
* programme principal qui appelle toutes les fonctions du code 
* initialisation des variables et des boucles nécéssaires
*/
int main()
{
    int lesX[TAILLE_SERPENT], lesY[TAILLE_SERPENT];
    int verifArret;
    char direction;
    verifArret = 1;
    direction = DROITE;
    for (int i = 0; i < TAILLE_SERPENT; i++)
    {
        lesX[i] = COORD_DEPARTX - i;
        lesY[i] = COORD_DEPARTY;
    }
    system("clear");
    disableEcho();
    dessinerSerpent(lesX, lesY);
    while (verifArret == 1)
    {
        if (kbhit() == 1)
        {
            char cara = getchar();
            if (cara == FIN_JEU)
            {
                verifArret = 0;
                system("clear");
            }
            else if ((cara == HAUT) && (direction != BAS))
            {
                direction = HAUT;
            }
            else if ((cara == DROITE) && (direction != GAUCHE))
            {
                direction = DROITE;
            }
            else if ((cara == BAS) && (direction != HAUT))
            {
                direction = BAS;
            }
            else if ((cara == GAUCHE) && (direction != DROITE))
            {
                direction = GAUCHE;
            }
        }
        effacer(lesX[TAILLE_SERPENT - 1], lesY[TAILLE_SERPENT - 1]);
        progresser(lesX, lesY, direction);
        usleep(TEMPORISATION);
    }
    enableEcho();
    return EXIT_SUCCESS;
}

/*
* fonction qui permet de déplacer le curseur auc coordonnées voulues
*/
void gotoXY(int x, int y)
{
    printf("\033[%d;%df", y, x);
}

/*
 * Cette fonction vérifie si une touche a été pressée sans arrêter le programme.
 */
int kbhit()
{
    // la fonction retourne :
    // 1 si un caractere est present
    // 0 si pas de caractere present

    int unCaractere = 0;
    struct termios oldt, newt;
    int ch;
    int oldf;

    // mettre le terminal en mode non bloquant
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    // restaurer le mode du terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if ((ch != EOF))
    {
        ungetc(ch, stdin);
        unCaractere = 1;
    }
    return unCaractere;
}

/*
* fonction qui désactive l'affichage au terminal pour
* éviter d'afficher les changements de direction du snake pendant la partie
*/
void disableEcho() {
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Desactiver le flag ECHO
    tty.c_lflag &= ~ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

/*
* fonction qui réactive l'affichage au terminal
*/
void enableEcho() {
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Reactiver le flag ECHO
    tty.c_lflag |= ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

/**
* @brief procédure qui va aux coordonées X et Y, et qui affiche le caractere c entré en parametre
* @param x de type int, définie la coordonnée de x
* @param y de type int, définie la coordonnée de y
* @param c de type char, définie le caractere a afficher
*/
void afficher(int x, int y, char c)
{
    if ((x>=COORD_MIN) && (y>=COORD_MIN))
    {
        gotoXY(x, y);
        printf("%c", c);
    }
}

/** 
* @brief procédure qui efface la queue du serpent pour qu'il reste toujours à une taille de 10 
* @param x de type int, entrée de la coordonée de x 
* @param y de type int, entrée de la coordonée de y 
*/
void effacer(int x, int y)
{
    gotoXY(x, y);
    printf("%c", ' ');
}

/** 
* @brief procédure qui dessine le serpent aux coordonnées qui évoluent quand le seprent avance 
* @param lesX[] de type tableau qui contient les coordonées X du serpent
* @param lesY[] de type tableau qui contient les coordonées Y du serpent
*/
void dessinerSerpent(int lesX[], int lesY[])
{
    for (int i = 0; i < TAILLE_SERPENT; i++)
    {
        if (i == 0) // Dessiner la tete
        {
            afficher(lesX[i], lesY[i], TETE);
        }
        else if (lesX[i] > 0) // Dessiner le corps
        {
            afficher(lesX[i], lesY[i], CORPS);
        }
    }
}

/*
 * Fonction progresser()
 * Cette fonction met à jour la position du serpent en fonction de la direction donnée,
 * puis dessine le serpent à sa nouvelle position.
 *
 * lesX[] : Tableau contenant les positions X de chaque segment du serpent.
 * lesY[] : Tableau contenant les positions Y de chaque segment du serpent.
 * direction : Caractère indiquant la direction de déplacement (HAUT, DROITE, BAS, GAUCHE).
 */
void progresser(int lesX[], int lesY[], char direction)
{
    // Calcule la position des prochains caracteres
    for (int i = TAILLE_SERPENT - 1; i > 0; i--)
    {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }

    if (direction == HAUT)
    {
        lesY[0]--;
    }
    if (direction == DROITE)
    {
        lesX[0]++;
    }
    if (direction == BAS)
    {
        lesY[0]++;
    }
    if (direction == GAUCHE)
    {
        lesX[0]--;
    }

    // Dessine le serpent
    dessinerSerpent(lesX, lesY);
}