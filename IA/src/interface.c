#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"

#define IA_NAME "PC_VersuS_V4"

/*
	#
	#	Fonctions&Structures des cibles = interne a l'IA
	#
*/

static int idCurr = 0;

static int nbIA = 0;

static int numberOfPlayers = 0;

static int idEnd = 0;

static int ownerMax = 0;

static int ownerMin = 0;

static int firstFight = 1;

typedef struct{
	int id;
}SContext;

SContext MemContext[8];
int tablDicesPerPlayer[8];

typedef struct {
	SCell* possibleTargets[20];
	int possibleTargetsWeight[20];
	int index;

	SCell* bestTarget;
	int indexBestTarget;

} STargets;

STargets* InitSTargets(){
	STargets* targets = (STargets*) malloc(sizeof(STargets));
	targets->index = 0;
	targets->bestTarget = NULL;
	targets->indexBestTarget = 0;
	return targets;
}

void AddTarget(STargets* targets, SCell* pCell,int nbDicesCell,int favoriteTarget){
	if (targets->bestTarget == NULL){
		targets->bestTarget = (targets->possibleTargets[targets->index] = pCell);
		targets->indexBestTarget = 0;
		targets->index ++;
	}else{
		targets->possibleTargets[targets->index] = pCell;
		targets->possibleTargetsWeight[targets->index] = targets->possibleTargetsWeight[targets->indexBestTarget];
		if (nbDicesCell>=targets->bestTarget->nbDices){
			targets->possibleTargetsWeight[targets->index] += 1;
		}
		if (pCell->nbDices == favoriteTarget){
			targets->possibleTargetsWeight[targets->index] += 1;
		}
		if (targets->possibleTargetsWeight[targets->index] > targets->possibleTargetsWeight[targets->indexBestTarget]){
			targets->bestTarget = pCell;
			targets->indexBestTarget = targets->index;
		}
		targets->index ++;
	}

}

void DeleteTargets(STargets* targets){
	free(targets);
}

SCell* GetBestTarget(STargets* targets){
	return targets->bestTarget;
}
/*
	#
	#	FIN Fonctions&Structures des cibles = interne a l'IA
	#
*/

/*
	#
	#	Fonctions&Structures d'ordonnement dans cellules
	#
*/
typedef struct {
	SCell** tablOrderedCells;
	int lentgh;
} SOrderedCellsUnit;


SCell** OrderCells(const SMap *map, int *size){
	//Pour l'instant les cellules ne sont pas triees mais stockees dans un tableau de cellules hors de la structure SMap
	//size = nombre de cells de l'IA => à remplir
	SCell** OrderedCells = (SCell**) malloc((map->nbCells)*sizeof(SCell*));
	int j =0;

	SCell* tablOrderedCells2Dice[map->nbCells];
	int lentgh2Dice = 0;
	SCell* tablOrderedCells3Dice[map->nbCells];
	int lentgh3Dice = 0;
	SCell* tablOrderedCells4Dice[map->nbCells];
	int lentgh4Dice = 0;
	SCell* tablOrderedCells5Dice[map->nbCells];
	int lentgh5Dice = 0;
	SCell* tablOrderedCells6Dice[map->nbCells];
	int lentgh6Dice = 0;
	SCell* tablOrderedCells7Dice[map->nbCells];
	int lentgh7Dice = 0;
	SCell* tablOrderedCells8Dice[map->nbCells];
	int lentgh8Dice = 0;


	for (int i=0;i<(map->nbCells);i++){
		//Pour chaque cellule du jeu
		if (map->cells[i].owner == MemContext[idCurr].id && map->cells[i].nbDices > 1){
			//Si la cellule est a l'IA et elle peut jouer
			switch (map->cells[i].nbDices){

				case 2 :
					tablOrderedCells2Dice[lentgh2Dice++] = &(map->cells[i]);
				break;

				case 3 :
					tablOrderedCells3Dice[lentgh3Dice++] = &(map->cells[i]);
				break;

				case 4 :
					tablOrderedCells4Dice[lentgh4Dice++] = &(map->cells[i]);
				break;

				case 5 :
					tablOrderedCells5Dice[lentgh5Dice++] = &(map->cells[i]);
				break;

				case 6 :
					tablOrderedCells6Dice[lentgh6Dice++] = &(map->cells[i]);
				break;

				case 7 :
					tablOrderedCells7Dice[lentgh7Dice++] = &(map->cells[i]);
				break;

				case 8 :
					tablOrderedCells8Dice[lentgh8Dice++] = &(map->cells[i]);
				break;

			}
		}
	}

	for (int i=0;i<lentgh8Dice;i++){
		OrderedCells[j++] = tablOrderedCells8Dice[i];
	}
	for (int i=0;i<lentgh7Dice;i++){
		OrderedCells[j++] = tablOrderedCells7Dice[i];
	}
	for (int i=0;i<lentgh6Dice;i++){
		OrderedCells[j++] = tablOrderedCells6Dice[i];
	}
	for (int i=0;i<lentgh5Dice;i++){
		OrderedCells[j++] = tablOrderedCells5Dice[i];
	}
	for (int i=0;i<lentgh4Dice;i++){
		OrderedCells[j++] = tablOrderedCells4Dice[i];
	}
	for (int i=0;i<lentgh3Dice;i++){
		OrderedCells[j++] = tablOrderedCells3Dice[i];
	}
	for (int i=0;i<lentgh2Dice;i++){
		OrderedCells[j++] = tablOrderedCells2Dice[i];
	}
	*size = j;
	return OrderedCells;
}

