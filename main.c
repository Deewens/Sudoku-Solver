#include <stdio.h>
#include <stdlib.h>
#include "structures/struct.h"

void lireGrille(int G[9][9], char *cible, int *cont); /* Définition : Tableau dans lequel sera stocké la grile, fichier contenant la grille */
void ecrireGrille(int G[9][9]); /* Fonction qui affiche la grille de Sudoku initiale */
int estCand(int G[9][9], int candidat, Case coord); /* Fonction qui resort 1 ou 0 (vrai ou faux) selon la valeur d'un nombre (candidat) dans une case donnée */
int ligne(int G[9][9], int candidat, Case coord); /* Fonction qui vérifie si une valeur se trouve sur une ligne donnée de la grille */
int colonne(int G[9][9], int candidat, Case coord); /* Fonction qui vérifie si une valeur se trouve sur une colonne donnée */
int bloc(int G[9][9], int candidat, Case coord); /* Fonction qui vérifie si une valeur se trouve dans un bloc donné */
void initJeu(int G[9][9], Cand C[9][9], Case O[81], int *NBO); /* Initialise NBO, O[81] et C[][].nbc / C[][].tab */
int appartient(Cand C[9][9], Case coord, int chiffre); /* Vérifie si un chiffre appartient à une case des candidats */
int estCandUnique(Cand C[9][9], Case coord, int candidat); /* Vérifie si un candidat est unique ou non */
int rechCaseUnique(Cand C[9][9]); /* Recherche si une case est unique ou non */
void fermerCase(int G[9][9], Cand C[9][9], Case coord, Case O[81], int *NBO); /* Ferme une case avec son candidat unique */
void ecrireCand(Cand C[9][9]); /* Ecrit la liste des candidats */
void fermerGrille(int G[9][9], Cand C[9][9], Case coord, Case O[81], int NBO); /* Ferme la grille avec des candidats unique */

int main(void) {
  int G[9][9] = {{0}, {0}}; /* Tableau contenant la grille */
  Cand C[9][9]; /* Tableau contenant les candidats de chaque case de la grille */
  int NBO = 0; /* Nombre de case(s) ouverte(s) */
  Case O[81];
  Case coord;
  int i, j;
  int cont;
  char nom[100];

  printf("Entrer le nom du fichier + son extension (.txt) : \n");
  scanf("%s", nom);

  lireGrille(G, nom, &cont);

  if (!cont) {
    printf("Le fichier n'est pas valide ! \n");
  } else {
      printf("Voici la grille original :\n");
      ecrireGrille(G);
      printf("Initialisation du jeu...\n");
      initJeu(G, C, O, &NBO);
      printf("Initialisation des candidats possibles :\n");
      ecrireCand(C);
      printf("Il y a %d cases ouvertes.\n", NBO);
      printf("\n");

      printf("Lancement du programme :\n");
      fermerGrille(G, C, coord, O, NBO);
  }


  return 0;
}

void lireGrille(int G[9][9], char *cible, int *cont) { /* Lire une grille dans un fichier et la stocke dans un tableau à deux dimensions (G) */

  int i, j, k;
  int car = 0;

  FILE* fichier = NULL;

  fichier = fopen(cible, "r");

  if (fichier == NULL) {
    printf("Erreur lors de l'ouverture de la grille de Sudoku !\n");
    *cont = 0;
  } else {
    printf("L'ouverture du fichier a été réalisé avec succès !\n");
    *cont = 1;
    do {
      fscanf(fichier, "%d %d %d", &i, &j, &k); /* A expliquer ... */
      G[i][j] = k;
    } while ((car = fgetc(fichier) != EOF));
    fclose(fichier);
  }
}

