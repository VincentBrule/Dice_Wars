#include "graphe.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//Fonction générant le graphe à partir du tableau t
SMap* CreerMap(Tableau t, int nbTerritoires, int nbJoueurs)
{
  SMap *map = (SMap*) malloc(sizeof(SMap));
  int *nbTerritoiresJoueur = (int*)calloc(nbJoueurs, sizeof(int)); // Vérifie que tous les joueurs ont le meme nombre de territoir
  int verifTerriNeutre = 0; // Vérifie qu'on a le bon nombre de territoires Neutres
  int maxTerri = nbTerritoires/nbJoueurs; // nb de territoire par joueur
  int nbTerritoiresNeutres = nbTerritoires - (maxTerri*nbJoueurs); // nb de territoire neutre
  if(nbTerritoiresNeutres == 0)
  {
    nbTerritoiresNeutres = 3*nbJoueurs;
    maxTerri -= 3;
  }
  else if(nbTerritoiresNeutres < 0) nbTerritoiresNeutres = 0;
  int randomTerriNeutre = rand()% 10;
  if(randomTerriNeutre < 2)
  {
    nbTerritoiresNeutres += nbJoueurs;
    maxTerri -=1;
  }
  int numeroJoueur = 0;
  int neutre = 0;
  int nbDesParJoueur = maxTerri * 3; // nb de dés par joueur
  int *tabDeJoueur = (int*)calloc(nbJoueurs, sizeof(int)); // vérifie que le nombre de de max n'est pas depassé
  map->nbCells = nbTerritoires;
  map->cells = (SCell*) malloc(nbTerritoires * sizeof(SCell));
  map->stack = (unsigned int*)calloc(nbJoueurs, sizeof(unsigned int));

  int *v = (int*) malloc(nbTerritoires * sizeof(int)); // Tableau temporaire pour stocker les voisins d'un territoire

  int connexe = 0; // Variable temporaire qui servira à vérifier qu'un territoire n'est pas seul (et donc que le graphe est connexe)

  // Tant que le graphe n'est pas connexe
  while (connexe == 0)
  {
    connexe = 0;
    // On initialise chaque territoire
    for (int i=0; i < nbTerritoires; ++i)
    {
        map->cells[i].id = i;
        //Choix du proprietaire d'un hexagone/////////
        do {
          numeroJoueur = (rand() % (nbJoueurs)) + 1;
          if(verifTerriNeutre == nbTerritoiresNeutres) neutre = 1;
          else neutre = rand() % (nbJoueurs + 1);
        } while((nbTerritoiresJoueur[numeroJoueur - 1] == maxTerri) &&  (neutre != 0));
        if(neutre != 0)
        {
          nbTerritoiresJoueur[numeroJoueur - 1]++;
          map->cells[i].owner = numeroJoueur; //OWNER DU territoires
        }
        else
        {
          verifTerriNeutre++;
          map->cells[i].owner = 0; //OWNER DU territoires
        }
        ///////////////////////////////////////////
        map->cells[i].nbDices = 1;
        // On récupère le nombre de voisins de i et les voisins dans v
        map->cells[i].nbNeighbors = voisins(t, i, nbTerritoires, v); 

        // On alloue l'espace nécéssaire au nombre de voisins (cela ne changera pas au cours de la partie)
        map->cells[i].neighbors = (SCell**) malloc(map->cells[i].nbNeighbors * sizeof(SCell*));
        for (int j=0; j<map->cells[i].nbNeighbors; ++j)
        {
          // On indique dans le tableau la liste des voisins
          // Ce n'est pas grave si le voisin n'est pas encore créé, son espace dans cells est déjà alloué et on indique seulement l'adresse de cet espace
          map->cells[i].neighbors[j] = &map->cells[ v[j] ]; // v[j] est l'id (donc l'indice dans cells) du voisin
        }
    }
    // On vérifie que chaque territoire n'est pas isolé
    for (int i=0; i < nbTerritoires; ++i)
    {
      // Pour cela on vérifie s'il est seul ou non
      if ((map->cells[i].owner != 0) && (nbTerritoireConnexe(map, i) != (maxTerri*nbJoueurs)))
      {
        // le graphe n'est pas connexe
        connexe = 0;
        verifTerriNeutre = 0;
        // on libère les malloc des voisins avant de recommencer
        for (int i=0; i<map->nbCells; ++i)
          free(map->cells[i].neighbors);
        for(int j = 0; j < nbJoueurs; j++)
          nbTerritoiresJoueur[j] = 0;

        // Pas besoin de continuer plus loin
        break;
      }
      connexe = 1;
    }
  }

  /////////////Distribution Dés////////////////////
  int verifDe = 0;
  for(int i = 0; i < nbTerritoires; i++)
  {
    if(map->cells[i].owner != 0) //Si ce n'est pas un territoire neutre
    {
      if(tabDeJoueur[map->cells[i].owner - 1] < nbDesParJoueur) //si on a encore des dés à placer pour le joueur
      {
        verifDe = (rand()%(8)) + 1;
        if((verifDe + tabDeJoueur[map->cells[i].owner - 1])  > nbDesParJoueur) verifDe = (nbDesParJoueur - tabDeJoueur[map->cells[i].owner - 1]); //Dépasse pas le nombre max de dé par joueur
        map->cells[i].nbDices = verifDe;
        tabDeJoueur[map->cells[i].owner - 1] += map->cells[i].nbDices;
      }
    }
  }
  //On repasse pour que tous les joueurs ai le meme nombre de dé//////////////
  int complement = 0;
  for(int j = 0; j < nbJoueurs; j++)
  {
    while(tabDeJoueur[j] < nbDesParJoueur)
    {
      complement = rand()%nbTerritoires;
      if(map->cells[complement].owner == (j+1) && map->cells[complement].nbDices < 8)
      {
        tabDeJoueur[j]++;
        map->cells[complement].nbDices++;
      }
    }
  }
  
  
  // Libération des ressources temporairement allouées
  free(tabDeJoueur);
  free(nbTerritoiresJoueur);
  free(v);
  
  
  return map;

}

