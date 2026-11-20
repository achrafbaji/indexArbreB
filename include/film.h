#ifndef FILM_H
#define FILM_H
#include <iostream>
#include <string>

/**
 * Film tiré du fichier title.basics.tsv d'IMDb.
 *
 * L'identifiant est la partie numérique du tconst (tt0111161 donne 111161).
 * Une année ou une durée inconnue vaut 0, des genres inconnus valent "".
 */
class Film{
  public:
    /**
     * Construit un film vide, requis par ArbreB pour ses tableaux de clés.
     */
    Film();

    /**
     * Construit un film à partir de ses champs.
     *
     * @param id partie numérique du tconst
     * @param titre titre principal
     * @param titreOriginal titre dans la langue d'origine
     * @param annee année de sortie, 0 si inconnue
     * @param duree durée en minutes, 0 si inconnue
     * @param genres genres séparés par des virgules, "" si inconnus
     */
    Film(int id, const std::string& titre, const std::string& titreOriginal,
         int annee, int duree, const std::string& genres);

    /**
     * Compare deux films selon leur identifiant.
     *
     * @param autre film à comparer
     * @return true si l'identifiant de ce film est plus petit que celui de autre
     */
    bool operator<(const Film& autre) const;

    int         id;
    std::string titre;
    std::string titreOriginal;
    int         annee;
    int         duree;
    std::string genres;
};

/**
 * Écrit un film sur une ligne : tconst | titre (année) | durée | genres.
 *
 * @param os flux de sortie
 * @param f film à écrire
 * @return le flux os
 */
std::ostream& operator<<(std::ostream& os, const Film& f);

#endif