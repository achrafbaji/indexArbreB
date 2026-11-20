#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
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
 * Affiche la liste des commandes.
 */
static void afficherAide(){
    std::cout << "Commandes :\n"
              << "  chercher <id>                          affiche le film\n"
              << "  ajouter <id> <annee> <duree> <titre>   ajoute un film\n"
              << "  supprimer <id>                         supprime le film\n"
              << "  mesurer                                compare les temps de recherche\n"
              << "  aide                                   affiche cette liste\n"
              << "  quitter                                termine le programme\n";
}

/**
 * Cherche un film par son id : l'arbre B donne la position, SQLite lit le film.
 *
 * @param index arbre B qui associe chaque id à une position
 * @param base base qui contient les films
 * @param id id du film à chercher
 */
static void chercher(const ArbreB<Entree>& index, const BaseFilms& base, int id){
    auto debut = std::chrono::steady_clock::now();
    const Entree* entree = index.rechercher(Entree(id, 0));
    Film film;
    bool trouve = entree != nullptr && base.lire(entree->position, film);
    double microsecondes = secondesDepuis(debut) * 1e6;

    if(trouve)
        std::cout << film << "\n  position " << entree->position << ", trouvé en "
                  << microsecondes << " µs" << std::endl;
    else
        std::cout << "aucun film avec l'id " << id << std::endl;
}

/**
 * Ajoute un film à la base et son entrée à l'index, si l'id est libre.
 *
 * @param index arbre B qui associe chaque id à une position
 * @param base base qui contient les films
 * @param film film à ajouter
 */
static void ajouter(ArbreB<Entree>& index, BaseFilms& base, const Film& film){
    if(index.contient(Entree(film.id, 0))){
        std::cout << "un film a déjà l'id " << film.id << std::endl;
        return;
    }
    long long position = base.ajouter(film);
    index.inserer(Entree(film.id, position));
    std::cout << "ajouté à la position " << position << " : " << film << std::endl;
}

/**
 * Supprime un film de la base et son entrée de l'index.
 *
 * @param index arbre B qui associe chaque id à une position
 * @param base base qui contient les films
 * @param id id du film à supprimer
 */
static void supprimer(ArbreB<Entree>& index, BaseFilms& base, int id){
    const Entree* entree = index.rechercher(Entree(id, 0));
    if(entree == nullptr){
        std::cout << "aucun film avec l'id " << id << std::endl;
        return;
    }
    base.supprimer(entree->position);
    index.enlever(Entree(id, 0));
    std::cout << "film " << id << " supprimé" << std::endl;
}

/**
 * Affiche une ligne du tableau des mesures.
 *
 * @param methode nom de la méthode mesurée
 * @param recherches nombre de recherches faites
 * @param secondes temps total des recherches
 */
static void afficherMesure(const std::string& methode, int recherches, double secondes){
    std::ostringstream temps;
    temps << std::fixed << std::setprecision(2) << secondes / recherches * 1e6;
    std::cout << "  " << std::left << std::setw(26) << methode << std::right
              << std::setw(8) << recherches << " recherches"
              << std::setw(12) << temps.str() << " µs par recherche" << std::endl;
}

/**
 * Compare le temps de recherche d'un film par id avec quatre méthodes :
 * l'arbre B seul, l'arbre B suivi de la lecture dans SQLite, SQLite sans
 * index et SQLite avec son propre index. L'index SQLite est supprimé à la fin.
 *
 * @param index arbre B qui associe chaque id à une position
 * @param base base qui contient les films
 */
static void mesurer(const ArbreB<Entree>& index, BaseFilms& base){
    std::vector<Entree> entrees = base.lireEntrees();
    if(entrees.empty()){
        std::cout << "la base est vide" << std::endl;
        return;
    }
    std::mt19937 generateur(2026);
    std::uniform_int_distribution<size_t> hasard(0, entrees.size() - 1);
    auto idsAuHasard = [&](int n){
        std::vector<int> ids;
        for(int i = 0; i < n; i++)
            ids.push_back(entrees[hasard(generateur)].id);
        return ids;
    };

    std::cout << entrees.size() << " films, arbre B d'ordre " << ORDRE_INDEX
              << " et de hauteur " << index.hauteur() << std::endl;

    Film film;
    int trouves = 0;

    std::vector<int> ids = idsAuHasard(1000000);
    auto debut = std::chrono::steady_clock::now();
    for(int id : ids)
        trouves += index.rechercher(Entree(id, 0)) != nullptr;
    afficherMesure("arbre B seul", ids.size(), secondesDepuis(debut));

    ids = idsAuHasard(10000);
    debut = std::chrono::steady_clock::now();
    for(int id : ids)
        trouves += base.lire(index.rechercher(Entree(id, 0))->position, film);
    afficherMesure("arbre B + lecture SQLite", ids.size(), secondesDepuis(debut));

    ids = idsAuHasard(20);
    debut = std::chrono::steady_clock::now();
    for(int id : ids)
        trouves += base.chercherParId(id, film);
    afficherMesure("SQLite sans index", ids.size(), secondesDepuis(debut));

    debut = std::chrono::steady_clock::now();
    base.creerIndexSQLite();
    std::cout << "  (index SQLite créé en " << secondesDepuis(debut) << " s)" << std::endl;
    ids = idsAuHasard(10000);
    debut = std::chrono::steady_clock::now();
    for(int id : ids)
        trouves += base.chercherParId(id, film);
    afficherMesure("SQLite avec son index", ids.size(), secondesDepuis(debut));
    base.supprimerIndexSQLite();

    std::cout << "  " << trouves << " films trouvés au total" << std::endl;
}

/**
 * Exécute une ligne de commande tapée par l'utilisateur.
 *
 * @param ligne ligne lue au clavier
 * @param index arbre B qui associe chaque id à une position
 * @param base base qui contient les films
 * @return false si l'utilisateur veut quitter, true sinon
 */
static bool executer(const std::string& ligne, ArbreB<Entree>& index, BaseFilms& base){
    std::istringstream entree(ligne);
    std::string commande;
    if(!(entree >> commande))
        return true;

    if(commande == "quitter")
        return false;

    if(commande == "aide"){
        afficherAide();
        return true;
    }

    if(commande == "mesurer"){
        mesurer(index, base);
        return true;
    }

    if(commande == "chercher" || commande == "supprimer"){
        int id;
        if(!(entree >> id)){
            std::cout << "usage : " << commande << " <id>" << std::endl;
            return true;
        }
        if(commande == "chercher")
            chercher(index, base, id);
        else
            supprimer(index, base, id);
        return true;
    }

    if(commande == "ajouter"){
        int id, annee, duree;
        std::string titre;
        if(!(entree >> id >> annee >> duree) || !std::getline(entree >> std::ws, titre) || titre.empty()){
            std::cout << "usage : ajouter <id> <annee> <duree> <titre>" << std::endl;
            return true;
        }
        ajouter(index, base, Film(id, titre, titre, annee, duree, ""));
        return true;
    }

    std::cout << "commande inconnue : " << commande << " (tape aide)" << std::endl;
    return true;
}

/**
 * Remplit la base au premier lancement, construit l'index arbre B à partir de
 * la base, puis exécute les commandes tapées jusqu'à quitter.
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

        afficherAide();
        std::string ligne;
        std::cout << "> " << std::flush;
        while(std::getline(std::cin, ligne) && executer(ligne, index, base))
            std::cout << "> " << std::flush;
    }
    catch(const std::exception& e){
        std::cerr << "Erreur : " << e.what() << std::endl;
        return 1;
    }
    return 0;
}