#include "tableau.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>


//#define VIDE -1

/*
* Crée un tableau et s'occupe de toute l'initialisation des cases
* Renvoie un Tableau (= Case**)
*/
Tableau CreerTableau()
{
  Tableau t = (Tableau) malloc(sizeof(Case*) * TAB_W);

  for(int i=0; i<TAB_W; ++i)
  {
    t[i] = (Case*) malloc(sizeof(Case)*TAB_H);

    for (int j=0; j<TAB_H; ++j)
    {
      t[i][j].id = -1; // On initialise tout le tableau à -1 pour faciliter la génération
    }
  }

  return t;
}

/*
* Libère un tableau de la mémoire
*/
void LibererTableau(Tableau t)
{
  for (int i=0; i<TAB_W; ++i)
  {
    free(t[i]);
  }
  free(t);
  t = NULL;
}

int GenererTerritoire(Tableau t, int nbJoueurs)
{
  int nbTerritoires = 4 * (nbJoueurs + 1); // On définie le nombre total de territoires sur le terrain

  int dW = (int) (TAB_W / (nbJoueurs + 1)) + 1;
  int dH = (int) (TAB_H / (nbJoueurs + 1)) + 1;
  printf("%d, %d, %d\n", nbTerritoires,  dW, dH);

  int seq[(nbJoueurs + 1)];
  sequence(seq, (nbJoueurs + 1));

  // Génère des points répartis plus ou moins uniformément et équitablement sur la grille
  for (int i=0; i<(nbJoueurs + 1); ++i)
  {
    for (int j=0; j<(nbJoueurs + 1); ++j)
    {
        t[i*dW][j*dH].id = i * (nbJoueurs + 1) + j;
    }
  }

  Tableau tmp = CreerTableau();

  // Génération des territoires
  while(!genFini(t))
  {
    // On commence par copier le tableau dans un tableau temporaire
    copierTableau(tmp, t);
    // Puis on le parcours
    for (int i=0; i<TAB_W; ++i)
    {
      for (int j=0; j<TAB_H; ++j)
      {
        if(t[i][j].id == -1)
        {
          int v = 4 - nbVoisinsVides(t, i, j); // Nombre de voisins non vide de i, j

          if (v >= 1) // Au moins une case voisine occupée
          {
            int n = rand() % v; // On choisit une des cases occupées voisines
            int c = 0;


            if ( (i-1 >= 0) && t[i-1][j].id > -1 && c == n)
            {
              tmp[i][j].id = t[i-1][j].id;  // On la copie
              c++;
            }
            else c++;

            if ( (j-1 >= 0) && t[i][j-1].id > -1 && c == n)
            {
              tmp[i][j].id = t[i][j-1].id;
              c++;
            }
            else c++;

            if ( (i+1 < TAB_W) && t[i+1][j].id > -1 && c == n)
            {
              tmp[i][j].id = t[i+1][j].id;
              c++;
            }
            else c++;

            if ( (j+1 < TAB_H) && t[i][j+1].id > -1 && c == n)
            {
              tmp[i][j].id = t[i][j+1].id;
              c++;
            }
          }
        }
      }
    }
    copierTableau(t, tmp);
  }
  LibererTableau(tmp);

  return (nbJoueurs+1)*(nbJoueurs+1); // Renvoie le nombre de territoires créés
}

int GenererTerritoire2(Tableau t, int nbTerritoires)
{
  int x = 0,
      y = 0;

  Point pt[nbTerritoires];
  for(int a = 0; a < TAB_W; a++)
  {
    for(int b = 0; b < TAB_H; b++)
    {
      t[a][b].id = -1;
    }
  }
  int i = 0;
  while(i != (nbTerritoires))
  {
    x = rand() % TAB_W;
    y = rand() % TAB_H;

    if(t[x][y].id == -1)
    {
      pt[i].x = x;
      pt[i].y = y;
      t[x][y].id =  i;
      i++;
    }
  }
  //_print_tab(t);

  for(int i=0; i<TAB_W; ++i)
  {
    for(int j=0; j<TAB_H; ++j)
    {
      if(t[i][j].id == -1)
      {
          int p = distanceMin(i, j, pt, nbTerritoires);
          t[i][j].id = t[ pt[p].x ][ pt[p].y ].id;
      }
    }
  }
  return nbTerritoires;
}


int distanceMin(int x, int y, Point *pt, int pt_size)
{
  int min = -1;
  int imin = -1;
  int d = 0;

  for (int i=0; i<pt_size; ++i)
  {
      d = abs(x - pt[i].x) + abs(y - pt[i].y); // Norme de Manhattan

      if(min == -1 || d < min)
      {
        min = d;
        imin = i;
      }
  }
  //printf(":%d:%d\n", min, imin);
  return imin;
}