// Copie une map en enlevant les liens de tout les voisins owner=0 car ce ne sont pas des vrais joueurs et enlevant -1 à tous les owners pour que ça parte de 0
SMap* copierMap(SMap *src, int nbJoueurs)
{
  SMap *map = (SMap*) malloc(sizeof(SMap));
  map->nbCells = src->nbCells;

  map->stack = (unsigned int*) malloc(nbJoueurs * sizeof(unsigned int));
  for(int i = 0; i<nbJoueurs; i++)
    map->stack[i] = src->stack[i];

  map->cells = (SCell*) malloc(map->nbCells * sizeof(SCell));

  for(int i=0; i<map->nbCells; ++i)
  {
    map->cells[i].id = i;
    map->cells[i].owner = src->cells[i].owner;
    map->cells[i].nbDices = src->cells[i].nbDices;

    // Si l'owner est un territoire neutre, on met les voisins à 0
    if (map->cells[i].owner == 0)
    {
      map->cells[i].nbNeighbors = 0;
      map->cells[i].neighbors = (SCell**) malloc(0); // On réserve un pointeur nul quand même pour la compatibilité avec le free
    }
    else
    {
      map->cells[i].nbNeighbors = src->cells[i].nbNeighbors;
      map->cells[i].neighbors = (SCell**) malloc(map->cells[i].nbNeighbors * sizeof(SCell*)); // On réserve potentiellement plus de voisins que nécéssaire (parce qu'on va enlever les owner=0)
                                                                                              // mais mieux vaut plus que pas assez
      int nbv = 0; // Compteur interne du nombre de voisin sans les owner=0
      for (int j=0; j<src->cells[i].nbNeighbors; ++j)
      {
        if (src->cells[i].neighbors[j]->owner > 0)
        {
          map->cells[i].neighbors[nbv++] = &(map->cells[ src->cells[i].neighbors[j]->id ]);
        }
        // Si on trouve un voisin owner=0 on enlève 1 à nbNeighbors car on ne le compte plus
        else
          map->cells[i].nbNeighbors--;
      }
    }

  }
  for(int i = 0; i < map->nbCells; i++)
    map->cells[i].owner -= 1;
  return map;
}

//Libère la mémoire occupée par la map
void DetruireMap(SMap *map)
{
  for (int i=0; i<map->nbCells; ++i)
    free(map->cells[i].neighbors);

  free(map->cells);
  free(map->stack);
  free(map);
}

//Envoit 1 si le territoire est seul
int estSeul(SCell *t)
{
  for (int i=0; i<t->nbNeighbors; ++i)
  {
      if (t->neighbors[i]->owner > 0)
      {
        return 0;
      }
  }
  return 1;
}

