#include "affichage.h"
#include "arbitre.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


SMap* creerMapVierge()
{
  SMap *map = (SMap*) malloc(sizeof(SMap));
  map->nbCells = 0;
  map->cells = NULL;
  map->stack = NULL;
  return map;
}

//Verifie si quelqu'un a gagné
int verifVictoire(SMap *g, int ownerRef)
{
  for(int i = 0; i < g->nbCells; i++)
  {
    if((ownerRef != (g->cells[i].owner)) && ((g->cells[i].owner) != 0)) return 0;
  }
  return 1;
}

//Fonction gérant les évenement de l'utilisateur
//Prend en paramètre le tableau des emplacements, le tableau des hexagones, le graphe SMap, le tableau de texture,
//le renderer et un tableau selection qui permet de savoir quelle case sont en cours de sélection
int gestionEvenement(SDL_Event event, SDL_Rect tabEmplacement[TAB_W][TAB_H], Tableau t, SMap *g, SDL_Texture* tabTexture[NBTEXTURE], SDL_Renderer *render, int selection[TAB_W][TAB_H], unsigned int *tourDeJeu, char texteResultatDe[100], char texteVictoireDefaite[20], STurn *coups, int tailleTableauDes, SDL_Rect **tabEmplacementDes, TTF_Font *police, SDL_Color couleurNoire, char **texteNomJoueur, int nbJoueur, int *fin)
{
  //SDL_Event event;
  static int selectionJoueur = 0;
  while (SDL_PollEvent(&event)) // Récupération des actions de l'utilisateur
  {
        switch(event.type)
        {
            case SDL_QUIT: // Clic sur la croix
                return 1;
                break;

            case SDL_MOUSEBUTTONDOWN: //Si c'est un clic
                if(event.button.button == 1 && !(*fin)) //Si c'est le bouton gauche
                {
                  strcpy(texteResultatDe, "\0"); //On efface le texte des résultats
                  strcpy(texteVictoireDefaite, "\0");
                  int coordX = event.button.x; //on récupère les coordonnées de la souris
                  int coordY = event.button.y;
                  int emplacementX = 0, emplacementY = 0;
                  if(verifFinTour(coordX, coordY))
                  {
                    if(verificationCoordonnees(coordX, coordY, tabEmplacement, &emplacementX, &emplacementY)) //on regarde sur quel hexagone on a cliqué
                    {
                      interactionTerritoire(t[emplacementX][emplacementY].id, tourDeJeu, g, t, coups, selection, texteResultatDe, texteVictoireDefaite, fin, &selectionJoueur, texteNomJoueur);
                    }
                  }
                  else //si le joueur termine son tour
                  {
                    deselectionCase(selection); //on met à jour le graphisme
                    if(finTour(g, tourDeJeu, t, tabTexture, render, tabEmplacement, selection, tailleTableauDes, tabEmplacementDes, police, couleurNoire, texteResultatDe, texteVictoireDefaite,  texteNomJoueur, nbJoueur))
                    {
                      return 1;
                    }
                  }
                SDL_RenderClear(render); //on actualise l'affichage quand on clique
                affichageMap(t, g, render, tabTexture, tabEmplacement, selection, tailleTableauDes, tabEmplacementDes, *tourDeJeu, police, couleurNoire, texteResultatDe, texteVictoireDefaite,  texteNomJoueur, nbJoueur);//met à jour l'affichage
                }
                while (SDL_PollEvent(&event)); //on vide le buffer au cas où il y a eu des interactions entre temps
                break;
      }
  }
  return 0;
}

//Fonction gérant l'attaque des joueurs
void attaque(STurn *coups, char texteResultatDe[100], char texteVictoireDefaite[20], SMap *g)
{
  int resultatAttaquant = 0, resultatDefenseur = 0;
  int nbDesAttaquant =  (g->cells[coups->cellFrom]).nbDices;
  int nbDesDefenseur =  (g->cells[coups->cellTo]).nbDices;

  for(int i = 0; i < nbDesAttaquant; i++)
  {
    resultatAttaquant += (rand() % (6)) + 1;
  }
  for(int j = 0; j < nbDesDefenseur; j ++)
  {
    resultatDefenseur += (rand() % (6)) + 1;
  }
  sprintf(texteResultatDe, "Attaquant : %d   VS   %d : Defenseur", resultatAttaquant, resultatDefenseur);
  if(resultatDefenseur >= resultatAttaquant) //Defaite
  {
    (g->cells[coups->cellFrom]).nbDices = 1; //on garde juste un dé
    strcpy(texteVictoireDefaite, "Defaite");
  }
  else //Victoire
  {
     (g->cells[coups->cellTo]).owner = (g->cells[coups->cellFrom]).owner; //on devient le propriétaire
     (g->cells[coups->cellTo]).nbDices = (g->cells[coups->cellFrom]).nbDices - 1; //on sépare nos dés
     (g->cells[coups->cellFrom]).nbDices = 1;
    strcpy(texteVictoireDefaite, "Victoire");
  }
}

