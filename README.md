# Dessiner sur un canevas ASCII"

## Description

Le projet canvascii.c consiste à créer un programme en C qui permet de dessiner sur un canevas ASCII. Le canevas peut être initialisé ou lu à partir de l'entrée standard (stdin) et le résultat est affiché sur la sortie standard (stdout). Le programme doit avoir une mise en œuvre précise pour permettre une correction automatisée et des tests automatisés.

Projet réalisé dans le cadre du cours Construction et maintenance de logiciels. Sigle: INF3135. Enseignant: Serge Dogny.  Université du Quebec à Montréal.
[sujet du travail](sujet.md).

## Auteur

Randy Razafindrabe (RAZR11308805)

## Fonctionnement

Le canevas est représenté par un fichier texte contenant h lignes de même longueur w. Les seuls caractères acceptés sont `.` qui représente un pixel vide, et les nombres de 0 à 7 qui représentent respectivement un pixel noir, rouge, vert, jaune, bleu, magenta, cyan et blanc.

Lorsqu'il est lancé sans argument, le programme affiche un manuel d'utilisation sur la sortie standard. Il prend en charge les options suivantes:

  * `-n HEIGHT,WIDTH`: crée un nouveau canevas vide de hauteur h et de largeur w. Cette option doit être utilisée en premier, sinon le comportement est indéterminé.
  * `-s`: affiche le canevas et quitte le programme.
  * `-k`: active la sortie en couleur.
  * `-p` CHAR`: définit le crayon à CHAR. Les seuls crayons autorisés sont 0 à 7.
  * `-h ROW`: dessine une ligne horizontale sur la ligne ROW.
  * `-v COL`: dessine une ligne verticale sur la colonne COL.
  * `-r ROW,COL,HEIGHT,WIDTH`: dessine un rectangle de dimension HEIGHT x WIDTH avec le coin supérieur gauche à (ROW, COL).
  * `-l ROW1,COL1,ROW2,COL2`: dessine un segment discret de (ROW1, COL1) à (ROW2, COL2) avec l'algorithme de Bresenham.
  * `-c ROW,COL,RADIUS`: dessine un cercle centré à (ROW, COL) de rayon RADIUS avec l'algorithme du point milieu.

Les dimensions maximales autorisées pour le canevas sont 40 lignes et 80 colonnes.

Voici quelques commande pour utiliser le projet:
  * La commande `make` crée l'exécutable `canvascii` s'il y a eu une modification du fichier `canvascii.c` en le compilant selon le standard C11, avec lesoptions `-Wall` et `-Wextra` lors de la compilation.
  * La commande `make test` recompile l'exécutable si nécessaire, puis lance la suite de tests contenue dans le fichier `check.bats`.
  * La commande `make html` transforme les fichiers `README.md` et `sujet.md` en HTML.
  * La commande `make clean` supprime les fichiers inutiles ou générés (`.o`,`.html`, etc.).

Voici un exemple de commandes pour exécuter le programme:
  ```./canvascii -n 2,4 ./canvascii -s -h 0 < examples/empty5x8.canvas ./canvascii -n 5,7 -r 1,1,3,5 ./canvascii -n 19,19 -p 1 -c 9,9,9 -l 3,3,15,15 -l 3,15,15,3 -k```

## Tests

Pour lancer les tests, il suffit de lancer la commande `make test`, le fichier check.bats se lancera automatiquement: 34 tests, 0 failures.

## Dépendances

Dépendances utilisées:
  * `GCC` pour la compilation 
  * `Bats` pour les tests.
  * `<stdio.h>`
  * `<string.h>`
  * `<stdlib.h>`
  * `<unistd.h>`
  * `<ctype.h>`

## Références

Deux fonctions nécessitaient l'utilisation d'algorithmes déjà existants.
Pour la fonction tracer_segment(), l'[Algorithme de Bresenham](https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm).
Pour la fonction tracer_cercle(), l'[Algorithme de tracé d'arc de cercle par point milieu](https://rosettacode.org/wiki/Bitmap/Midpoint_circle_algorithm#C).

## État du projet

* [X] Le nom du dépôt GitLab est exactement `inf3135-tp1-h23` (Pénalité de
  **50%**).
* [X] L'URL du dépôt GitLab est exactement (remplacer `utilisateur` par votre
  nom identifiant GitLab) `https://gitlab.info.uqam.ca/utilisateur/inf3135-tp1-h23`
  (Pénalité de **50%**).
* [X] Les usagers `dogny_g` et `correcteur` ont accès au projet en mode *Developer*
  (Pénalité de **50%**).
* [X] Le dépôt GitLab est un *fork* de [ce
  dépôt](https://gitlab.info.uqam.ca/inf3135-sdo/canvascii)
  (Pénalité de **50%**).
* [X] Le dépôt GitLab est privé (Pénalité de **50%**).
* [X] Le dépôt contient au moins un fichier `.gitignore`.
* [X] Le fichier `Makefile` permet de compiler le projet lorsqu'on entre
  `make`. Il supporte les cibles `html`, `test` et `clean`.
* [X] Le nombre de tests qui réussissent/échouent avec la `make test` est
  indiqué quelque part dans le fichier `README.md`.
* [X] Les sections incomplètes de ce fichier (`README.md`) ont été complétées.
* [X] L'en-tête du fichier est documentée.
* [X] L'en-tête des déclarations des fonctions est documentée (*docstring*).
* [X] Le programme ne contient pas de valeurs magiques.
