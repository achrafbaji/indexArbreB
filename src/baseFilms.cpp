#include "baseFilms.h"
#include <stdexcept>

BaseFilms::BaseFilms(const std::string& chemin) : db(nullptr){
    if(sqlite3_open(chemin.c_str(), &db) != SQLITE_OK){
        std::runtime_error e = erreur("ouverture de " + chemin);
        sqlite3_close(db);
        throw e;
    }
    executer("CREATE TABLE IF NOT EXISTS films("
             "id INTEGER, titre TEXT, titre_original TEXT, "
             "annee INTEGER, duree INTEGER, genres TEXT)");
}

BaseFilms::~BaseFilms(){
    sqlite3_close(db);
}

void BaseFilms::insererTous(const std::vector<Film>& films){
    sqlite3_stmt* requete = nullptr;
    if(sqlite3_prepare_v2(db,
           "INSERT INTO films(id, titre, titre_original, annee, duree, genres) "
           "VALUES(?, ?, ?, ?, ?, ?)", -1, &requete, nullptr) != SQLITE_OK)
        throw erreur("préparation de l'insertion");

    executer("BEGIN");
    for(const Film& f : films){
        sqlite3_bind_int(requete, 1, f.id);
        sqlite3_bind_text(requete, 2, f.titre.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(requete, 3, f.titreOriginal.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(requete, 4, f.annee);
        sqlite3_bind_int(requete, 5, f.duree);
        sqlite3_bind_text(requete, 6, f.genres.c_str(), -1, SQLITE_TRANSIENT);
        if(sqlite3_step(requete) != SQLITE_DONE){
            std::runtime_error e = erreur("insertion du film " + std::to_string(f.id));
            sqlite3_finalize(requete);
            executer("ROLLBACK");
            throw e;
        }
        sqlite3_reset(requete);
    }
    sqlite3_finalize(requete);
    executer("COMMIT");
}

long long BaseFilms::nombreFilms() const{
    sqlite3_stmt* requete = nullptr;
    if(sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM films", -1, &requete, nullptr) != SQLITE_OK)
        throw erreur("comptage des films");
    long long n = 0;
    if(sqlite3_step(requete) == SQLITE_ROW)
        n = sqlite3_column_int64(requete, 0);
    sqlite3_finalize(requete);
    return n;
}

bool BaseFilms::lire(long long position, Film& film) const{
    sqlite3_stmt* requete = nullptr;
    if(sqlite3_prepare_v2(db,
           "SELECT id, titre, titre_original, annee, duree, genres "
           "FROM films WHERE rowid = ?", -1, &requete, nullptr) != SQLITE_OK)
        throw erreur("lecture d'un film");
    sqlite3_bind_int64(requete, 1, position);

    bool trouve = sqlite3_step(requete) == SQLITE_ROW;
    if(trouve)
        film = Film(sqlite3_column_int(requete, 0),
                    reinterpret_cast<const char*>(sqlite3_column_text(requete, 1)),
                    reinterpret_cast<const char*>(sqlite3_column_text(requete, 2)),
                    sqlite3_column_int(requete, 3),
                    sqlite3_column_int(requete, 4),
                    reinterpret_cast<const char*>(sqlite3_column_text(requete, 5)));
    sqlite3_finalize(requete);
    return trouve;
}

bool BaseFilms::chercherParId(int id, Film& film) const{
    sqlite3_stmt* requete = nullptr;
    if(sqlite3_prepare_v2(db,
           "SELECT id, titre, titre_original, annee, duree, genres "
           "FROM films WHERE id = ?", -1, &requete, nullptr) != SQLITE_OK)
        throw erreur("recherche par id");
    sqlite3_bind_int(requete, 1, id);

    bool trouve = sqlite3_step(requete) == SQLITE_ROW;
    if(trouve)
        film = Film(sqlite3_column_int(requete, 0),
                    reinterpret_cast<const char*>(sqlite3_column_text(requete, 1)),
                    reinterpret_cast<const char*>(sqlite3_column_text(requete, 2)),
                    sqlite3_column_int(requete, 3),
                    sqlite3_column_int(requete, 4),
                    reinterpret_cast<const char*>(sqlite3_column_text(requete, 5)));
    sqlite3_finalize(requete);
    return trouve;
}

void BaseFilms::creerIndexSQLite(){
    executer("CREATE INDEX IF NOT EXISTS index_id ON films(id)");
}

void BaseFilms::supprimerIndexSQLite(){
    executer("DROP INDEX IF EXISTS index_id");
}

long long BaseFilms::ajouter(const Film& film){
    sqlite3_stmt* requete = nullptr;
    if(sqlite3_prepare_v2(db,
           "INSERT INTO films(id, titre, titre_original, annee, duree, genres) "
           "VALUES(?, ?, ?, ?, ?, ?)", -1, &requete, nullptr) != SQLITE_OK)
        throw erreur("préparation de l'ajout");
    sqlite3_bind_int(requete, 1, film.id);
    sqlite3_bind_text(requete, 2, film.titre.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(requete, 3, film.titreOriginal.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(requete, 4, film.annee);
    sqlite3_bind_int(requete, 5, film.duree);
    sqlite3_bind_text(requete, 6, film.genres.c_str(), -1, SQLITE_TRANSIENT);
    bool reussi = sqlite3_step(requete) == SQLITE_DONE;
    sqlite3_finalize(requete);
    if(!reussi)
        throw erreur("ajout du film " + std::to_string(film.id));
    return sqlite3_last_insert_rowid(db);
}

bool BaseFilms::supprimer(long long position){
    sqlite3_stmt* requete = nullptr;
    if(sqlite3_prepare_v2(db, "DELETE FROM films WHERE rowid = ?", -1, &requete, nullptr) != SQLITE_OK)
        throw erreur("préparation de la suppression");
    sqlite3_bind_int64(requete, 1, position);
    bool reussi = sqlite3_step(requete) == SQLITE_DONE;
    sqlite3_finalize(requete);
    if(!reussi)
        throw erreur("suppression de la position " + std::to_string(position));
    return sqlite3_changes(db) > 0;
}



std::vector<Entree> BaseFilms::lireEntrees() const{
    sqlite3_stmt* requete = nullptr;
    if(sqlite3_prepare_v2(db, "SELECT id, rowid FROM films", -1, &requete, nullptr) != SQLITE_OK)
        throw erreur("lecture des entrées");
    std::vector<Entree> entrees;
    while(sqlite3_step(requete) == SQLITE_ROW)
        entrees.push_back(Entree(sqlite3_column_int(requete, 0), sqlite3_column_int64(requete, 1)));
    sqlite3_finalize(requete);
    return entrees;
}

void BaseFilms::executer(const std::string& sql){
    if(sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK)
        throw erreur(sql);
}

std::runtime_error BaseFilms::erreur(const std::string& contexte) const{
    return std::runtime_error(contexte + " : " + (db ? sqlite3_errmsg(db) : "mémoire insuffisante"));
}