int verifAttaque(SMap *g, int idAttaquant, int idTerritoireAttaque, Tableau t, int ownerR)
{
  if(g->cells[idAttaquant].owner == ownerR && g->cells[idAttaquant].nbDices > 1)
  {
    for(int i = 0; i < g->cells[idAttaquant].nbNeighbors; i++)
    {
      if(((g->cells[idAttaquant].neighbors[i])->id == idTerritoireAttaque) && (g->cells[idAttaquant].neighbors[i]->owner != ownerR) && (g->cells[idAttaquant].neighbors[i]->owner != 0))
      {
        return 1;
      }
    }
  }
  return 0;
}

//Renvoir le nbre de territoire de ownerRef
int nbTerritoireFinTour(int ownerRef, SMap *g)
{
  int nb = 0;
  for(int i = 0; i < g->nbCells; i++)
  {
    if(((g->cells[i]).owner) == ownerRef)
    {
      nb++;
    }
  }
  return nb;
}

//Gère la distribution des dés à la fin d'un tour
int distributionDes(int nbDes, SMap *g, int ownerR, Tableau t, SDL_Texture *tabTexture[NBTEXTURE], SDL_Renderer *render,  SDL_Rect tabEmplacement[TAB_W][TAB_H],  int selection[TAB_W][TAB_H], int tailleTableauDes, SDL_Rect **tabEmplacementDes,  TTF_Font *police, SDL_Color couleurNoire, char texteResultatDe[100], char texteVictoireDefaite[20],  char **texteNomJoueur, int nbJoueur)
{
  int nbT = nbTerritoireFinTour(ownerR, g); //Nombre de territoire du ownerR
  int n = 0, emplacement = 0, nb2 = 0, j = 0;
  int nbTerritoireAvecPlaceDe = 0;
  SCell **tabC = (SCell**)malloc(sizeof(SCell*) * nbT); //Tableau contenant des pointeurs vers des SCell
  for(int i = 0; i < g->nbCells; i++) //On parcours tous les territoire pour récupérer celle appartenant à owner R
  {
    if((((g->cells[i]).owner) == ownerR) && ((g->cells[i]).nbDices < 8)) //Si on trouve un terrain et qu'on peut lui mettre des dés
    {
      tabC[n++] = &(g->cells[i]); //On garde son pointeur et on incremente le nb de territoire dispo
      nbTerritoireAvecPlaceDe++;
    }
  }
  nb2 = nbTerritoireAvecPlaceDe;
  while((j < nbDes) && (nb2 != 0)) //On veut placer le nb de dé donné en paramètre
  {
    emplacement = (rand()%(nbTerritoireAvecPlaceDe)); //On choisit aléatoirement un terrain où placer les dés
    if(tabC[emplacement] != NULL)
    {
      if(tabC[emplacement]->nbDices < 8) //Si ce terrain a de la place, on y met un de
      {
         selectionCase(tabC[emplacement]->id, t, selection, 1); //on mets la case en surbrillance
         if(AVEC_AFFICHAGE)
         {
           SDL_RenderClear(render); //on peint le fond en blanc
           affichageMap(t, g, render, tabTexture, tabEmplacement, selection, tailleTableauDes, tabEmplacementDes, ownerR, police, couleurNoire, texteResultatDe, texteVictoireDefaite,  texteNomJoueur, nbJoueur); //on l'affiche
         }
         if(DELAY_AFFICHAGE) SDL_Delay(100); //on attends 0.3secs
         if(interactionQuitte() == 1)
         {
           return -1;
         }
         (tabC[emplacement]->nbDices)++; //on ajoute le dé et on met a jour l'affichage
         nbDes--;
         if(AVEC_AFFICHAGE)
         {
           SDL_RenderClear(render); //on peint le fond en blanc
           affichageMap(t, g, render, tabTexture, tabEmplacement, selection, tailleTableauDes, tabEmplacementDes, ownerR, police, couleurNoire, texteResultatDe, texteVictoireDefaite,  texteNomJoueur, nbJoueur);
         }
         if(DELAY_AFFICHAGE) SDL_Delay(300);
         if(interactionQuitte() == 1)
         {
           return -1;
         }
         deselectionCase(selection); //on met à jour le graphisme
      }
      else
      {
        tabC[emplacement] = NULL;
        nb2--; //Sinon on retire 1 au nombre de terrain disponible
      }
    }
  }
  free(tabC); //on désalloue le tableau
  return nbDes; //on renvoit le nombre de dés disponible dans la pile du joueur
}

