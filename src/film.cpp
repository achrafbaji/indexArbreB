#include "film.h"
#include <iomanip>

Film::Film() : id(0), annee(0), duree(0){
}

Film::Film(int id, const std::string& titre, const std::string& titreOriginal,
           int annee, int duree, const std::string& genres)
 : id(id), titre(titre), titreOriginal(titreOriginal),
   annee(annee), duree(duree), genres(genres){
}

bool Film::operator<(const Film& autre) const{
    return id < autre.id;
}

std::ostream& operator<<(std::ostream& os, const Film& f){
    os << "tt" << std::setw(7) << std::setfill('0') << f.id << std::setfill(' ')
       << " | " << f.titre << " (" << f.annee << ") | "
       << f.duree << " min | " << f.genres;
    return os;
}