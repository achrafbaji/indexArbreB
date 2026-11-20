#include <chrono>
#include <iostream>
#include <vector>
#include "arbreB.h"
#include "baseFilms.h"
#include "entree.h"
#include "film.h"
#include "imdb.h"

/**
 * Ordre de l'arbre B qui sert d'index : nombre maximum d'enfants d'un noeud.
 */
const int ORDRE_INDEX = 32;

/**
 * Mesure le temps écoulé depuis un instant donné.
 *
 * @param debut instant de départ
 * @return le nombre de secondes écoulées depuis debut
 */
static double secondesDepuis(std::chrono::steady_clock::time_point debut){
    return std::chrono::duration<double>(std::chrono::steady_clock::now() - debut).count();
}

/**
 * Cherche un film par son id : l'arbre B donne la position, SQLite lit le film.
 *
 * @param index arbre B qui associe chaque id à une position
 * @param base base qui contient les films
 * @param id id du film à chercher
 */
static void chercherFilm(const ArbreB<Entree>& index, const BaseFilms& base, int id){
    const Entree* entree = index.rechercher(Entree(id, 0));
    Film film;
    if(entree != nullptr && base.lire(entree->position, film))
        std::cout << "id " << id << " -> position " << entree->position << " : " << film << std::endl;
    else
        std::cout << "id " << id << " : aucun film" << std::endl;
}

/**
 * Remplit la base au premier lancement, construit l'index arbre B à partir de
 * la base, puis cherche quelques films par leur id.
 *
 * @return 0 si tout a réussi, 1 en cas d'erreur
 */
int main(){
    try{
        BaseFilms base("donnees/films.db");

        if(base.nombreFilms() == 0){
            std::cout << "Premier lancement : chargement des films dans SQLite..." << std::endl;
            auto debut = std::chrono::steady_clock::now();
            base.insererTous(lireFilms("donnees/title.basics.tsv"));
            std::cout << "Chargé en " << secondesDepuis(debut) << " s" << std::endl;
        }

        auto debut = std::chrono::steady_clock::now();
        std::vector<Entree> entrees = base.lireEntrees();
        ArbreB<Entree> index(ORDRE_INDEX);
        for(const Entree& e : entrees)
            index.inserer(e);
        std::cout << "Index arbre B d'ordre " << ORDRE_INDEX << " construit avec "
                  << entrees.size() << " entrées en " << secondesDepuis(debut) << " s" << std::endl;

        for(int id : {9, 111161, 1375666, 1})
            chercherFilm(index, base, id);
    }
    catch(const std::exception& e){
        std::cerr << "Erreur : " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
