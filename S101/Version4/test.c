/**
* @file version2.c
* @brief Programme SAE1.01 version2, un seprent 
* @author CHAUVEL Arthur 1E2
* @version Version 2.1
* @date 06/11/2024
*
* deuxième version du programme permettant de déplacer le serpent dans la direction que l'on souhaite,
* une collision entre la tete du serpent et sa queue ne stope pas le programme,
* si on n'appuye sur aucune touche alors le seprent se déplace indéfiniment dans la direction donnée.
*/

#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h> 
#include <termios.h> 
#include <fcntl.h>

/* Définition des constantes */

#define TAILLE_SERPENT 10
#define LARGEUR_ECRAN 40
#define HAUTEUR_ECRAN 40
#define TETE 'O'
#define CORPS 'X'
#define TOUCHE_ARRET 'a'
#define TEMPORISATION 200000

/* Signatures des procédures et fonctions */
void afficher(int x, int y, char c);
void effacer(int x, int y);
void enableEcho();
void disableEcho();
void dessinerSerpent(int lesX[], int lesY[], int taille);
void progresser(int lesX[], int lesY[], int taille, char direction);
void gotoXY(int x, int y);
int kbhit();

/**
 * @brief Programme principal gérant le déroulement du jeu.
 */
int main() {
    int lesX[TAILLE_SERPENT]; /* tableau des X à la taille du serpent */
    int lesY[TAILLE_SERPENT]; /* tableau des Y à la taille du serpent */
    int stop = 0;
    char direction = 'd'; /* Direction initiale */

    /* Initialisation de la position du serpent */
    lesX[0] = 20; /* Tête du serpent */
    lesY[0] = 20;
    for (int i = 1; i < TAILLE_SERPENT; i++) {
        lesX[i] = lesX[i - 1] - 1;
        lesY[i] = lesY[0];
    }

    disableEcho(); /* Empêche l'affichage des touches */

    while (!stop) {
        dessinerSerpent(lesX, lesY, TAILLE_SERPENT);

        /* Lecture de la touche sans bloquer */
        if (kbhit()) {
            char touche = getchar();
            if (touche == 'q' && direction != 'd') direction = 'q';
            if (touche == 'z' && direction != 's') direction = 'z';
            if (touche == 's' && direction != 'z') direction = 's';
            if (touche == 'd' && direction != 'q') direction = 'd';
            if (touche == TOUCHE_ARRET) stop = 1;
        }

        progresser(lesX, lesY, TAILLE_SERPENT, direction);
        usleep(TEMPORISATION); /* Pause pour ralentir le serpent */
        system("clear");
    }

    enableEcho(); /* Réactivation de l'écho */
    return 0;
}

/**
* @brief Permet d'effacer un caractère à des coordonnées precises 
* @param x Entier indiquant le premier parametre de la coordonnée à afficher 
* @param y Entier indiquant le second parametre de la coordonnée à afficher
* @return Affiche le caractère aux coordonnées fournies
*/
void afficher(int x, int y, char c) {
    gotoXY(x, y);
    printf("%c", c);
    fflush(stdout);
}

/* Désactive l'écho du terminal */
void disableEcho() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

/* Réactive l'écho du terminal */
void enableEcho() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

/**
* @brief Permet d'effacer un caractère à des coordonnées precises 
* @param x Entier indiquant le premier parametre de la coordonnée à effacer 
* @param y Entier indiquant le second parametre de la coordonnée à effacer
* @return Efface le caractère aux coordonnées fournies
*/
void effacer(int x, int y) {
    gotoXY(x, y);
    printf(" ");
    fflush(stdout);
}

/**
* @brief Permet d'afficher le serpent à des coordonnées precises 
* @param lesX Tableau des coordonnées X du serpent  
* @param lesY Tableau des coordonnées y du serpent
* @param taille Entier caractérisant la taille du serpent
* @return Affiche le serpent dans sa totalité
*/
void dessinerSerpent(int lesX[], int lesY[], int taille) {
    for (int i = 0; i < taille; i++) 
    {
        if (i == 0) 
        {
            afficher(lesX[i], lesY[i], TETE); 
        } 
        else 
        {
            afficher(lesX[i], lesY[i], CORPS); 
        }
    }
}

/**
 * @brief Fait avancer le serpent dans la direction spécifiée.
 */
void progresser(int lesX[], int lesY[], int taille, char direction) {
    /* Déplacement de chaque segment */
    for (int i = taille - 1; i > 0; i--) {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }

    /* Mise à jour de la tête selon la direction */
    if (direction == 'd') lesX[0]++;
    if (direction == 'q') lesX[0]--;
    if (direction == 'z') lesY[0]--;
    if (direction == 's') lesY[0]++;

    /* Téléportation du serpent si la tête dépasse les limites de l'écran */
    if (lesX[0] < 0) lesX[0] = LARGEUR_ECRAN - 1;
    if (lesX[0] >= LARGEUR_ECRAN) lesX[0] = 0;
    if (lesY[0] < 0) lesY[0] = HAUTEUR_ECRAN - 1;
    if (lesY[0] >= HAUTEUR_ECRAN) lesY[0] = 0;
}

/**
* @brief Permet le déplacement du curseur aux coordonnées fournies 
* @param x Entier indiquant le premier parametre de la coordonnée 
* @param y Entier indiquant le second parametre de la coordonnée
* @return Fait progresser le serpent vers la droite sans l'afficher
*/
void gotoXY(int x, int y){
    printf("\033[%d;%dH", y, x);
}

int kbhit(){
    // la fonction retourne :
    // 1 si un caractere est present
    // 0 si pas de caractere present
    
    int unCaractere=0;
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
 

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        unCaractere=1;
    } 
    return unCaractere;
}