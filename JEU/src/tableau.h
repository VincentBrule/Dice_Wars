#ifndef H_TABLEAU
#define H_TABLEAU

#define TAB_W 70 // Largeur du tableau
#define TAB_H 75 // Hauteur du tableau

// Structure d'une case (un hexagone)
typedef struct Case
{
  int id;         // Stocke l'id du territoire auquel la case appartiendra
  int voisins[6]; // Stocke l'id des six voisins
} Case;

// Structure Point générique
typedef struct Point
{
  int x;
  int y;
  int nbX;
  int nbY;
} Point;


typedef Case** Tableau; // On définie un Tableau comme étant un tableau de TAB_W*TAB_H Cases
// Chaque case contient l'ID du territoire auquel elle appartient

// Renvoi un tableau de la bonne taille
Tableau CreerTableau();



// Obsolète
int GenererTerritoire(Tableau t, int nbJoueurs);

// Génère les territoires
// Renvoie le nombre de territoires créés
// Diagramme de Voronoi
int GenererTerritoire2(Tableau t, int nbTerritoires);

// Libère le tableau
void LibererTableau(Tableau t);


/////// Fonctions internes

// Renvoie une séquence aléatoire entre 0 et n-1 dans t
void sequenceAleatoire(int *t, int n);

// Renvoie le tableau t avec la séquence [0, 1, 2, ..., n-1]
void sequence(int *t, int n);

// Renvoie vrai si la génération est fini
int genFini(Tableau t);

// Copie le contenu de src dans dst
void copierTableau(Tableau dst, Tableau src);

// Compte le nombre de voisins vide (-1) de la case x,y
// OBSOLETE: ne prend pas en compte les six voisins
int  nbVoisinsVides(Tableau t, int x, int y);

// Compte le nombre de voisin de la case x,y et stocke leur id dans v
// Les voisins id=-1 ne sont pas comptés dans la valeur de retour
int voisinsCase(Tableau t, int x, int y, int v[6]);

// Distance de Manhattan entre la case x,y et la case pt
int distanceMin(int x, int y, Point *pt, int pt_size);

void resultats(int nbPlayers);

// Print du tableau
// Ne marche que si le tableau est carré à cause de l'inversion de x et y
// Pour pouvoir afficher dans le bon sens
void _print_tab(Tableau t); // Pour debug seulement

#endif // H_TABLEAU
