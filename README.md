# indexArbreB

Ce projet montre à quoi sert un **arbre B** dans une vraie base de données.

Un arbre B est codé en C++, puis utilisé pour chercher rapidement des films dans
une base SQLite qui contient **761 544 films** venant d'IMDb.

## L'idée

Une base de données range les films les uns après les autres. Pour trouver un
film, sans aide, il faut les regarder **un par un** jusqu'à tomber sur le bon.
Avec des centaines de milliers de films, c'est lent.

Un **index** règle ce problème. C'est comme l'index à la fin d'un livre : au lieu
de lire tout le livre, on regarde l'index, il donne la page, et on va
directement à la bonne page.

Dans ce projet :

- la base SQLite range chaque film à une **position** ;
- l'arbre B joue le rôle de l'index : on lui donne l'identifiant d'un film, il
  donne sa position ;
- on lit ensuite le film directement à cette position.

## C'est quoi un arbre B ?

C'est un arbre de recherche où chaque nœud contient **plusieurs clés** et peut
avoir **plusieurs enfants** (pas seulement deux comme dans un arbre binaire).

- Avec un arbre d'ordre 32, un nœud peut avoir jusqu'à 32 enfants.
- L'arbre est donc très large et très peu haut : ici, **5 niveaux** suffisent
  pour 761 544 films.
- Pour trouver un film, on lit au plus 5 nœuds.
- Quand un nœud est trop plein, il se coupe en deux. Quand il est trop vide, il
  emprunte une clé à un voisin ou se fusionne avec lui. Comme ça, l'arbre reste
  toujours équilibré.

## Résultats

Temps moyen pour trouver un film, mesuré sur un Mac :

| Méthode | Temps |
|---|---|
| Sans index (SQLite regarde tous les films) | 11 345 µs |
| Avec l'arbre B du projet | **10 µs** |
| Avec l'index de SQLite | 13 µs |

L'arbre B rend la recherche environ **1 100 fois plus rapide** que sans index,
et il est aussi rapide que l'index de SQLite.

Une différence quand même : l'arbre B est gardé en mémoire et doit être
reconstruit à chaque lancement (environ 0,1 seconde). L'index de SQLite, lui, est
sauvegardé sur le disque.

## Organisation du code

```
include/arbreB.h      l'arbre B : chercher, insérer, enlever
include/entree.h      ce que l'arbre stocke : identifiant du film et position
include/film.h        la classe Film
include/imdb.h        lecture du fichier IMDb
include/baseFilms.h   tout ce qui touche à SQLite
src/                  le code des fonctions et le programme principal
scripts/              le script qui télécharge les données
```

## Comment l'essayer

Il faut :

- un compilateur C++ (g++ ou clang++) ;
- SQLite (déjà installé sur Mac ; sur Ubuntu : `sudo apt install libsqlite3-dev`) ;
- environ 1,5 Go d'espace libre pour les données.

Ensuite :

```bash
./scripts/telecharger_donnees.sh
make
./indexArbreB
```

Le premier lancement prend quelques secondes de plus : le programme remplit la
base avec les films. Ensuite, ces commandes sont disponibles :

```
chercher <id>                          affiche un film
ajouter <id> <annee> <duree> <titre>   ajoute un film
supprimer <id>                         supprime un film
mesurer                                compare les temps de recherche
aide                                   affiche les commandes
quitter                                quitte le programme
```

Exemple : `chercher 1375666` affiche *Inception* (son code IMDb est `tt1375666`).

## Les données

Les films viennent des données publiques d'IMDb
(https://developer.imdb.com/non-commercial-datasets/), utilisées pour un projet
personnel, sans but commercial. Elles ne sont pas dans ce dépôt parce qu'elles
sont trop lourdes : le script `scripts/telecharger_donnees.sh` les télécharge.