//Fonction gérant les actions d'une IA
int gestionCompleteIA(SDL_Rect tabEmplacement[TAB_W][TAB_H], Tableau t, SMap *g, SDL_Texture* tabTexture[NBTEXTURE], SDL_Renderer *render, int selection[TAB_W][TAB_H], unsigned int *tourDeJeu, char texteResultatDe[100], char texteVictoireDefaite[20], STurn *coups, int tailleTableauDes, SDL_Rect **tabEmplacementDes, TTF_Font *police, SDL_Color couleurNoire, char **texteNomJoueur, int nbJoueur, int *joueursIA, int nombreJoueursIA, int *sortie, int *fin)
{
			if(verifAttaque(g, coups->cellFrom, coups->cellTo, t, *tourDeJeu))
			{
        selectionCase(coups->cellFrom, t, selection, 1);
        selectionCase(coups->cellTo, t, selection, 0);
		    if(DELAY_AFFICHAGE) SDL_Delay(300);
        if(interactionQuitte() == 1)
        {
          return 1;
        }
        if(AVEC_AFFICHAGE)
        {
          SDL_RenderClear(render); //on peint le fond en blanc
				  affichageMap(t, g, render, tabTexture, tabEmplacement, selection, tailleTableauDes, tabEmplacementDes, *tourDeJeu, police, couleurNoire, texteResultatDe, texteVictoireDefaite,  texteNomJoueur, nbJoueur);//affiche la map
        }
        if(DELAY_AFFICHAGE) SDL_Delay(100);
        if(interactionQuitte() == 1)
        {
          return 1;
        }
        attaque(coups, texteResultatDe, texteVictoireDefaite, g);
        if(verifVictoire(g, (g->cells[coups->cellTo]).owner)) //la partie est finie
        {
          *fin = (g->cells[coups->cellTo].owner); //on met dans fin le joueur qui a gagné
          deselectionCase(selection);
          if(AVEC_AFFICHAGE)
          {
            SDL_RenderClear(render); //on peint le fond en blanc
  				  affichageMap(t, g, render, tabTexture, tabEmplacement, selection, tailleTableauDes, tabEmplacementDes, *tourDeJeu, police, couleurNoire, texteResultatDe, texteVictoireDefaite,  texteNomJoueur, nbJoueur);//affiche la map
          }
        }
        return 0;
			}
			else //L'ia joue un coup interidt on stop tout
			{
				*sortie = 0;
        printf("L'IA A JOUE UN COUP INTERDIT : IA ID %d , ATTaque : %d , OWNER %d, OWNER IA %d, TOUR DE JEU %d\n", coups->cellFrom, coups->cellTo, g->cells[coups->cellTo].owner,g->cells[coups->cellFrom].owner, *tourDeJeu); //message de verif
        return 0;
			}
}

