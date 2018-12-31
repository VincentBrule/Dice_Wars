#ifndef H_ARBITRE
#define H_ARBITRE

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <dlfcn.h> //utilisé pour dlopen

#define NBTEXTURE 14

typedef void (*pfInitGame)(unsigned int id, unsigned int nbPlayer, SPlayerInfo *info);
typedef int (*pfPlayTurn)(unsigned int id, const SMap *map, STurn *turn);
typedef void (*pfEndGame)(unsigned int id, unsigned int idWinner);

typedef struct PFonctionsIA //Structure contenant les différents types de pointeur de fonction
{
  pfInitGame InitGame;
  pfPlayTurn PlayTurn;
  pfEndGame EndGame;

} PFonctionsIA;

//Fonctions pour l'IA////////////
int initialisationPointeursIA(PFonctionsIA *tabIA, int nbIA, void **tabLib);
void initialisationIA(int nbIA, int *joueursIA, int nbJoueurs, void **tabLib, SPlayerInfo *infoIA, PFonctionsIA *tabIA, char **texteNomJoueur);
int gestionCompleteIA(SDL_Rect tabEmplacement[TAB_W][TAB_H], Tableau t, SMap *g, SDL_Texture* tabTexture[NBTEXTURE], SDL_Renderer *render, int selection[TAB_W][TAB_H], unsigned int *tourDeJeu, char texteResultatDe[100], char texteVictoireDefaite[20], STurn *coups, int tailleTableauDes, SDL_Rect **tabEmplacementDes, TTF_Font *police, SDL_Color couleurNoire, char **texteNomJoueur, int nbJoueur, int *joueursIA, int nombreJoueursIA, int *sortie, int *sortieIA);
SMap* creerMapVierge();

//Fonctions pour la gestion du jeu///////////
int estDansTableau(int valeur,int* tableau,int longueurTableau);
int finTour(SMap *g, unsigned int *tourDeJeu, Tableau t, SDL_Texture *tabTexture[NBTEXTURE], SDL_Renderer *render, SDL_Rect tabEmplacement[TAB_W][TAB_H],  int selection[TAB_W][TAB_H], int tailleTableauDes, SDL_Rect **tabEmplacementDes,  TTF_Font *police, SDL_Color couleurNoire, char texteResultatDe[100], char texteVictoireDefaite[20],  char **texteNomJoueur, int nbJoueur);
int distributionDes(int nbDes, SMap *g, int ownerR, Tableau t, SDL_Texture *tabTexture[NBTEXTURE], SDL_Renderer *render,  SDL_Rect tabEmplacement[TAB_W][TAB_H],  int selection[TAB_W][TAB_H], int tailleTableauDes, SDL_Rect **tabEmplacementDes,  TTF_Font *police, SDL_Color couleurNoire, char texteResultatDe[100], char texteVictoireDefaite[20],  char **texteNomJoueur, int nbJoueur);
int nbTerritoireFinTour(int ownerRef, SMap *g);
int verificationCoordonnees(int coordX, int coordY, SDL_Rect tabEmplacement[TAB_W][TAB_H], int *emplacementX, int *emplacementY);
int gestionEvenement(SDL_Event event, SDL_Rect tabEmplacement[TAB_W][TAB_H], Tableau t, SMap *g, SDL_Texture* tabTexture[NBTEXTURE], SDL_Renderer *render, int selection[TAB_W][TAB_H], unsigned int *tourDeJeu, char texteResultatDe[100], char texteVictoireDefaite[20], STurn *coups, int tailleTableauDes, SDL_Rect **tabEmplacementDes, TTF_Font *police, SDL_Color couleurNoire, char **texteNomJoueur, int nbJoueur,  int *fin);
int interactionQuitte();
void selectionCase(int id, Tableau t, int selection[TAB_W][TAB_H], int remiseAZero);
int verifAttaque(SMap *g, int idAttaquant, int idTerritoireAttaque, Tableau t, int ownerR);
void deselectionCase(int selection[TAB_W][TAB_H]);
void attaque(STurn *coups, char texteResultatDe[100], char texteVictoireDefaite[20], SMap *g);
int verifFinTour(int coordX, int coordY);
int verifVictoire(SMap *g, int ownerRef);
void interactionTerritoire(int idEnCours, unsigned int *tourDeJeu, SMap *g, Tableau t, STurn *coups, int selection[TAB_W][TAB_H],  char texteResultatDe[100], char texteVictoireDefaite[20], int *fin, int *selectionJoueur, char **texteNomJoueur);

//Fonctions pour l'initialisation du jeu///////////
char** tableauNomJoueur(int nb);
void emplacementCasesEtDes(SDL_Rect tabEmplacement[TAB_W][TAB_H], Tableau t, Point *tabID,  SMap *g, int tailleTableauDes, SDL_Texture* tabTexture[NBTEXTURE], SDL_Rect **tabEmplacementDes);
int idMax(Tableau t);

#endif // H_ARBITRE
