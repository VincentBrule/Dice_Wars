#ifndef H_AFFICHAGE
#define H_AFFICHAGE

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "tableau.h"
#include "interface.h"
#include "graphe.h"
#include <dlfcn.h> //utilisé pour dlopen
#include "arbitre.h"

#define xstr(s) str(s) //permet de simplifier ecriture des chemins
#define str(s) #s

#define cheminSpriteCases ./sprites/cases/ //chemin vers les png des hexagones
#define cheminSpriteDes ./sprites/dices/ //chemin vers les png des dés

#define numPolice 1 //numéro de la police souhaitée
#define LARGEUR 1400
#define HAUTEUR 900
#define NBTEXTURE 14
#define DECALAGE 40 //permet que l'ecran ne soit pas collé en haut pour que l'affichage des dés ne dépasse pas de l'écran
#define DELAY_AFFICHAGE 1 //1 pour avoir un rendu normal, 0 pour un rendu rapide
#define NB_SOMMET 60
#define TOUR_ALEATOIRE 1 //1 pour que les tour de jeu  soient tirés aléatoirement
#define AVEC_AFFICHAGE 1 //0 pour annuler affichage et plus de rapidité dans les tests



//Fonctions d'affichage////////////////
void tourDeDes(Point *tabID, Tableau t, SMap *g, int tailleTableauDes, int i, int j);
void afficherDesFinTour(SDL_Texture *tabTexture[NBTEXTURE], SDL_Renderer *render, int nb, TTF_Font *police, SMap *g, SDL_Color couleurNoire);
void afficherTexteDesPile(TTF_Font *police, SDL_Color couleurNoire, SDL_Renderer *render, unsigned int tourDeJeu, unsigned int *stack);
void affichageMap(Tableau t, SMap *g, SDL_Renderer *render, SDL_Texture* tabTexture[NBTEXTURE],  SDL_Rect tabEmplacement[TAB_W][TAB_H],  int selection[TAB_W][TAB_H], int tailleTableauDes, SDL_Rect **tabEmplacementDes, unsigned int tourDeJeu, TTF_Font *police, SDL_Color couleurNoire, char texteResultatDe[100], char texteVictoireDefaite[20],  char **texteNomJoueur, int nbJoueur);
void affichageDes(int tailleTableauDes, SDL_Texture *tabTexture[NBTEXTURE],  SDL_Renderer *render,  SMap *g, SDL_Rect **tabEmplacementDes);
void affichageFrontieres(SDL_Renderer *render, SMap *g, Tableau t);
void affichageHexagones(SDL_Renderer *render, SMap *g, Tableau t, SDL_Rect tabEmplacement[TAB_W][TAB_H], SDL_Texture *tabTexture[NBTEXTURE], int selection[TAB_W][TAB_H]);
void afficherBoutonFinTour(SDL_Texture *tabTexture[NBTEXTURE], SDL_Renderer *render);
void afficherTexteResultatDes(char texte[100],char texteVictoireDefaite[20], TTF_Font *police, SDL_Color couleurNoire, SDL_Renderer *render);
void afficherTexteTourDeJeu(TTF_Font *police, SDL_Color couleurNoire, SDL_Renderer *render, unsigned int tourDeJeu, SDL_Texture* tabTexture[NBTEXTURE], char **texteNomJoueur);
int chargementSprites(SDL_Texture* tabTexture[NBTEXTURE], SDL_Renderer *render, SDL_Surface* tabSurfaceTexture[NBTEXTURE]);


//Fonctions pour l'initialisation du jeu///////////
char** tableauNomJoueur(int nb);
void emplacementCasesEtDes(SDL_Rect tabEmplacement[TAB_W][TAB_H], Tableau t, Point *tabID,  SMap *g, int tailleTableauDes, SDL_Texture* tabTexture[NBTEXTURE], SDL_Rect **tabEmplacementDes);
int idMax(Tableau t);

//Fonctions pour la libération des ressources
void libererTableauNom(char **t, int nb);
void liberationMemoire(SDL_Texture* tabTexture[NBTEXTURE], SDL_Surface* tabSurfaceTexture[NBTEXTURE], SDL_Renderer *render, SDL_Window* window);
void libererTableauDes(int tailleTableauDes, SDL_Rect **tabEmplacementDes);
Point* genererTableauAffichageDes(int id);
SDL_Rect** genererTableauEmplacementDes(int tailleTableauDes);

#endif // H_AFFICHAGE
