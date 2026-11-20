#ifndef BASEFILMS_H
#define BASEFILMS_H
#include <stdexcept>
#include <string>
#include <vector>
#include <sqlite3.h>
#include "entree.h"
#include "film.h"

/**
 * Base SQLite qui stocke les films dans une table films.
 *
 * Chaque film est rangé à une position, le rowid de SQLite. La colonne id n'a
 * volontairement aucun index SQLite : c'est l'arbre B du projet qui servira
 * d'index pour retrouver la position d'un film à partir de son id.
 */
class BaseFilms{
  public:
    /**
     * Ouvre la base, la crée si le fichier n'existe pas, et crée la table films
     * si elle n'existe pas encore.
     *
     * @param chemin chemin du fichier de la base, par exemple "donnees/films.db"
     * @throws std::runtime_error si la base ne peut pas être ouverte
     */
    explicit BaseFilms(const std::string& chemin);

    /**
     * Ferme la base.
     */
    ~BaseFilms();

    /**
     * Copie interdite : une seule BaseFilms possède la connexion à la base.
     */
    BaseFilms(const BaseFilms&) = delete;

    /**
     * Affectation interdite : une seule BaseFilms possède la connexion à la base.
     */
    BaseFilms& operator=(const BaseFilms&) = delete;

    /**
     * Ajoute tous les films à la table, dans une seule transaction.
     *
     * @param films films à ajouter
     * @throws std::runtime_error si une insertion échoue
     */
    void insererTous(const std::vector<Film>& films);

    /**
     * Compte les films de la table.
     *
     * @return le nombre de films stockés
     * @throws std::runtime_error si la requête échoue
     */
    long long nombreFilms() const;

    /**
     * Lit le film rangé à une position donnée.
     *
     * @param position rowid du film dans la table
     * @param film reçoit le film lu
     * @return true si un film existe à cette position, false sinon
     * @throws std::runtime_error si la requête échoue
     */
    bool lire(long long position, Film& film) const;

    /**
     * Lit l'id et la position de tous les films, pour construire l'index.
     *
     * @return une entrée par film de la table
     * @throws std::runtime_error si la requête échoue
     */
    std::vector<Entree> lireEntrees() const;

  private:
    sqlite3* db;

    /**
     * Exécute une instruction SQL qui ne retourne aucune ligne.
     *
     * @param sql instruction à exécuter
     * @throws std::runtime_error si l'instruction échoue
     */
    void executer(const std::string& sql);

    /**
     * Construit une exception avec le dernier message d'erreur de SQLite.
     *
     * @param contexte description de l'opération qui a échoué
     * @return l'exception à lancer
     */
    std::runtime_error erreur(const std::string& contexte) const;
};

#endif