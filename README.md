Compilation:

- "make DiceWars" pour compiler le jeu arbitre
    
- "make libIA" pour compiler l'IA
    
- "make" ou "make all" pour compiler les deux
    
L'executable du jeu est placé dans DiceWars/ et libIA.so est placé dans Dicewars/IA/.

Pour lancer le jeu, se placer dans le dossier DiceWars et lancer "./DiceWars X Y [IA1.so, IA2.so, ...]" où X est le nombre de parties, Y le nombre de joueurs, et IA.so une bibliothèque IA.
Exemple: "./DiceWars 10 3 IA/libIA.so IA/libIA.so" pour lancer le jeu avec 10 parties, 3 joueurs, dont deux intelligences artificielles situées dans le dossier IA.


Prérèglage:

Deux constantes de affichage.h peuvent être modifiées pour changer le comportement de l'affichage.

- DELAY_AFFICHAGE à 0 permet d'accélérer le rendu (nottament la remise des dés et les coups des IA)
 
- AVEC_AFFICHAGE à 0 désactive l'affichage partiellement (affiche uniquement le résultat de la partie) pour tester plus rapidement les combats d'IA
    
