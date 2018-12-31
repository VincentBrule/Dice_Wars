#include "affichage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//Met dans le tableau emplacement, les positions des cases où il faudra blitz
//Prend en parametre un sprite pour obtenir sa taille qui est identique pour tous les sprites et le tableau qui va recevoir les positions et le tableau contenant les hexagones pour remplir les voisins des hexagones
void emplacementCasesEtDes(SDL_Rect tabEmplacement[TAB_W][TAB_H], Tableau t, Point *tabID,  SMap *g, int tailleTableauDes, SDL_Texture* tabTexture[NBTEXTURE], SDL_Rect **tabEmplacementDes)
{
  int v[6] = {0};
  int w = 0, h = 0;
  int wRef = 0, hRef = 0;
  SDL_QueryTexture(tabTexture[0],  NULL, NULL, &wRef,  &hRef); //recupère la taille de la surface
  for(int i = 0; i < TAB_W; i++) //largeur
  {
    for(int j = 0; j < TAB_H; j++) //hauteur
    {
      voisinsCase(t, i, j, v); //récupère les voisins
      tourDeDes(tabID, t, g, tailleTableauDes, i, j);//On calcule les positions pour les dés
      if(j%2 == 1) //Ligne impaire décalée de 18 pixels
      {
        SDL_Rect dest = {((18*i) + 9), (10*j)+DECALAGE, 18, 15};
        tabEmplacement[i][j] = dest;
      }
      else if(j%2 == 0) //Ligne paire
      {
        SDL_Rect dest = {(18*i), (10*j)+DECALAGE, 18, 15};
        tabEmplacement[i][j] = dest;
      }
    }
  }
  for(int b = 0; b <= tailleTableauDes; b++)
  {
    if(tabID[b].nbX != -2 && tabID[b].x > 0 && tabID[b].y > 0) //si ce n'est pas un terrain neutre
    {
      tabID[b].x = (tabID[b].x)/(tabID[b].nbX);
      tabID[b].y = (tabID[b].y)/(tabID[b].nbY);
      for(int c = 0; c < 4; c++) //stocke les emplacement dans le tabEmplacementDes
      {
        SDL_QueryTexture(tabTexture[c+9],  NULL, NULL, &w,  &h); //recupère la taille de la surface
        SDL_Rect temp = {tabID[b].x, tabID[b].y - h + DECALAGE + 3, w, h}; //Calcul enplacement pour les piles de dés
        SDL_Rect temp2 = {tabID[b].x-(w/1.5), tabID[b].y - h  + DECALAGE , w, h};
        tabEmplacementDes[b][c] = temp;
        tabEmplacementDes[b][c+4] = temp2;
      }
    }
    else
    {
      SDL_Rect neutre = {-10, -10, -10, -10}; //nous ne pouvons pas mettre NULL car sinon on perd le pointeur pour free par la suite
      for(int i = 0; i < 8; i++)
        tabEmplacementDes[b][i] =  neutre;//Si c'est un terrain neutre, on n'y place pas de dés
    }
  }
}

//Fonction gérant l'affichage de la map
//Paramètres : (1)Le tableau contenant les hexagones avec leur id et tableau de voisins, (2) le graphe SMap, (3) le renderer, (4) Le tableau contenant les textures
//(5) Le tableau contenant les positions pour blitz, (6) le tableau permettant de savoir les cases en cours de sélection pour les mettres en surbrillances
void affichageMap(Tableau t, SMap *g, SDL_Renderer *render, SDL_Texture* tabTexture[NBTEXTURE],  SDL_Rect tabEmplacement[TAB_W][TAB_H], int selection[TAB_W][TAB_H], int tailleTableauDes, SDL_Rect **tabEmplacementDes, unsigned int tourDeJeu, TTF_Font *police, SDL_Color couleurNoire, char texteResultatDe[100], char texteVictoireDefaite[20], char **texteNomJoueur, int nbJoueur)
{
  affichageHexagones(render, g, t, tabEmplacement, tabTexture, selection); //Affiche le "sol" du jeu
  affichageFrontieres(render, g, t); //Affiche les frontières entre les différents territoires
  affichageDes(tailleTableauDes, tabTexture,  render, g, tabEmplacementDes); //Affiche les piles de dés
  afficherTexteResultatDes(texteResultatDe,texteVictoireDefaite, police, couleurNoire, render); //Affiche le résultat d'un combat entre deux territoires
  afficherTexteTourDeJeu(police,couleurNoire, render, tourDeJeu, tabTexture, texteNomJoueur); //Affiche le tour de jeu
  afficherTexteDesPile(police, couleurNoire, render,tourDeJeu, g->stack); //Affiche les dés présents dans la pile
  afficherBoutonFinTour(tabTexture, render);//boutonFinTour
  afficherDesFinTour(tabTexture, render, nbJoueur, police, g, couleurNoire); //Affiche les dés que les joueurs pourront avoir à la fin de leur tour
  SDL_RenderPresent(render); // Affichage mis à jour
}