void ecrireGrille(int G[9][9]) { /* Permet d'afficher une grille de Sudoku */
  int i, j;

  printf("\n");
  printf("-------------------------\n");
  for (i = 0; i < 9; i++) {
    printf("| ");
    for (j = 0; j < 9; j++) {
      if (j == 2 || j == 5) {
        printf("%d ", G[i][j]);
        printf("| ");
      } else {
        printf("%d ", G[i][j]);
      } if ((i == 2 && j == 8) || (i == 5 && j == 8)) {
        printf("|");
        printf("\n|-----------------------");
      }
    }
    printf("|");
    printf("\n");
  }
  printf("-------------------------\n");
}

int estCand(int G[9][9], int candidat, Case coord) { /* Renvoie 1 si une des trois fonctions (ligne, colonne et bloc) sont fausses, sinon renvoie 0 */
  if (!ligne(G, candidat, coord) && !colonne(G, candidat, coord) && !bloc(G, candidat, coord)) {
    return 1;
  }

  return 0;
}

int ligne(int G[9][9], int candidat, Case coord) { /* Renvoie 1 si "candidat" se trouve sur la même ligne */
  int j;

  for (j = 0; j < 9; j++) {
    if (G[coord.x][j] == candidat) {
      return 1;
    }
  }

  return 0;
}

int colonne(int G[9][9], int candidat, Case coord) { /* Renvoie 1 si "candidat" se trouve sur la même colonne */
  int i;

  for (i = 0; i < 9; i++) {
    if (G[i][coord.y] == candidat) {
      return 1;
    }
  }

  return 0;
}

int bloc(int G[9][9], int candidat, Case coord) { /* Renvoie 1 si "candidat" se trouve dans le même bloc (carré de 3x3) */
  int x = coord.x - (coord.x % 3);
  int y = coord.y - (coord.y % 3);

  for (coord.x = x; coord.x < x + 3; coord.x++) {
    for (coord.y = y; coord.y < y + 3; coord.y++) {
      if (G[coord.x][coord.y] == candidat) {
        return 1;
      }
    }
  }

  return 0;
}

void initJeu(int G[9][9], Cand C[9][9], Case O[81], int *NBO) { /* Initialise C, O et NBO */
  int i, k = 0, l = 0;
  int ferme;
  int *tmp;
  int NBOtmp = 0;
  Case coord;

  for (coord.x = 0; coord.x < 9; coord.x++) {
    for (coord.y = 0; coord.y < 9; coord.y++) {
      /* Initialisation de C[9][9] */
      ferme = G[coord.x][coord.y] == 0; /* 0 si la case est ouverte, 1 sinon */
      C[coord.x][coord.y].nbc = 0;

      if (!ferme) { /* Si la case n'est pas ouverte */
        C[coord.x][coord.y].nbc = 0;
        C[coord.x][coord.y].tab = NULL;
      }
      else {
        O[NBOtmp].x = coord.x;
        O[NBOtmp].y = coord.y;
        NBOtmp++; /* mieux ici le NBO */
        l = 0;
        for (k = 1; k <= 9; k++) { /* Vérification des candidats de 1 à 9 */
          if (estCand(G, k, coord)) { /* Si un candidat fonctionne (==1), on l'initialise dans Cand C */
            C[coord.x][coord.y].nbc++; /* Incrémentation du nombre de valeurs possible pour une case */

            if (l == 0) {
              C[coord.x][coord.y].tab = malloc(sizeof(int));
              if (C[coord.x][coord.y].tab == NULL) {
                printf("Erreur d'allocation de C[coord.x][coord.y].tab !\n");
              }
            }
            else {
              tmp = malloc(C[coord.x][coord.y].nbc * sizeof(int));
              if (tmp == NULL) {
                printf("Erreur d'allocation de tmp !\n");
              }
              for (i = 0; i < C[coord.x][coord.y].nbc - 1; ++i) {
                tmp[i] = C[coord.x][coord.y].tab[i];
              }

              free(C[coord.x][coord.y].tab);
              C[coord.x][coord.y].tab = tmp;
            }

            C[coord.x][coord.y].tab[l] = k;
            l++;
          }
        }
      }
    }
  }

  *NBO = NBOtmp;
}