//Renvoit les voisins
int voisins(Tableau t, int id, int nbTerritoires, int *v)
{
  int n = 0; // Compte le nombre de voisins

  for (int x=0; x<TAB_W; ++x)
  {
    for (int y=0; y<TAB_H; ++y)
    {
        if (t[x][y].id == id) // Si on trouve une case id, on observe ses 6 voisins
        {
          int vcell[6] = {0};
          voisinsCase(t, x, y, vcell); // Récupère les voisins de (x,y) dans v

          for (int i=0; i<6; ++i)
          {
            if(vcell[i] != -1 && vcell[i] != id && !estDans(vcell[i], v, n)) // Si le voisin n'est pas déjà présent, on l'ajoute
            {
              v[n++] = vcell[i];
            }
          }
        }
    }
  }
  return n;
}

// Renvoit le nb de territoire contigus afin de verifier si la map est bien connexe
int nbTerritoireConnexe(SMap *map, int id)
{
  int visite[map->nbCells];    // Territoires déjà visités
  int nbVisite = 1;            // Nombre de territoires déjà visités

  int connexe[map->nbCells];   // Territoires connexes
  int n = 1;                   // Nombre de territoires connexes (en comptant le terrioire de départ)
  int m = 0;                   // Compteur interne

  SCell *c = &map->cells[id];  // Territoire de départ
  connexe[0] = c->id;          // Le premier est toujours celui de départ
  visite[0] = c->id;  // Et on a déjà visité la case de départ

  while(m < n) // Tant qu'il reste des territoires contigus à vérifier
  {
    c = &map->cells[ connexe[m] ]; // On passe au territoire suivant

    for (int i=0; i<c->nbNeighbors; ++i)
    {
      SCell *voisin = c->neighbors[i];

      if(!estDans(voisin->id, visite, nbVisite)) // Si l'on a jamais visité le territoire
      {
        visite[nbVisite++] = voisin->id; // Ajout de voisin à la liste des territoires visités

        if (voisin->owner != 0) // On a trouvé un voisin de même propriétaire
          connexe[n++] = voisin->id;    // Ajout du voisin
      }
    }
    m++;
  }
  return n;
}

// Renvoie le  nombre de territoires contigus appartenant au même propriétaire pour une id donnée
int nbTerritoiresContigus(SMap *map, int id)
{
  int visite[map->nbCells];    // Territoires déjà visités
  int nbVisite = 1;            // Nombre de territoires déjà visités

  int connexe[map->nbCells];   // Territoires connexes
  int n = 1;                   // Nombre de territoires connexes (en compteur le terrioire de départ)
  int m = 0;                   // Compteur interne

  SCell *c = &map->cells[id];  // Territoire de départ
  int owner = c->owner;        // Propriétaire du territoire de départ
  connexe[0] = c->id;          // Le premier est toujours celui de départ
  visite[0] = c->id;  // Et on a déjà visité la case de départ

  while(m < n) // Tant qu'il reste des territoires contigus à vérifier
  {
    c = &map->cells[ connexe[m] ]; // On passe au territoire suivant

    for (int i=0; i<c->nbNeighbors; ++i)
    {
      SCell *voisin = c->neighbors[i];

      if(!estDans(voisin->id, visite, nbVisite)) // Si l'on a jamais visité le territoire
      {
        visite[nbVisite++] = voisin->id; // Ajout de voisin à la liste des territoires visités

        if (voisin->owner == owner) // On a trouvé un voisin de même propriétaire
          connexe[n++] = voisin->id;    // Ajout du voisin
      }
    }
    m++;
  }
  return n;
}

//Renvoie le max en appelant la fonction ci-dessus
int maxTerritoiresContigus(SMap *map, int idJoueur)
{
  int n = 0;
  int t = 0;

  for (int i=0; i<map->nbCells; ++i)
  {
      if (map->cells[i].owner == idJoueur)
      {
        t = nbTerritoiresContigus(map, map->cells[i].id);
        n = (t > n) ? t : n;
      }
  }
  return n;
}

// Indique si i est dans le tableau t de taille size
int estDans(int i , int* t, int size)
{
  for (int j=0; j<size; ++j)
  {
    if (t[j] == i) return 1;
  }
  return 0;
}

//Print pour debug
void printNei(SCell *c)
{
  for (int i = 0; i < c->nbNeighbors; ++i)
    printf("%02d (o %d at %p) ", c->neighbors[i]->id, c->neighbors[i]->owner, c->neighbors[i]);
  printf("\n");
}

void printMap(SMap *m)
{
  for (int i = 0; i < m->nbCells; ++i)
  {
    printf("Cell %02d (o %d at %p) : ", m->cells[i].id, m->cells[i].owner, &m->cells[i]);
    printNei(&m->cells[i]);
  }
}
