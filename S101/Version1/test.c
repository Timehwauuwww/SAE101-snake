#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

// Constantes
const int N = 10;  // Taille du serpent
const char ESPACE = ' ';
const char CORPS = 'X';
const char TETE = 'O';
const char TOUCHE = 'a';

// Prototypes des fonctions
void gotoXY(int x, int y);
void afficher(int x, int y, char c);
void effacer(int x, int y);
void dessinerSerpent(int lesX[], int lesY[]);
void progresser(int lesX[], int lesY[]);
int kbhit();
void replacerCurseurApres(int x, int y);

int main() {
    int x, y;
    int lesX[N], lesY[N];
    char touche = 0;  // Touche par défaut

    // Initialisation de la position du serpent
    do {
        printf("Entrez votre valeur de x comprise entre %d et %d : ", N, 40); // x doit être >= N
        scanf("%d", &x);
        if (x < N || x >= 40) {
            printf("Valeur erronée ! Veuillez entrer une valeur de x comprise entre %d et 40.\n", N);
        }
    } while (x < N || x >= 40);  // x doit être assez grand pour que le serpent soit en entier

    do {
        printf("Entrez votre valeur de y comprise entre 1 et 40 : ");
        scanf("%d", &y);
        if (y <= 0 || y >= 40) {
            printf("Valeur erronée ! Veuillez entrer une valeur de y comprise entre 0 et 40.\n");
        }
    } while (y <= 0 || y >= 40);

    // Position initiale du serpent
    for (int i = 0; i < N; i++) {
        lesX[i] = x - i;  // Le serpent est placé horizontalement, de gauche à droite
        lesY[i] = y;
    }

    // Boucle principale pour déplacer le serpent
    while (1) {
        // Effacer la queue du serpent
        effacer(lesX[N-1], lesY[N-1]);

        // Faire avancer le serpent vers la droite
        progresser(lesX, lesY);

        // Dessiner le serpent
        dessinerSerpent(lesX, lesY);

        // Replacer le curseur après la tête du serpent
        replacerCurseurApres(lesX[0], lesY[0]);

        // Attendre un peu avant de continuer
        usleep(200000);  // 200ms

        // Vérifier si une touche a été pressée
        if (kbhit()) {
            touche = getchar();

            if (touche == TOUCHE) {
                // Si la touche 'a' est pressée, effacer l'écran et terminer le programme
                system("clear");
                break;  // Sortir de la boucle sans redessiner le serpent
            }
        }
    }

    return 0;
}

void gotoXY(int x, int y) {
    printf("\033[%d;%df", y, x);
}

void afficher(int x, int y, char c) {
    gotoXY(x, y);
    printf("%c", c);
}

void effacer(int x, int y) {
    afficher(x, y, ESPACE);
}

void dessinerSerpent(int lesX[], int lesY[]) {
    for (int i = 0; i < N; i++) {
        if (i == 0) {
            afficher(lesX[i], lesY[i], TETE);
        } else {
            afficher(lesX[i], lesY[i], CORPS);
        }
    }
}

// Fonction pour replacer le curseur après la première position du serpent
void replacerCurseurApres(int x, int y) {
    gotoXY(x + 1, y);  // Déplacer le curseur une position après la tête du serpent
    fflush(stdout);  // Forcer l'affichage immédiat
}

void progresser(int lesX[], int lesY[]) {
    // Décaler les coordonnées du corps du serpent
    for (int i = N - 1; i > 0; i--) {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }
    lesX[0]++;  // La tête avance d'une colonne vers la droite
}

int kbhit() {
    // La fonction retourne 1 si un caractère est présent
    // 0 si pas de caractère présent
    int unCaractere = 0;
    struct termios oldt, newt;
    int ch;
    int oldf;

    // Mettre le terminal en mode non-bloquant
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
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

    return unCaractere;
}