//Fonction gérant l'affichage des dés que les joueurs vont obtenir à la fin du tour (Partie droite de l'écran)
void afficherDesFinTour(SDL_Texture *tabTexture[NBTEXTURE], SDL_Renderer *render, int nb, TTF_Font *police, SMap *g, SDL_Color couleurNoire)
{
  char texte[20];
  int tailleTexte = 0;
  int w = 0, h = 0;
  SDL_QueryTexture(tabTexture[0],  NULL, NULL, &w,  &h); //recupère la taille de la surface
  int nbTerritoire = 0;
  int nbValide = 0;
  for(int i = 0; i < nb; i++)
  {
    nbTerritoire = maxTerritoiresContigus(g, (i+1)); //on calcul pour chaque joueur le max de territoire contigu
    if(nbTerritoire) //Permet de supprimer les joueurs qui n'ont plus de territoire
    {
      sprintf(texte, "x %d", nbTerritoire);
      tailleTexte = strlen(texte);
      SDL_Rect emplacementTexte = {LARGEUR - 70, (HAUTEUR/6)+(nbValide*75) - 10, tailleTexte*10, 50};
      SDL_Surface* texteSurface = TTF_RenderText_Solid(police, texte, couleurNoire); //Creation de la surface, avec la police la couleur noire et le texte mis en paramatre
      SDL_Texture* Message = SDL_CreateTextureFromSurface(render, texteSurface); //Creation de la texture avec le texte voulu
      SDL_Rect emplacementCase = {LARGEUR-120, (HAUTEUR/6)+(nbValide*75), h, w}; //tous les hexagones font la même taille
      SDL_RenderCopy(render, tabTexture[i], NULL, &emplacementCase); // Copie du sprite grâce au SDL_Renderer
      SDL_RenderCopy(render, Message, NULL, &emplacementTexte); // Copie du sprite grâce au SDL_Renderer
      SDL_DestroyTexture(Message);
      SDL_FreeSurface(texteSurface);
      nbValide++;
    }
  }
}

//Fonction qui affiche le résulat des combats
void afficherTexteResultatDes(char texte[100], char texteVictoireDefaite[20],TTF_Font *police, SDL_Color couleurNoire, SDL_Renderer *render)
{

  int tailleTexte = strlen(texte);
  SDL_Rect emplacementTexte = {(LARGEUR/2) - ((tailleTexte/2)*10), (8*HAUTEUR)/9, tailleTexte*10, 50};
  SDL_Surface* texteSurface = TTF_RenderText_Solid(police, texte, couleurNoire); //Creation de la surface, avec la police la couleur noire et le texte mis en paramatre
  SDL_Texture* Message = SDL_CreateTextureFromSurface(render, texteSurface); //Creation de la texture avec le texte voulu
  SDL_RenderCopy(render, Message, NULL, &emplacementTexte); // Copie du sprite grâce au SDL_Renderer
  int tailleTexte2 = strlen(texteVictoireDefaite);
  SDL_Rect emplacementTexte2 = {(LARGEUR/2) - ((tailleTexte2/2)*10), (14*HAUTEUR)/15, tailleTexte2*10, 50};
  SDL_Surface* texteSurface2 = TTF_RenderText_Solid(police, texteVictoireDefaite, couleurNoire); //Creation de la surface, avec la police la couleur noire et le texte mis en paramatre
  SDL_Texture* Message2 = SDL_CreateTextureFromSurface(render, texteSurface2); //Creation de la texture avec le texte voulu
  SDL_RenderCopy(render, Message2, NULL, &emplacementTexte2); // Copie du sprite grâce au SDL_Renderer

  SDL_DestroyTexture(Message);
  SDL_FreeSurface(texteSurface);
  SDL_DestroyTexture(Message2);
  SDL_FreeSurface(texteSurface2);
}

