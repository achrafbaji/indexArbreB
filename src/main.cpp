#include <chrono>
#include <iostream>
#include <vector>
#include "film.h"
#include "imdb.h"

/**
 * Lit les films d'IMDb, affiche le temps de lecture et quelques films.
 *
 * @return 0 si la lecture a réussi
 */
int main(){
    auto debut = std::chrono::steady_clock::now();
    std::vector<Film> films = lireFilms("donnees/title.basics.tsv");
    auto fin = std::chrono::steady_clock::now();

    double secondes = std::chrono::duration<double>(fin - debut).count();
    std::cout << films.size() << " films lus en " << secondes << " s" << std::endl;

    for(int i = 0; i < 3 && i < (int) films.size(); i++)
        std::cout << films[i] << std::endl;
    std::cout << "..." << std::endl;
    std::cout << films.back() << std::endl;
    return 0;
}