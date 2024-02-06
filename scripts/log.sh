#!/bin/bash

if [ "$#" -lt 1 ]; then
    echo "Erreur, aucune chaîne à ajouter au fichier"
    exit 1
fi

chaine="$*"
nom_fichier="stats.txt"

if [ ! -f "$nom_fichier" ]; then
    touch "$nom_fichier"
    echo "Le fichier $nom_fichier a été créé."
fi

echo "$chaine" >> "$nom_fichier"
echo "La chaîne a été ajoutée au fichier $nom_fichier."