//fFonction affichant les dés présents dans la pile du joueur
void afficherTexteDesPile(TTF_Font *police, SDL_Color couleurNoire, SDL_Renderer *render, unsigned int tourDeJeu, unsigned int *stack)
{
  char resultat[50];
  sprintf(resultat, "Des dans votre pile : %d", stack[tourDeJeu - 1]);
  int tailleTexte = strlen(resultat);
  SDL_Rect emplacementTexte = {5, (14*HAUTEUR)/15, tailleTexte*10, 50};
  SDL_Surface* texteSurface = TTF_RenderText_Solid(police, resultat, couleurNoire); //Creation de la surface, avec la police la couleur noire et le texte mis en paramatre
  SDL_Texture* Message = SDL_CreateTextureFromSurface(render, texteSurface); //Creation de la texture avec le texte voulu
  SDL_RenderCopy(render, Message, NULL, &emplacementTexte); // Copie du sprite grâce au SDL_Renderer
  SDL_DestroyTexture(Message);
  SDL_FreeSurface(texteSurface);
}

//Fonction affichant le tour du joueur actuel
void afficherTexteTourDeJeu(TTF_Font *police, SDL_Color couleurNoire, SDL_Renderer *render, unsigned int tourDeJeu, SDL_Texture* tabTexture[NBTEXTURE], char **texteNomJoueur)
{
  char resultat[50];
  int w = 0, h = 0;
  sprintf(resultat, "Tour de %s : ", texteNomJoueur[tourDeJeu - 1]);
  int tailleTexte = strlen(resultat);
  SDL_Rect emplacementTexte = {5, (8*HAUTEUR)/9, tailleTexte*10, 50};
  SDL_Surface* texteSurface = TTF_RenderText_Solid(police, resultat, couleurNoire); //Creation de la surface, avec la police la couleur noire et le texte mis en paramatre
  SDL_Texture* Message = SDL_CreateTextureFromSurface(render, texteSurface); //Creation de la texture avec le texte voulu
  SDL_RenderCopy(render, Message, NULL, &emplacementTexte); // Copie du sprite grâce au SDL_Renderer

  SDL_QueryTexture(tabTexture[tourDeJeu - 1],  NULL, NULL, &w,  &h); //recupère la taille de la surface
  SDL_Rect emplacementCouleurJoueur = {5 + tailleTexte*10, (9*HAUTEUR)/10, w, h};
  SDL_RenderCopy(render, tabTexture[tourDeJeu-1], NULL, &emplacementCouleurJoueur); // Copie du sprite grâce au SDL_Renderer

  SDL_DestroyTexture(Message);
  SDL_FreeSurface(texteSurface);

}

//Fonction affichant le bouton fin Tour
void afficherBoutonFinTour(SDL_Texture *tabTexture[NBTEXTURE], SDL_Renderer *render)
{
  int w = 0, h = 0;
  SDL_QueryTexture(tabTexture[13],  NULL, NULL, &w,  &h);
  SDL_Rect boutonFinTour = {(4*LARGEUR)/5, (8*HAUTEUR)/9, w, h};
  SDL_RenderCopy(render, tabTexture[13], NULL, &boutonFinTour);
}

//Fonction affichant les hexagones
void affichageHexagones(SDL_Renderer *render, SMap *g, Tableau t, SDL_Rect tabEmplacement[TAB_W][TAB_H], SDL_Texture *tabTexture[NBTEXTURE], int selection[TAB_W][TAB_H])
{
  for(int i = 0; i < TAB_W; i++)
  {
    for(int j = 0; j < TAB_H; j++)
    {
      //On associe une couleur à chaque propriétaire
      if(g->cells[t[i][j].id].owner != 0) SDL_RenderCopy(render, tabTexture[g->cells[t[i][j].id].owner - 1], NULL, &(tabEmplacement[i][j])); // Copie du sprite grâce au SDL_Renderer
      if(selection[i][j] == 1) SDL_RenderCopy(render, tabTexture[8], NULL, &(tabEmplacement[i][j])); //si la case est sélectionnée, on lui applique une surbrillance
      }
  }
}