void FreeOrderedTargets(SCell** OrderedCells){
	free(OrderedCells);
}

/*
	#
	#	FIN Fonctions&Structures d'ordonnement dans cellules
	#
*/


int GetContiguousTerritories(const SMap *map, int idJoueur);
int getCurrentID(int id);

//Calcul le nombre de dé par joueur
void MajTablDicesPerPlayer(const SMap *map){
	int j = 0;
	for (int i=ownerMin;i<= ownerMax;i++){
		if(ownerMin != 0) j = i - ownerMin;
		tablDicesPerPlayer[j] = GetContiguousTerritories(map,i);
	}
}


int isIn(int i , int* t, int size){
	for (int j=0; j<size; ++j){
		if (t[j] == i) return 1;
	}
	return 0;
}

//Calcul le nombre de dé environnant
int GetAverageNumberDiceAround(SCell* cell){
	int average = 0;

	for(int i=0;i<(cell->nbNeighbors);i++){
		if (cell->neighbors[i]->owner != cell->owner){
			average += cell->neighbors[i]->nbDices;
		}
	}

	if(average == 0 || cell->nbNeighbors < 1)
		return 0;

	else
		return (average/(cell->nbNeighbors));
}

int GetNbContiguousTerritories(const SMap *map, int id){
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

      if(!isIn(voisin->id, visite, nbVisite)) // Si l'on a jamais visité le territoire
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

int GetContiguousTerritories(const SMap *map, int idJoueur){
	int n = 0;
	int t = 0;
	for (int i=0; i<map->nbCells; ++i){
		if (map->cells[i].owner == idJoueur){
			t = GetNbContiguousTerritories(map, map->cells[i].id);
			n = (t > n) ? t : n;
		}
	}
	return n;
}

int GetFavoriteTarget(const SMap *map){
	//La Favorite target est le joueur le plus puissant de la map
	int favoriteTarget = 0;
	for(int i=0;i< numberOfPlayers;i++){
		//Pour chaque joueur
		//tablDicesPerPlayer[i] est le score du joueur
		if ((favoriteTarget == 0)||((tablDicesPerPlayer[i] >= tablDicesPerPlayer[favoriteTarget])&&(i != MemContext[idCurr].id))){
			//Si elle n'a pas de cible ou si (le joueur possede plus de des que la cible et le joueur en question n'est pas elle)
			favoriteTarget = i;
		}
	}
	return favoriteTarget;
}

int BasicTest(SCell* hunterCell,SCell* targetCell){
	return (hunterCell->nbDices >= targetCell->nbDices+2);
}

int SecondaryTest(SCell* hunterCell,SCell* targetCell){
	//Le test 2 est plus permissif que le test 1
	return (hunterCell->nbDices >= targetCell->nbDices);
}

int NoNeedForTest(SCell* hunterCell,SCell* targetCell){
	//Est appele dans le cas ou l'IA a une pile importante de des
	return 1;
}

SCell* GetTarget(const SMap *map,SCell* hunterCell,int favoriteTarget,int (*functionTest)(SCell*,SCell*)){
	//L'IA va obtenir toutes les cibles potentielles autour d'elle puis selectionner celle qui est la plus forte
	STargets* targets = InitSTargets();
	int averageDicesAround = GetAverageNumberDiceAround(hunterCell);
	if (averageDicesAround<= hunterCell->nbDices){
		//Si la moyenne de dés (des cellules n'appartenant pas a l'IA) est inferieure au nombre de dés que possède la cellule
		//Permet de ne pas jouer si autour les joueurs sont trop forts
		for (int j=0;j<(hunterCell->nbNeighbors);j++){
			//Pour chaque voisin
			if (hunterCell->neighbors[j]->owner != MemContext[idCurr].id){
				//Si le voisin n'appartient pas a l'IA
				if ((*functionTest)(hunterCell,hunterCell->neighbors[j])){
					//En general : Si on a plus de nombres de des que l'adversaire
					AddTarget(targets,hunterCell->neighbors[j],hunterCell->neighbors[j]->nbDices,favoriteTarget);
				}
			}

		}
	}
	SCell* finalTarget =  GetBestTarget(targets);
	DeleteTargets(targets);
	return finalTarget;
}

int GetShoot(const SMap *map, SCell** cellsOrdered, STurn *turn, int (*functionTest)(SCell*,SCell*), int size){
	int favoriteTarget = GetFavoriteTarget(map);

	for (int i=0;i<size;i++){
		//Pour chaque cellule du jeu
			//Si la cellule est a l'IA et elle peut jouer
		SCell* target = GetTarget(map,cellsOrdered[i],favoriteTarget,functionTest);
		if (target != NULL){
			turn->cellFrom = cellsOrdered[i]->id;
			turn->cellTo = target->id;
			return 1;
		}
	}
	return 0;
}

int getCurrentID(int id){
	idCurr = -1;
	for(int i =0; i < nbIA; i++) //récupération du contexte
	{
		if (MemContext[i].id == id) idCurr = i;
	}
	if(idCurr == -1){return 1;} //il ne trouve pas l'id lui correspondant
	else{return 0;}
}




// Fonction à appeler à chaque tour sur la stratégie et tant que le retour de fonction est vrai et qu'il n'y a pas d'erreur.
//	* Ne pas oublier pour l'arbitre de dupliquer toute la structure map pour chaque appel !
//	* En cas d'erreur, rétablir la carte dans l'état initial avant le premier tour du joueur.
// valeur de retour : booléen : 0 coups terminés, 1 structure turn complétée avec un nouveau coup à jouer.
int PlayTurn(unsigned int id, const SMap *map, STurn *turn){
	if(firstFight)
	{
		firstFight = 0;
		for(int i = 0; i < map->nbCells; i++)
		{
			if(map->cells[i].owner > ownerMax) ownerMax = map->cells[i].owner;
		}

		ownerMin = ownerMax - (numberOfPlayers - 1);

	}

	if (getCurrentID(id)){return 0;}
	//Si l'IA ne trouve pas l'ID lui correspondant

	MajTablDicesPerPlayer(map);
	//Mise a jour de la table nb des par joueur
	int size = 0;
	int (*functionTest)(SCell*,SCell*) = BasicTest;
	SCell** cellsOrdered = OrderCells(map, &size);
	int booleanTestTurn;

	if (map->stack[id] >= 10){
		functionTest = NoNeedForTest;
		booleanTestTurn = GetShoot(map,cellsOrdered,turn,functionTest, size);
		FreeOrderedTargets(cellsOrdered);
		return (booleanTestTurn);
	}else{
		functionTest = BasicTest;
		if (GetShoot(map,cellsOrdered,turn,functionTest, size)){
			//GetShoot rempli le turn si l'IA peut jouer
			FreeOrderedTargets(cellsOrdered);
			return 1;
		}else{
			if (map->stack[id] >= 0){
				functionTest = SecondaryTest;
				booleanTestTurn = GetShoot(map,cellsOrdered,turn,functionTest, size);
				FreeOrderedTargets(cellsOrdered);
				return (booleanTestTurn);
			}else{
				FreeOrderedTargets(cellsOrdered);
				return 0;
			}
		}
	}
}

// Fonction à appeler au début de chaque partie
// La stratégie doit compléter la structure SPlayerInfo
void InitGame(unsigned int id, unsigned int nbPlayer, SPlayerInfo *info){
	MemContext[nbIA++].id = id;
	numberOfPlayers = nbPlayer;
	firstFight = 1;
	strcpy((*info).name,IA_NAME);

	strcpy((*info).members[0],"Vincent BRULE");
	strcpy((*info).members[1],"Simon LASSALLE");
	strcpy((*info).members[2],"Corentin MIMEAU");
	strcpy((*info).members[3],"Pierre SAVATTE");


}

// Fonction à appeler à la fin de chaque partie
void EndGame(unsigned int id, unsigned int idWinner){ //TODO A refaire quand tu auras des choses à FREE
	idEnd++;
	firstFight = 1;
	for(int i =0; i < nbIA; i++) //récupération du contexte
	{
		if (MemContext[i].id == id) idCurr = i; //permet de savoir quel contexte free
	}
	//Nous n'avons rien a free
	if(idEnd == nbIA) //Toute les IA ont eu un EndTurn, on peut reintialiser les variables
	{
		idCurr = 0;
		nbIA = 0; //assure que l'on n'ira pas plus loin si une autre partie recommence
		numberOfPlayers = 0;
		idEnd = 0;
	}
}
