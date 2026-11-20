#include "imdb.h"
#include <fstream>
#include <stdexcept>

/**
 * Extrait la partie numérique d'un tconst.
 *
 * @param tconst identifiant IMDb de la forme "tt0111161"
 * @return la partie numérique, par exemple 111161
 * @throws std::invalid_argument si le tconst ne contient pas de nombre
 */
static int idDepuisTconst(const std::string& tconst){
    return std::stoi(tconst.substr(2));
}

/**
 * Convertit un champ IMDb en entier.
 *
 * @param s champ lu dans le fichier, "\N" si la valeur est absente
 * @return la valeur du champ, 0 si elle est absente
 * @throws std::invalid_argument si le champ n'est ni "\N" ni un nombre
 */
static int entierOuZero(const std::string& s){
    if(s == "\\N") return 0;
    return std::stoi(s);
}

/**
 * Découpe une ligne du fichier aux tabulations.
 *
 * @param ligne ligne lue dans le fichier
 * @return les champs de la ligne, dans l'ordre
 */
static std::vector<std::string> decouper(const std::string& ligne){
    std::vector<std::string> champs;
    std::string champ;
    for(char c : ligne){
        if(c == '\t'){ champs.push_back(champ); champ.clear(); }
        else champ += c;
    }
    champs.push_back(champ);
    return champs;
}

std::vector<Film> lireFilms(const std::string& chemin){
    std::ifstream fichier(chemin);
    if(!fichier)
        throw std::runtime_error("impossible d'ouvrir " + chemin);

    std::vector<Film> films;
    std::string ligne;
    std::getline(fichier, ligne);

    while(std::getline(fichier, ligne)){
        std::vector<std::string> c = decouper(ligne);
        if(c.size() != 9 || c[1] != "movie")
            continue;
        films.push_back(Film(idDepuisTconst(c[0]), c[2], c[3],
                             entierOuZero(c[5]), entierOuZero(c[7]),
                             c[8] == "\\N" ? "" : c[8]));
    }
    return films;
}