//Fonction affichant les frontières
void affichageFrontieres(SDL_Renderer *render, SMap *g, Tableau t)
{
  SDL_SetRenderDrawColor(render, 0, 0, 0, 255); //Couleur des lignes délimitants les territoires
  int offset = 0; //décalage des lignes impaires
  for(int j = 0; j < TAB_H; j++) //On parcourt le tableau de gauche à droite pour ne pas faire l'offset à chaque tour
  {
    if(j%2==0) offset = 0; //Si la ligne est paire, il n'y a pas de décalage, sinon on décale de 19 pixels
    else offset = 9;
    for(int i = 0; i < TAB_W; i++)
    {
      if(g->cells[t[i][j].id].owner > 0) //0 equivaut à un territoire neutre
      {
        for(int h = 0; h < 6; h++) //On parcours les 6 voisins de l'hexagone : on commence par en haut à gauche et on tourne dans le sens horaire
        {
          if((t[i][j]).id != (t[i][j]).voisins[h])
          {
            switch(h)
            {
              case(0):
              {
                SDL_RenderDrawLine(render, 9+(i*18)+offset , (j*10)+DECALAGE , (i*18)+offset , 5+(j*10)+DECALAGE); //en haut a gauche
                break;
              }
              case(1):
              {
                SDL_RenderDrawLine(render, 9+(i*18)+offset , (j*10)+DECALAGE , 18+(i*18)+offset , 5+(j*10)+DECALAGE); //en haut a droite
                break;
              }
              case(2):
              {
                SDL_RenderDrawLine(render, 18+(i*18)+offset , 5+(j*10)+DECALAGE , 18+(i*18)+offset , 10+(j*10)+DECALAGE); //droite milieu
                break;
              }
              case(3):
              {

                SDL_RenderDrawLine(render, 18+(i*18)+offset , 10+(j*10)+DECALAGE , 8+(i*18)+offset , 15+(j*10)+DECALAGE); //en bas à droite
                break;
              }
              case(4):
              {
                SDL_RenderDrawLine(render, (i*18)+offset , 10+(j*10)+DECALAGE , 9+(i*18)+offset, 15+(j*10)+DECALAGE); //en bas à gauche
                break;

              }
              case(5):
              {
                SDL_RenderDrawLine(render, (i*18)+offset , 5+(j*10)+DECALAGE , (i*18)+offset , 10+(j*10)+DECALAGE); //gauche milieu
                break;
              }
            }
          }
        }
      }
    }
  }
  SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
}

//Fonction affichant les dés sur chaque territoire grâce aux emplacements calculés ci-dessus
void affichageDes(int tailleTableauDes, SDL_Texture *tabTexture[NBTEXTURE],  SDL_Renderer *render,  SMap *g, SDL_Rect **tabEmplacementDes)
{
  for(int p = 0; p < (tailleTableauDes+1); p++) //Boucle pour afficher les tours de dés
  {
    if(tabEmplacementDes[p][0].x != (-10)) //Si ce n'est pas un terrain neutre
    {
      if(g->cells[p].nbDices == 1)
      {
        SDL_RenderCopy(render, tabTexture[9], NULL, &tabEmplacementDes[p][0]);
      }
      else if(g->cells[p].nbDices == 2)
      {
        SDL_RenderCopy(render, tabTexture[10], NULL, &tabEmplacementDes[p][1]);
      }
      else if(g->cells[p].nbDices == 3)
      {
        SDL_RenderCopy(render, tabTexture[11], NULL, &tabEmplacementDes[p][2]);
      }
      else if(g->cells[p].nbDices == 4)
      {
        SDL_RenderCopy(render, tabTexture[12], NULL, &tabEmplacementDes[p][3]);
      }
      else if(g->cells[p].nbDices == 5)
      {
        SDL_RenderCopy(render, tabTexture[9], NULL, &tabEmplacementDes[p][4]);
        SDL_RenderCopy(render, tabTexture[12], NULL, &tabEmplacementDes[p][3]);
      }
      else if(g->cells[p].nbDices == 6)
      {
        SDL_RenderCopy(render, tabTexture[10], NULL, &tabEmplacementDes[p][5]);
        SDL_RenderCopy(render, tabTexture[12], NULL, &tabEmplacementDes[p][3]);
      }
      else if(g->cells[p].nbDices == 7)
      {
        SDL_RenderCopy(render, tabTexture[11], NULL, &tabEmplacementDes[p][6]);
        SDL_RenderCopy(render, tabTexture[12], NULL, &tabEmplacementDes[p][3]);
      }
      else if(g->cells[p].nbDices == 8)
      {
        SDL_RenderCopy(render, tabTexture[12], NULL, &tabEmplacementDes[p][7]);
        SDL_RenderCopy(render, tabTexture[12], NULL, &tabEmplacementDes[p][3]);
      }
    }
  }
}

