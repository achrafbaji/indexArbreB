#include <chrono>
#include <iostream>
#include <vector>
#include "baseFilms.h"
#include "film.h"
#include "imdb.h"

/**
 * Remplit la base SQLite avec les films d'IMDb au premier lancement, puis
 * affiche le nombre de films et quelques films lus par leur position.
 *
 * @return 0 si tout a réussi, 1 en cas d'erreur
 */
int main(){
    try{
        BaseFilms base("donnees/films.db");

        if(base.nombreFilms() == 0){
            std::cout << "Premier lancement : chargement des films dans SQLite..." << std::endl;
            auto debut = std::chrono::steady_clock::now();
            std::vector<Film> films = lireFilms("donnees/title.basics.tsv");
            base.insererTous(films);
            auto fin = std::chrono::steady_clock::now();
            std::cout << "Chargé en " << std::chrono::duration<double>(fin - debut).count()
                      << " s" << std::endl;
        }

        std::cout << base.nombreFilms() << " films dans la base" << std::endl;

        Film film;
        for(long long position : {1LL, 2LL, base.nombreFilms()})
            if(base.lire(position, film))
                std::cout << "position " << position << " : " << film << std::endl;
    }
    catch(const std::exception& e){
        std::cerr << "Erreur : " << e.what() << std::endl;
        return 1;
    }
    return 0;
}