#!/bin/sh
# Télécharge le fichier des titres IMDb dans donnees/
set -e
mkdir -p donnees
cd donnees
curl -O https://datasets.imdbws.com/title.basics.tsv.gz
gunzip -kf title.basics.tsv.gz
echo "Lignes : $(wc -l < title.basics.tsv)"