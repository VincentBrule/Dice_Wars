#ifndef H_GRAPHE
#define H_GRAPHE

#include "interface.h"
#include "tableau.h"


// Créé un graphe à partir du tableau généré précédemnt
SMap* CreerMap(Tableau t, int nbTerritoires, int nbJoueurs);
void DetruireMap(SMap *map); // Destructeur

// Copie une SMap dans un nouveau pointeur SMap* et le renvoie
SMap* copierMap(SMap *src, int nbJoueurs);

// Renvoie le nombre max de territoires contigus d'un joueur
int maxTerritoiresContigus(SMap *map, int idJoueur);

////////////////////////
// Fonctions internes //
////////////////////////

// Renvoie le nombre de voisins de id et stocke ces voisins dans v
int voisins(Tableau t, int id, int nbTerritoires, int *v);

// Indique si i est dans le tableau t de taille size
int estDans(int i , int* t, int size);

// Renvoie le  nombre de territoires contigus appartenant au même propriétaire
// Pour un territoire id donné (donc pour un groupe de territoire seulement)
int nbTerritoiresContigus(SMap *map, int id);
// Idem mais vérifie la connexité du graphe
int nbTerritoireConnexe(SMap *map, int id);

int estSeul(SCell *t);

void printNei(SCell *c); // Affiche les voisins de c
void printMap(SMap *m);  // Affiche les cellules et leurs voisins de m


#endif//  H_GRAPHE
