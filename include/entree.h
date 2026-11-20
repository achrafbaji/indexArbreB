#ifndef ENTREE_H
#define ENTREE_H

/**
 * Entrée de l'index : l'id d'un film et sa position dans la base SQLite.
 *
 * Deux entrées se comparent seulement par id, donc l'arbre B trie les entrées
 * par id et retrouve la position à partir de l'id.
 */
class Entree{
  public:
    /**
     * Construit une entrée vide, requise par ArbreB pour ses tableaux de clés.
     */
    Entree() : id(0), position(0){}

    /**
     * Construit une entrée.
     *
     * @param id id du film
     * @param position rowid du film dans la table films, 0 si inconnue
     */
    Entree(int id, long long position) : id(id), position(position){}

    /**
     * Compare deux entrées selon leur id.
     *
     * @param autre entrée à comparer
     * @return true si l'id de cette entrée est plus petit que celui de autre
     */
    bool operator<(const Entree& autre) const{ return id < autre.id; }

    int       id;
    long long position;
};

#endif