//Fonction gérant les actions a réalisé à la fin d'un tour
int finTour(SMap *g, unsigned int *tourDeJeu, Tableau t, SDL_Texture *tabTexture[NBTEXTURE], SDL_Renderer *render, SDL_Rect tabEmplacement[TAB_W][TAB_H],  int selection[TAB_W][TAB_H], int tailleTableauDes, SDL_Rect **tabEmplacementDes,  TTF_Font *police, SDL_Color couleurNoire, char texteResultatDe[100], char texteVictoireDefaite[20],  char **texteNomJoueur, int nbJoueur)
{
  //Affichage fin tour///
  sprintf(texteResultatDe, "Fin du tour de %s", texteNomJoueur[*tourDeJeu - 1]); //On affiche un texte pour dire que le tour est terminé
  strcpy(texteVictoireDefaite, "");
  deselectionCase(selection); //On déselectionne toutes les cases
  if(AVEC_AFFICHAGE)
  {
    SDL_RenderClear(render); //on peint le fond en blanc
    affichageMap(t, g, render, tabTexture, tabEmplacement, selection, tailleTableauDes, tabEmplacementDes, *tourDeJeu, police, couleurNoire, texteResultatDe, texteVictoireDefaite,  texteNomJoueur, nbJoueur);//affiche la map
  }
  if(DELAY_AFFICHAGE) SDL_Delay(200); //on atteint 0.1sec
  //////////
  ///////Calcul des dés à distribuer
  int nbDesDispo = g->stack[(*tourDeJeu)-1] +  maxTerritoiresContigus(g, *tourDeJeu); //Calcul du nombre de dés disponible qui comprend la pile + le max de territoire contigu qu'il possède
  int nbDesRestant = distributionDes(nbDesDispo, g, *tourDeJeu, t, tabTexture, render, tabEmplacement, selection, tailleTableauDes, tabEmplacementDes, police, couleurNoire, texteResultatDe, texteVictoireDefaite, texteNomJoueur, nbJoueur);
  if(nbDesRestant == -1) //si le joueur a appuyé sur la croix rouge pendant la distribution des dés
  {
    return 1;
  }
  g->stack[(*tourDeJeu)-1] = nbDesRestant;
  if(g->stack[(*tourDeJeu)-1] > 40) g->stack[(*tourDeJeu)-1] = 40; //On mets à jour la pile de dés, max 40 dés dans la pile
  ////Passage au tour suivant///////////////
  deselectionCase(selection); //on met à jour le graphisme
  strcpy(texteResultatDe, "");
  *tourDeJeu = (*tourDeJeu + 1) % (nbJoueur+1); //on incremente pour que ce soit au prochaine joueur de jouer
  if(*tourDeJeu == 0) *tourDeJeu = 1;
  while(maxTerritoiresContigus(g, *tourDeJeu) == 0) //Si le joueur n'a plus de territoires, on passe au joueur suivant
  {
    *tourDeJeu = (*tourDeJeu + 1) % (nbJoueur+1); //on incremente pour que ce soit au prochaine joueur de jouer
    if(*tourDeJeu == 0) *tourDeJeu = 1;
  }
  if(AVEC_AFFICHAGE)
  {
    SDL_RenderClear(render); //on peint le fond en blanc
    affichageMap(t, g, render, tabTexture, tabEmplacement, selection, tailleTableauDes, tabEmplacementDes, *tourDeJeu, police, couleurNoire, texteResultatDe, texteVictoireDefaite,  texteNomJoueur, nbJoueur);//affiche la map
  }
  if(interactionQuitte() == 1)
  {
    return -1;
  }
  return 0;
}

//Verifie si valeur est dans le tableau de taille longueurTableau
int estDansTableau(int valeur,int* tableau,int longueurTableau){
	for(int i=0; i < longueurTableau ; i++){
		if (tableau[i] == valeur){
			return i;
		}
	}
	return -1;
}


//Initialise les pointeurs des IA
int initialisationPointeursIA(PFonctionsIA *tabIA, int nbIA, void **tabLib)
{
  for(int i = 0; i < nbIA; i++)
  {
    if ((tabIA[i].InitGame = (pfInitGame)dlsym(tabLib[i], "InitGame")) == NULL)
    {
      return 1;
    }
    if ((tabIA[i].PlayTurn = (pfPlayTurn)dlsym(tabLib[i], "PlayTurn")) == NULL)
    {
      return 1;
    }
    if ((tabIA[i].EndGame = (pfEndGame)dlsym(tabLib[i], "EndGame")) == NULL)
    {
      return 1;
    }
  }
  return 0;
}

//Calcul les tours de jeux des IA et leur transmet avec Initgame
void initialisationIA(int nbIA, int *joueursIA, int nbJoueurs, void **tabLib, SPlayerInfo *infoIA, PFonctionsIA *tabIA, char **texteNomJoueur) //initialise les tours des ia et les textes des joueurs
{
  int tmp = 0;
  for(int i = 0; i < nbIA; i++)
  {
    joueursIA[i] = -1;
  }
  for(int i = 0; i < nbIA; i++) //choisis un tour aléatoirement
  {
    if(TOUR_ALEATOIRE)
    {
      do {
        tmp = rand()%(nbJoueurs) + 1;
      } while(estDans(tmp, joueursIA, nbIA));
      joueursIA[i] = tmp; //attribution des numéros de tour aux IA aléatoirement
    }
    else joueursIA[i] = i+1; //pas alétoire
  }
  initialisationPointeursIA(tabIA, nbIA, tabLib); //Chargement de tous les pointeurs vers les fonctions
  for(int i = 0; i < nbIA; i++)
  {
    tabIA[i].InitGame(joueursIA[i] - 1, nbJoueurs, infoIA);
    strcpy(texteNomJoueur[joueursIA[i] - 1], infoIA->name);
  }
  for(int i = 0; i < nbJoueurs; i++)
  {
    if(!(estDans(i+1, joueursIA, nbIA)))
    {
      char nomJoueur[50];
      sprintf(nomJoueur, "Joueur %d", i+1);
      strcpy(texteNomJoueur[i], nomJoueur);
    }
  }
}