//Fonction chargeant les sprites en mémoires
//Prend en paramètre le tableau de texture à remplir, le renderer et le tableau de surface à remplir
//renvoit un int = 0 s'il n'y a pas eu de soucis
int chargementSprites(SDL_Texture* tabTexture[NBTEXTURE], SDL_Renderer *render, SDL_Surface* tabSurfaceTexture[NBTEXTURE])
{
  //Chargement des sprites
  SDL_Surface* blue = NULL;
  SDL_Surface* yellow = NULL;
  SDL_Surface* green = NULL;
  SDL_Surface* orange = NULL;
  SDL_Surface* purple = NULL;
  SDL_Surface* red = NULL;
  SDL_Surface* bleu_fonce = NULL;
  SDL_Surface* autre = NULL;
  SDL_Surface* transpa = NULL;
  SDL_Surface* de_1 = NULL;
  SDL_Surface* de_2 = NULL;
  SDL_Surface* de_3 = NULL;
  SDL_Surface* de_4 = NULL;
  SDL_Surface* boutonFin = NULL;

  if((blue = IMG_Load(xstr(cheminSpriteCases)"case_blue.png")) == NULL)//on charge le sprite en mémoire
  {
    fprintf(stdout,"Échec lors du chargement du sprite(%s)\n",SDL_GetError());
    return 1;
  }
  if((yellow = IMG_Load(xstr(cheminSpriteCases)"case_yellow.png")) == NULL)//on charge le sprite en mémoire
  {
    fprintf(stdout,"Échec lors du chargement du sprite(%s)\n",SDL_GetError());
    return 1;
  }
  if((green = IMG_Load(xstr(cheminSpriteCases)"case_green.png")) == NULL)//on charge le sprite en mémoire
  {
    fprintf(stdout,"Échec lors du chargement du sprite(%s)\n",SDL_GetError());
    return 1;
  }
  if((orange = IMG_Load(xstr(cheminSpriteCases)"case_orange.png")) == NULL)//on charge le sprite en mémoire
  {
    fprintf(stdout,"Échec lors du chargement du sprite(%s)\n",SDL_GetError());
    return 1;
  }
  if((purple = IMG_Load(xstr(cheminSpriteCases)"case_purple.png")) == NULL)//on charge le sprite en mémoire
  {
    fprintf(stdout,"Échec lors du chargement du sprite(%s)\n",SDL_GetError());
    return 1;
  }
  if((red = IMG_Load(xstr(cheminSpriteCases)"case_red.png")) == NULL)//on charge le sprite en mémoire
  {
    fprintf(stdout,"Échec lors du chargement du sprite(%s)\n",SDL_GetError());
    return 1;
  }
  if((bleu_fonce = IMG_Load(xstr(cheminSpriteCases)"case_pink.png")) == NULL)//on charge le sprite en mémoire
  {
    fprintf(stdout,"Échec lors du chargement du sprite(%s)\n",SDL_GetError());
    return 1;
  }
  if((autre = IMG_Load(xstr(cheminSpriteCases)"case_sky.png")) == NULL)//on charge le sprite en mémoire
  {
    fprintf(stdout,"Échec lors du chargement du sprite(%s)\n",SDL_GetError());
    return 1;
  }
  if((transpa = IMG_Load(xstr(cheminSpriteCases)"brighter.png")) == NULL)//on charge le sprite en mémoire
  {
    fprintf(stdout,"Échec lors du chargement du sprite(%s)\n",SDL_GetError());
    return 1;
  }
  if((de_1 = IMG_Load(xstr(cheminSpriteDes)"dice_1.png")) == NULL)//on charge le sprite en mémoire
  {
    fprintf(stdout,"Échec lors du chargement du sprite(%s)\n",SDL_GetError());
    return 1;
  }
  if((de_2 = IMG_Load(xstr(cheminSpriteDes)"dice_2.png")) == NULL)//on charge le sprite en mémoire
  {
    fprintf(stdout,"Échec lors du chargement du sprite(%s)\n",SDL_GetError());
    return 1;
  }
  if((de_3 = IMG_Load(xstr(cheminSpriteDes)"dice_3.png")) == NULL)//on charge le sprite en mémoire
  {
    fprintf(stdout,"Échec lors du chargement du sprite(%s)\n",SDL_GetError());
    return 1;
  }
  if((de_4 = IMG_Load(xstr(cheminSpriteDes)"dice_4.png")) == NULL)//on charge le sprite en mémoire
  {
    fprintf(stdout,"Échec lors du chargement du sprite(%s)\n",SDL_GetError());
    return 1;
  }
  if((boutonFin = IMG_Load("./sprites/finTour/boutonFinTour.png")) == NULL)//on charge le sprite en mémoire
  {
    fprintf(stdout,"Échec lors du chargement du sprite(%s)\n",SDL_GetError());
    return 1;
  }
  //On remplit le tableau pour faciliter la destruction des surfaces à la fin du programme
  tabSurfaceTexture[0] = blue;
  tabSurfaceTexture[1] = yellow;
  tabSurfaceTexture[2] = green;
  tabSurfaceTexture[3] = orange;
  tabSurfaceTexture[4] = purple;
  tabSurfaceTexture[5] = red;
  tabSurfaceTexture[6] = bleu_fonce;
  tabSurfaceTexture[7] = autre;
  tabSurfaceTexture[8] = transpa;
  tabSurfaceTexture[9] = de_1;
  tabSurfaceTexture[10] = de_2;
  tabSurfaceTexture[11] = de_3;
  tabSurfaceTexture[12] = de_4;
  tabSurfaceTexture[13] = boutonFin;

  //On crée la struture associé au sprite associé en mémoire
  SDL_Texture* caseBleue = SDL_CreateTextureFromSurface(render , blue); // Préparation des sprite
  SDL_Texture* caseJaune = SDL_CreateTextureFromSurface(render , yellow);
  SDL_Texture* caseVerte = SDL_CreateTextureFromSurface(render , green);
  SDL_Texture* caseOrange = SDL_CreateTextureFromSurface(render , orange);
  SDL_Texture* caseViolette = SDL_CreateTextureFromSurface(render , purple);
  SDL_Texture* caseRouge = SDL_CreateTextureFromSurface(render , red);
  SDL_Texture* caseBleuFonce = SDL_CreateTextureFromSurface(render , bleu_fonce);
  SDL_Texture* caseAutre = SDL_CreateTextureFromSurface(render , autre);
  SDL_Texture* caseSelection = SDL_CreateTextureFromSurface(render , transpa);
  SDL_Texture* dex1 = SDL_CreateTextureFromSurface(render , de_1);
  SDL_Texture* dex2 = SDL_CreateTextureFromSurface(render , de_2);
  SDL_Texture* dex3 = SDL_CreateTextureFromSurface(render , de_3);
  SDL_Texture* dex4 = SDL_CreateTextureFromSurface(render , de_4);
  SDL_Texture* boutonF = SDL_CreateTextureFromSurface(render , boutonFin);

  //On stocke les textures dans un tableau pour pouvoir les reutiliser par la suite
  tabTexture[0] = caseBleue;
  tabTexture[1] = caseJaune;
  tabTexture[2] = caseVerte;
  tabTexture[3] = caseOrange;
  tabTexture[4] = caseViolette;
  tabTexture[5] = caseRouge;
  tabTexture[6] = caseBleuFonce;
  tabTexture[7] = caseAutre;
  tabTexture[8] = caseSelection;
  tabTexture[9] = dex1;
  tabTexture[10] = dex2;
  tabTexture[11] = dex3;
  tabTexture[12] = dex4;
  tabTexture[13] = boutonF;
  return 0;
}