int  nbVoisinsVides(Tableau t, int x, int y)
{
  int n = 0;

  if ( (x-1 >= 0)     && t[x-1][y].id == -1) n++;
  if ( (y-1 >= 0)     && t[x][y-1].id == -1) n++;
  if ( (x+1 < TAB_W)  && t[x+1][y].id == -1) n++;
  if ( (y+1 < TAB_H)  && t[x][y+1].id == -1) n++;

  return n;
}

int voisinsCase(Tableau t, int x, int y, int v[6])
{
  int n = 0;
  // La séquence de x à regarder dépend de la parité de la ligne
  int *a = (y % 2 == 0) ? (int[6]) {x-1, x, x+1, x, x-1, x-1} : (int[6]) {x, x+1, x+1, x+1, x, x-1};
  // La séquence de y à regarder est constantes
  int b[6] = {y-1, y-1, y, y+1, y+1, y};

  for (int i=0; i<6; ++i)
  {
    if(a[i] >= 0 && a[i] < TAB_W && b[i] >= 0 && b[i] < TAB_H)
    {
      v[i] = t[a[i]][b[i]].id; // lol
      (t[x][y]).voisins[i] = t[a[i]][b[i]].id;
      if (t[a[i]][b[i]].id == -1) n++;
    }
    else
    {
      v[i] = -1;
      (t[x][y]).voisins[i] = -1;
    }

  }
  return n;
}

void copierTableau(Tableau dst, Tableau src)
{
  for (int i=0; i < TAB_W; ++i)
  {
    memcpy(dst[i], src[i], TAB_H * sizeof(Case));
  }
}

void sequenceAleatoire(int *t, int n)
{
  // On commence par remplir le tableau de 0 à n-1
  for (int i=0; i<n; ++i)
    t[i] = i;

  int tmp = 0;
  int r = 0;

  // On permutte les cases au hasard pour mélanger
  for (int i=0; i<n; ++i)
  {
    r = rand() % n;
    tmp = t[i];
    t[i] = t[r];
    t[r] = tmp;
  }
}

void sequence(int *t, int n)
{
  for (int i=0; i<n; ++i)
    t[i] = i;
}

int genFini(Tableau t)
{
  for (int i=0; i<TAB_W; ++i)
  {
    for (int j=0; j<TAB_H; ++j)
    {
      if (t[i][j].id == -1) // Si on tombe sur -1, on a pas fini de générer le tableau
        return 0;
    }
  }
  return 1;
}

void resultats(int nbPlay)
{
  printf("\n");
  FILE *pFile , *pFile2;
  int found, ind = 0, nbPlayers = 0;
  char c;
  char name[100] = {'\0'};
  int *score = (int*)calloc(nbPlay, sizeof(int));
  char **players = (char**)malloc(sizeof(char*) * nbPlay);
  for (int i=0; i < nbPlay; i++){
    players[i] = (char*)malloc(100*(sizeof(char)));
    for (int j=0; j<100; j++) players[i][j]='\0';
  }

  if ((pFile = fopen("./resultat.txt", "r")) && (pFile2 = fopen("./resultats_finaux.txt", "a+")) )
  {
    rewind(pFile);
    for (c = fgetc(pFile); ! feof(pFile); c = fgetc(pFile)) {
      found = 0;
      if (c != '\t'){
        name[ind++] = c;
      } else {
        for (int i=0; i<nbPlay; i++){ // On cherche si le joueur gagnant est deja dans players
          if (strcmp(name, players[i]) == 0){
            score[i] ++;
            found = 1;
            break;
          }
        }
        if (found){
          for (int j=0; j<100; j++) name[j] = '\0'; // On reinitialise name
        } else {
          for (int k=0; k<100; k++) {
            players[nbPlayers][k] = name[k];
            name[k] = '\0';
          }
          score[nbPlayers++] ++;
        }
        ind = 0;
      }
    }
    for (int j=0; j<nbPlayers; j++){
	    fprintf(pFile2, "%s a obtenu %d victoires.\n", players[j], score[j]);
    }
  fclose(pFile);
  fclose(pFile2);
  free(score);
  for(int i = 0; i < nbPlay; i++)
    free(players[i]);
  free(players);
  } else printf("Erreur d'ouverture du fichier resultats\n");
}


void _print_tab(Tableau t)
{
  for (int i=0; i<TAB_W; ++i)
  {
    if (i%2 == 1) printf("  ");
    for (int j=0; j<TAB_H; ++j)
      printf("%02d  ", t[i][j].id); // Affichage de la transposée qui correspond à l'affichage réel

    printf("\n");
  }
}
