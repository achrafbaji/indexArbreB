#ifndef IMDB_H
#define IMDB_H
#include <string>
#include <vector>
#include "film.h"

/**
 * Lit le fichier title.basics.tsv d'IMDb et garde seulement les films.
 *
 * @param chemin chemin du fichier title.basics.tsv
 * @return les films dont le titleType vaut "movie", dans l'ordre du fichier
 * @throws std::runtime_error si le fichier ne peut pas être ouvert
 */
std::vector<Film> lireFilms(const std::string& chemin);

#endif