//Fonction libérant la mémoire allouée
void liberationMemoire(SDL_Texture* tabTexture[NBTEXTURE], SDL_Surface* tabSurfaceTexture[NBTEXTURE], SDL_Renderer *render, SDL_Window* window)
{
  //Liberation de la memoire
  for(int i = 0; i < NBTEXTURE; i++)
  {
    SDL_DestroyTexture(tabTexture[i]);
    SDL_FreeSurface(tabSurfaceTexture[i]);
  }
  SDL_DestroyRenderer(render);
  SDL_DestroyWindow(window);
}

//Verifie si le clic est sur le bouton fin tour
int verifFinTour(int coordX, int coordY)
{
  if(coordX >= (4*LARGEUR)/5 && coordX <= ((4*LARGEUR)/5)+150 && coordY >= (8*HAUTEUR)/9 && coordY <= ((8*HAUTEUR)/9)+75) return 0;
  else return 1;
}

//Permet de savoir dans quel hexagone se situe le clic
//Prend en paramètre les coordonnées du clic, les emplacements des sprites, et l'emplacement de l'hexagone qui sera remplit dans les cases pointees par emplacementX et Y
int verificationCoordonnees(int coordX, int coordY, SDL_Rect tabEmplacement[TAB_W][TAB_H], int *emplacementX, int *emplacementY)
{
  int l = 0;
  int h = 0;
  while(h < TAB_H) //boucle while car on va la quitter en cours si on trouve la case correspondante
    {
      while(l < TAB_W)
      {
        //On verifie que l'on est bien dans le sprite
        if((coordX  >= tabEmplacement[l][h].x) && (coordY >= tabEmplacement[l][h].y) && (coordX  <= (tabEmplacement[l][h].x + tabEmplacement[l][h].w)) && (coordY  <= (tabEmplacement[l][h].y + tabEmplacement[l][h].h)) )
        {
          //On verifie que le clic est à l'interieur de l'hexagone et pas seulement dans celui du sprite
          if((coordY > (tabEmplacement[l][h].y + 9)) && (coordY < (tabEmplacement[l][h].y + 22))) //Equation des droites des hexagones
          {
            *emplacementX = l;
            *emplacementY = h;
            return 1;
          }
          else if( (coordX < (tabEmplacement[l][h].x + 21)) && (coordY < (tabEmplacement[l][h].y + 10)))
          {
            if((((-10.0/17.0)*(float)(coordX - tabEmplacement[l][h].x)) + 10.0 - (coordY - tabEmplacement[l][h].y)) <= 0) //cadrant en haut à gauche : equation de la droite limite
            {
              *emplacementX = l;
              *emplacementY = h;
              return 1;
            }
          }
          else if( (coordX > (tabEmplacement[l][h].x + 21)) && (coordY < (tabEmplacement[l][h].y + 10)))
          {
            if((((10.0/17.0)*(float)(coordX - tabEmplacement[l][h].x)) -(210.0/17.0) - (coordY - tabEmplacement[l][h].y)) <= 0) //cadrant en haut à droite : equation de la droite limite
            {
              *emplacementX = l;
              *emplacementY = h;
              return 1;
            }
          }
          else if( (coordX < (tabEmplacement[l][h].x + 21)) && (coordY > (tabEmplacement[l][h].y + 21)))
          {
            if((((9.0/17.0)*(float)(coordX - tabEmplacement[l][h].x)) + 22 - (coordY - tabEmplacement[l][h].y)) >= 0) //cadrant en bas à gauche : equation de la droite limite
            {
              *emplacementX = l;
              *emplacementY = h;
              return 1;
            }
          }
          else if( (coordX > (tabEmplacement[l][h].x + 21)) && (coordY > (tabEmplacement[l][h].y + 21)))
          {
            if((((-9.0/16.0)*(float)(coordX - tabEmplacement[l][h].x)) + (347.0/8.0) - (coordY - tabEmplacement[l][h].y)) >= 0) //cadrant en bas à droite : equation de la droite limite
            {
              *emplacementX = l;
              *emplacementY = h;
              return 1;
            }
          }
        }
        l++;
      }
      h++;
      l = 0;
    }
    return 0;
}