int appartient(Cand C[9][9], Case coord, int chiffre) {
  int i;
  for (i = 0; i < C[coord.x][coord.y].nbc; i++) {
    if (C[coord.x][coord.y].tab[i] == chiffre && C[coord.x][coord.y].tab != NULL) {
      return 1;
    }
  }
  return 0;
}

int estCandUnique(Cand C[9][9], Case coord, int candidat) {
  if (appartient(C, coord, candidat) && C[coord.x][coord.y].nbc == 1) {
    return 1;
  }

  return 0;
}

int rechCaseUnique(Cand C[9][9]) {
  Case coord;
  int i;

  for (coord.x = 0; coord.x < 9; coord.x++) {
    for (coord.y = 0; coord.y < 9; coord.y++) {
      for (i = 0; i <= 9; i++) {
        if (estCandUnique(C, coord, i)) {
          return 1;
        }
      }
    }
  }
  return 0;
}


void fermerCase(int G[9][9], Cand C[9][9], Case coord, Case O[81], int *NBO) {
  int NBOtmp;

  G[coord.x][coord.y] = C[coord.x][coord.y].tab[0];
  initJeu(G, C, O, &NBOtmp);

  *NBO = NBOtmp;
}

void fermerGrille(int G[9][9], Cand C[9][9], Case coord, Case O[81], int NBO) {
  int i;

  while (rechCaseUnique(C)) {
    for (coord.x = 0; coord.x < 9; coord.x++) {
      for (coord.y = 0; coord.y < 9; coord.y++) {
        for (i = 0; i <= 9; i++) {
          if (estCandUnique(C, coord, i)) {
            printf("Voici la liste des %d candidats restant :\n", NBO);
            ecrireCand(C);
            fermerCase(G, C, coord, O, &NBO);
            printf("La case (%d,%d) a été fermé avec le chiffre %d.\n", coord.x, coord.y, i);
            ecrireGrille(G);
          }
        }
      }
    }
  }

  if (NBO != 0) {
    printf("La grille est trop difficile pour être résolu, toutes les candidats uniques ont été mis. Voici les candidats restant :\n");
    ecrireCand(C);
  }
}

void ecrireCand(Cand C[9][9]) {
  Case coord;
  int k;

  printf("\n");
  printf("*************************************************************\n");
  for (coord.x = 0; coord.x < 9; coord.x++) {
    printf("* ");
    for (coord.y = 0; coord.y < 9; coord.y++) {
      if (C[coord.x][coord.y].nbc == 0) {
        printf("   ");
      } else {
        for (k = 0; k < C[coord.x][coord.y].nbc; k++) {
          printf("%d", C[coord.x][coord.y].tab[k]);
        }
      }
      if (coord.y == 2 || coord.y == 5 || coord.y == 8) {
        printf(" * ");
      } else {
        printf(" | ");
      }
    }
    printf("\n");
    if (coord.x == 2 || coord.x == 5 || coord.x == 8) {
      printf("*************************************************************\n");
    } else {
      printf("*-------------------------------------------------------------*\n");
    }
  }
}

/* Mémo

--Pour afficher C.tab--

for(coord.x = 0; coord.x < 9; coord.x++) { // Affichage test de C.tab
  for(coord.y = 0; coord.y < 9; coord.y++) {
    for (int i = 0; i < C[coord.x][coord.y].nbc; ++i) {
      printf("%d", C[coord.x][coord.y].tab[i]);
    }
    printf(" ");
  }
}

--Pour afficher C.nbc--

printf("%d ", C[coord.x][coord.y].nbc);

--Pour afficher NBO et O--

printf("%d\n", NBO);

for(int i = 0; i < NBO; i++) {
  printf("%d ", O[i].x);
  printf("%d\n", O[i].y);
}

*/