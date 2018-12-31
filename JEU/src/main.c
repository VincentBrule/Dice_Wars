#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "graphe.h"
#include "tableau.h"
#include "affichage.h"

int main(int argc, char **argv)
{
    srand(time(NULL)); //initialise le random
    ///////RECUPERATION DES PARAMETRES LORS DU LANCEMENT///////////
    if(argc < 3 || argc > 11) //Si nous avons moins de 3 arguments, il manque des arguments
    {
      printf("Il manque des arguments\n");
      return EXIT_FAILURE;
    }

    int nbPartie = atoi(argv[1]); //récupère le nb de parties voulu
    int nbJoueurs = atoi(argv[2]); //récupère le nb de joueur total, ia inclut
    int nbIA = argc - 3; //récupère le nb d'ia
    if(nbPartie == 0 || nbJoueurs == 0) //si la conversion n'a pas fonctionné, on renvoit une erreur car les arguments n'étaient pas des nombres
    {
      printf("Erreur lors de la saisie des arguments.\n");
      return EXIT_FAILURE;
    }

    if((nbJoueurs < nbIA) || (nbJoueurs < 2) || (nbJoueurs > 8)) //Verifie qu'il y a au moins Deux joueurs
    {
      printf("Arguments invalides.\n");
      return EXIT_FAILURE;
    }

    char **texteNomJoueur = tableauNomJoueur(nbJoueurs); //Tableau récupérant le nom des joueurs pour l'affichage
    printf("Nb partie = %d, NBJoueurs = %d, nbIA = %d\n", nbPartie, nbJoueurs, nbIA);
    //////////////////////////////////////////////////////////////////

    /////////////CHARGEMENT DES LIBRAIRIES DYNAMIQUES/////////////////
    void **tabLib = (void**)malloc(sizeof(void*) * nbIA); //tab récupérant les lib chargées

    char cheminIA[100];

    for(int i = 0; i < nbIA; i++)
    {
      strcpy(cheminIA, argv[3+i]); //on récupère le chemin vers l'ia

      if ((tabLib[i]=dlopen(cheminIA, RTLD_LAZY)) == NULL)
      {
       printf("Erreur: impossible d'ouvrir la librairie dynamique.\n");
       return EXIT_FAILURE;
      }
    }
    //////////////////////////////////////////////////////////////////

    //////////////////////INITIALISATION DES IA/////////////////////////////
    SPlayerInfo *infoIA = (SPlayerInfo*)malloc(sizeof(SPlayerInfo)); //Création de la structure SPlayer Info
    STurn *coups = (STurn*)malloc(sizeof(STurn)); //structure pour les coups
    int *joueursIA = calloc(nbIA, sizeof(int)); //Tableau récupérant les numéros de tours des IA
    PFonctionsIA *tabIA = (PFonctionsIA*)malloc(sizeof(PFonctionsIA) * nbIA); //tableau récupérant les pointeurs vers les librairies chargées pour chaque IA
    //attribution aléatoire des tours
    initialisationIA(nbIA, joueursIA, nbJoueurs, tabLib, infoIA, tabIA, texteNomJoueur);
    ///////////////////////////////////////////////////////////////////////



    //////////////////////////////INITIALISATION DU JEU////////////////////////////////////////
    ////Création de la map
    Tableau t = CreerTableau(); //génération du tableau d'hexagones
    int nbTerri = GenererTerritoire2(t, NB_SOMMET); //génération des territoires
    SMap *g = CreerMap(t, nbTerri, nbJoueurs); //graphe

    /////////////////////

    int selection[TAB_W][TAB_H] = {{0}}; //tableau des cases sélectionnées
    int tailleTableauDes = idMax(t); //Nombre de territoire
    Point *tabID = genererTableauAffichageDes(tailleTableauDes); //Genere le tableau avec l'emplacement des dés pour chaque territoire
    unsigned int tourDeJeu = 1; //Joueur qui doit jouer
    SDL_Color couleurNoire = {0, 0, 0}; //defini la couleur pour le texte
    TTF_Font *police = NULL; //pointeur vers la police ouverte par la suite
    char texteResultatDe[100];//Texte affichant le résultat de chaque attaque
    char texteVictoireDefaite[20]; //Texte affichant la prise ou non d'un territoire
    strcpy(texteResultatDe, "");
    strcpy(texteVictoireDefaite, "");
    FILE *pFile;
    if (!(pFile=fopen("./resultat.txt", "a+"))) //Ouverture du fichier pour afficher les résultats des parties
    {
      printf("Erreur: impossible d'ouvrir le fichier pour enregistrer les résultats.\n");
      return 1;
    }

  /////////////////////////////INITIALISATION SDL///////////////////////////////////////////
  if (SDL_Init(SDL_INIT_VIDEO) != 0 && IMG_Init(IMG_INIT_PNG) != 0) //initialise le module vidéo et image png
  {
      fprintf(stdout,"Échec de l'initialisation de la SDL (%s)\n",SDL_GetError());
      return 1;
  }
  if(TTF_Init() == -1)
  {
    fprintf(stdout,"Échec de l'initialisation de TTF (%s)\n",SDL_GetError());
    return 1;
  }
  if(!(police = TTF_OpenFont("./fonts/police"xstr(numPolice)".ttf", 25))) //charge la police d'ecriture
  {
    printf("Impossible de charger la police.\n");
    return EXIT_FAILURE;
  }
  // Création de la fenêtre
  SDL_Window* window = NULL;
  if((window = SDL_CreateWindow("DiceWars",SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, LARGEUR, HAUTEUR, SDL_WINDOW_SHOWN)) == NULL) //Nous creons la fenetre centrée sur lecran de 1400/900
  {
    fprintf(stdout,"Échec lors de la creation de la fenetre (%s)\n",SDL_GetError());
    return EXIT_FAILURE;
  }
  SDL_Renderer *render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); //renderer propre a la SDL2.0
  SDL_Texture* tabTexture[NBTEXTURE]; //recupere les textures
  SDL_Surface* tabSurfaceTexture[NBTEXTURE]; //recupere les surfaces des textures
  if(chargementSprites(tabTexture, render, tabSurfaceTexture)) //Charment de tous les sprites nécessaires à l'affichage
  {
    fprintf(stdout,"Échec lors du chargement des sprites (%s)\n",SDL_GetError());
    return EXIT_FAILURE;
  }


  SDL_Rect tabEmplacement[TAB_W][TAB_H]; //Tableau recuperant les emplacements pour le graphisme de la carte
  SDL_Rect** tabEmplacementDes = genererTableauEmplacementDes(tailleTableauDes); //Tableau récupérant l'emplacement des dés
  emplacementCasesEtDes(tabEmplacement, t, tabID, g, tailleTableauDes, tabTexture, tabEmplacementDes); //calcul emplacement pour les dés et les hexagones
  SDL_SetRenderDrawColor(render, 255, 255, 255, 255); //couleur blanche pour le fond

  if(AVEC_AFFICHAGE) //Si on a mis 1 dans le define, on garde l'affichage
  {
    SDL_RenderClear(render); //on peint le fond en blanc
    affichageMap(t, g, render, tabTexture, tabEmplacement, selection, tailleTableauDes, tabEmplacementDes, tourDeJeu, police, couleurNoire, texteResultatDe, texteVictoireDefaite,  texteNomJoueur, nbJoueurs);//affiche la map
    SDL_RenderPresent(render); // Affichage mis à jour
  }

  SMap *copieMap;
  int quit = 0, sortie = 1, fin = 0, position = -1; //quit = 1 si appuie sur la croix pour fermer le jeu, sortie = 0 si l'ia joue un coup interdit, fin = 0 si l'IA ou un joueur a gagné
  SDL_Event event;
  ///BOUCLE PRINCIPALE/////////////////////
  while(!quit && nbPartie > 0) //Tant que l'on ne ferme pas le jeu ou qu'il reste des partie, on continue
  {
    if(verifVictoire(g, tourDeJeu))
    {
      fin = tourDeJeu;
    }

    if(!fin)//si la partie n'est pas terminée
    {
      position = estDansTableau(tourDeJeu, joueursIA, nbIA);
  		if (position != -1) //c'est a une IA de jouer
  		{
        copieMap = copierMap(g, nbJoueurs);
  			while (tabIA[position].PlayTurn((tourDeJeu - 1), copieMap, coups) && sortie && (!quit) && (!fin)) //Tant que l'IA joue, qu'elle n'a pas fait un coup interdit, que le joueur n'a pas quitté et que l'IA n'a pas gagné
  			{
  				quit = gestionCompleteIA(tabEmplacement, t, g, tabTexture, render, selection, &tourDeJeu, texteResultatDe, texteVictoireDefaite, coups, tailleTableauDes, tabEmplacementDes, police, couleurNoire, texteNomJoueur, nbJoueurs, joueursIA, nbIA, &sortie, &fin);
          DetruireMap(copieMap);
          copieMap = copierMap(g, nbJoueurs);
        }
        DetruireMap(copieMap);
  			if(!quit && (!fin))//Si l'ia a finit de jouer, que le joueur n'a pas quitté et que personne a gagné
  			{
          sortie = 1;
  				quit = finTour(g, &tourDeJeu, t, tabTexture, render, tabEmplacement, selection, tailleTableauDes, tabEmplacementDes, police, couleurNoire, texteResultatDe, texteVictoireDefaite,  texteNomJoueur, nbJoueurs);
        }
        while (SDL_PollEvent(&event)); //on vide le buffer event
      }
  		else //joueur humain
  		{
  		  quit = gestionEvenement(event, tabEmplacement, t, g, tabTexture, render, selection, &tourDeJeu, texteResultatDe, texteVictoireDefaite, coups, tailleTableauDes, tabEmplacementDes, police, couleurNoire, texteNomJoueur, nbJoueurs, &fin);
  	  }
    }

    else //la partie est terminée
    {
      fprintf(pFile, "%s\t", texteNomJoueur[fin - 1]); //on écrit le résultat dans le fichier txt ouvert
      strcpy(texteResultatDe, ""); //on vide les textes pour la nouvelle partie
      strcpy(texteVictoireDefaite, "");
      sprintf(texteResultatDe, "Le joueur %s a gagne !\n", texteNomJoueur[fin - 1]); //Affichage du joueur gagnant

      SDL_RenderClear(render); //on peint le fond en blanc
      affichageMap(t, g, render, tabTexture, tabEmplacement, selection, tailleTableauDes, tabEmplacementDes, tourDeJeu, police, couleurNoire, texteResultatDe, texteVictoireDefaite,  texteNomJoueur, nbJoueurs);//affiche la map
      SDL_Delay(1000); //on atteint 1 sec

      //Free//
      DetruireMap(g); // Libération de la map précédente
      libererTableauDes(tailleTableauDes, tabEmplacementDes);//on detruit le tableau des emplcaments pour les dés car il va changer
      free(tabID); //idem
      LibererTableau(t);
      ///////////////////////////////

      //Génération d'une nouvelle partie
      t = CreerTableau(); //génération du tableau d'hexagones
      nbTerri = GenererTerritoire2(t, NB_SOMMET); //génération des territoires
      g = CreerMap(t, nbTerri, nbJoueurs); //graphe
      tailleTableauDes = idMax(t);
      tabEmplacementDes = genererTableauEmplacementDes(tailleTableauDes); //Tableau récupérant l'emplacement des dés
      tabID = genererTableauAffichageDes(tailleTableauDes); //Genere le tableau avec l'emplacement des dés pour chaque territoire
      emplacementCasesEtDes(tabEmplacement, t, tabID, g, tailleTableauDes, tabTexture, tabEmplacementDes); //calcul emplacement pour les dés et les hexagones
      strcpy(texteResultatDe, ""); //on vide les textes pour la nouvelle partie
      strcpy(texteVictoireDefaite, "");
      deselectionCase(selection);
      tourDeJeu = 1; //on remet le tour au premier joueur
      quit = 0, sortie = 1, fin = 0;
      nbPartie--; //on reduit le nombre de partie de 1
      printf("Il reste %d parties.\n", nbPartie);

      //Reintialisation des IA
      for(int i = 0; i < nbIA; i++) tabIA[i].EndGame(joueursIA[i] - 1, 1); //on appelle EngGame
      initialisationIA(nbIA, joueursIA, nbJoueurs, tabLib, infoIA, tabIA, texteNomJoueur);//on apelle initGame pour reintialiser les IA
    }
	}
  fclose(pFile);
  resultats(nbJoueurs); //Affiche dans un fichier résultats_finaux.txt les résultats globaux par joueur
  printf("Resultats par partie presents dans le fichier resultat.txt\nResultats par joueur presents dans le fichier resultats_finaux.txt\n");
  //////////////////////////////////////
  //Libération de la mémoire allouée
  for(int i = 0; i < nbIA; i++) tabIA[i].EndGame(joueursIA[i] - 1, 1);
  for(int i = 0; i < nbIA; i++) dlclose(tabLib[i]);
  libererTableauDes(tailleTableauDes, tabEmplacementDes); // libere la mémoire prise par le tableau dés
  liberationMemoire(tabTexture, tabSurfaceTexture, render, window);
  LibererTableau(t);
  libererTableauNom(texteNomJoueur, nbJoueurs);
  free(tabID);
  free(coups);
  free(tabIA);
  free(joueursIA);
  DetruireMap(g);
  free(infoIA);
  free(tabLib);
  TTF_CloseFont(police);
  IMG_Quit();
  TTF_Quit();
  SDL_Quit();

  return EXIT_SUCCESS;
}