//Fonction gardant actif la croix pendant toutes l'éxecution du programme
int interactionQuitte()
{
  SDL_Event event;
  while (SDL_PollEvent(&event)) // Récupération des actions de l'utilisateur
  {
        switch(event.type)
        {
            case SDL_QUIT: // Clic sur la croix
                return 1;
                break;
        }
  }
  return 0;
}

//Fonction gérant l'interaction entre l'utilisateur et les territoires
void interactionTerritoire(int idEnCours, unsigned int *tourDeJeu, SMap *g, Tableau t, STurn *coups, int selection[TAB_W][TAB_H],  char texteResultatDe[100], char texteVictoireDefaite[20], int *fin, int *selectionJoueur, char **texteNomJoueur)
{
  if(g->cells[idEnCours].owner == (*tourDeJeu)) //si on clique dans un de ses terrains
  {
    if(g->cells[idEnCours].nbDices > 1)
    {
      selectionCase(idEnCours, t, selection, 1);
      coups->cellFrom = idEnCours;
      (*selectionJoueur) = 1;
    }
  }
  else if(((*selectionJoueur) == 1) && (g->cells[idEnCours].owner > 0)) //Si on attaque quelqu'un et que l'un de nos terrains est sélectionné
  {
    if(verifAttaque(g, coups->cellFrom, idEnCours, t, *tourDeJeu))
    {
      (*selectionJoueur) = 0;
      coups->cellTo = idEnCours;
      attaque(coups, texteResultatDe, texteVictoireDefaite, g);
      deselectionCase(selection);
      if(verifVictoire(g, (g->cells[coups->cellTo]).owner)) //la partie est finie
      {
        (*fin) = (g->cells[coups->cellTo].owner);
      }
    }
  }
}

//Fonction remettant le tableau de selection a 0
void deselectionCase(int selection[TAB_W][TAB_H])
{
  for(int a = 0; a < TAB_W; a++)
  {
    for(int b = 0; b < TAB_H; b++)
    {
      selection[a][b] = 0;
    }
  }
}

//Fonction gérant la sélection des cases
//Prend en paramètre l'id des cases à sélectionner, le tableau d'hexagone et le tableau où l'on indique les cases sélectionnées
void selectionCase(int id, Tableau t, int selection[TAB_W][TAB_H], int remiseAZero)
{
  for(int a = 0; a < TAB_W; a++)
  {
    for(int b = 0; b < TAB_H; b++)
    {
      if(remiseAZero) selection[a][b] = 0;
      if(t[a][b].id == id)
      {
        selection[a][b] = 1; //la case appartient au même territoire donc on la sélectionne
      }
    }
  }
}


//Fonction renvoyant l'id max dans tableau t
int idMax(Tableau t)
{
  int idMax = 0;
  for(int i = 0; i < TAB_W; i++)
  {
    for(int j = 0; j < TAB_H; j++)
    {
      if(t[i][j].id > idMax) idMax = t[i][j].id;
    }
  }
  return idMax;
}

//Fonction calculant l'emplacement pour les tours de dés
void tourDeDes(Point *tabID, Tableau t, SMap *g, int tailleTableauDes, int i, int j)
{
    if(g->cells[t[i][j].id].owner > 0)
    {

      tabID[t[i][j].id].y += ((j)*10) + 8;
      (tabID[t[i][j].id].nbY)++;
      if(j%2 == 0)
      {
        tabID[t[i][j].id].x += 9 + (i*18);
      }
      else
      {
        tabID[t[i][j].id].x += 18 + (i*18);
      }
      (tabID[t[i][j].id].nbX)++;
  }
  else
  {
    tabID[t[i][j].id].nbX = -2;
  }
}

//Fonctions d'allocation
SDL_Rect** genererTableauEmplacementDes(int tailleTableauDes)
{
  SDL_Rect **tabEmplacementDes = (SDL_Rect**) malloc(sizeof(SDL_Rect*) * (tailleTableauDes+1)); //emplacement des dés
  for(int i = 0; i <= tailleTableauDes; i++)
  {
    tabEmplacementDes[i] = (SDL_Rect*)malloc(sizeof(SDL_Rect) * 8);
  }
  return tabEmplacementDes;
}

Point* genererTableauAffichageDes(int id)
{
  Point *tabID = (Point*)malloc(sizeof(Point) * (id+1));
  for(int i = 0; i <= id; i++)
  {
    tabID[i].x = 0;
    tabID[i].y = 0;
    tabID[i].nbX = 0;
    tabID[i].nbY = 0;
  }
  return tabID;
}

char** tableauNomJoueur(int nb)
{
  char **texteNomJoueur = (char**)malloc(sizeof(char*) * nb);
  for(int i = 0; i < nb; i++)
  {
    texteNomJoueur[i] = (char*)malloc(sizeof(char) * 60);
  }
  return texteNomJoueur;
}
//

//Fonctions libérant les ressources
void libererTableauDes(int tailleTableauDes, SDL_Rect **tabEmplacementDes)
{
  for(int i = 0; i <= tailleTableauDes; i++)
  {
    free(tabEmplacementDes[i]);
  }
  free(tabEmplacementDes);
}

void libererTableauNom(char **t, int nb)
{
  for(int i = 0; i < nb; i++)
  {
    free(t[i]);
  }
  free(t);
}
