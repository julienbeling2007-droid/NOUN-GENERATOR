Générateur de Mots Aléatoires (par Syllabes)
Application C++ modulaire utilisant SDL3 et ImGui pour générer des mots aléatoires par assemblage de syllabes. Génère des prénoms, villes, pays, animaux et objets avec des règles personnalisables.
📋 Fonctionnalités

✨ Génération par syllabes : assemblage intelligent de syllabes selon le type
🎛️ Règles personnalisables :

Nombre de syllabes (min/max)
Consonnes doubles (ll, mm, nn, etc.)
Capitalisation (majuscule initiale)
Autorisation des espaces
Préfixe et suffixe personnalisés


🎨 Interface graphique moderne avec ImGui
📊 Historique des 50 derniers mots générés
⚡ Génération unique ou par lots (2 ou plus)
🔤 Syllabes spécifiques par catégorie (français)

🧩 Méthode de Génération par Syllabes
Types de syllabes :

Consonnes simples : b, c, d, f, g, etc.
Voyelles et diphtongues : a, e, i, ai, au, eu, ou, an, en, etc.
Consonnes doubles : ll, mm, nn, pp, rr, ss, tt, ff
Syllabes prédéfinies par type :

Prénoms : ma, ri, an, alex, tho, mas, etc.
Villes : par, lyon, mar, seille, bourg, ville, etc.
Pays : fran, al, magne, gique, lande, etc.
Animaux : li, on, ti, gre, phant, etc.
Objets : ta, ble, lam, pe, teur, etc.



Algorithme :
Le générateur combine des syllabes aléatoires selon le type choisi, en respectant :

Le nombre de syllabes demandé
L'alternance consonne-voyelle pour les mots génériques
Les syllabes typiques du type sélectionné

🏗️ Architecture Modulaire
random_word_generator/
├── build.py                  Script de compilation
├── README.md
├── include/
│   ├── generator.h          Module de génération
│   └── ui.h                 Module interface utilisateur
├── src/
│   ├── main.cpp             Point d'entrée
│   ├── generator.cpp        Implémentation du générateur
│   ├── ui.cpp               Implémentation de l'UI
│   ├── imgui_impl_sdl3.cpp
│   ├── imgui_impl_sdlrenderer3.cpp
│   └── imgui/               Bibliothèque ImGui
│       ├── imgui.cpp
│       ├── imgui_demo.cpp
│       ├── imgui_draw.cpp
│       ├── imgui_tables.cpp
│       └── imgui_widgets.cpp
├── build/                   Fichiers objets (généré)
└── bin/                     Exécutable (généré)

Les fichiers suivants sont nécessaires dans src/imgui/ :

imgui.cpp, imgui.h
imgui_demo.cpp
imgui_draw.cpp
imgui_tables.cpp
imgui_widgets.cpp
imgui_internal.h
imstb_rectpack.h, imstb_textedit.h, imstb_truetype.h

Les backends SDL3 doivent être copiés depuis imgui/backends/ vers src/ :

imgui_impl_sdl3.cpp, imgui_impl_sdl3.h
imgui_impl_sdlrenderer3.cpp, imgui_impl_sdlrenderer3.h

🚀 Compilation
Utiliser le script Python
bash# Compilation
python3 build.py

# Nettoyage
python3 build.py clean
Compilation manuelle (alternative)
bash# Créer les répertoires
mkdir -p build bin

# Compiler les sources
g++ -std=c++17 -c src/main.cpp -o build/main.o -Iinclude -Isrc/imgui
g++ -std=c++17 -c src/generator.cpp -o build/generator.o -Iinclude
g++ -std=c++17 -c src/ui.cpp -o build/ui.o -Iinclude -Isrc/imgui

# Compiler ImGui
g++ -std=c++17 -c src/imgui/imgui.cpp -o build/imgui.o -Isrc/imgui
g++ -std=c++17 -c src/imgui/imgui_demo.cpp -o build/imgui_demo.o -Isrc/imgui
g++ -std=c++17 -c src/imgui/imgui_draw.cpp -o build/imgui_draw.o -Isrc/imgui
g++ -std=c++17 -c src/imgui/imgui_tables.cpp -o build/imgui_tables.o -Isrc/imgui
g++ -std=c++17 -c src/imgui/imgui_widgets.cpp -o build/imgui_widgets.o -Isrc/imgui

# Compiler les backends
g++ -std=c++17 -c src/imgui_impl_sdl3.cpp -o build/imgui_impl_sdl3.o -Isrc/imgui
g++ -std=c++17 -c src/imgui_impl_sdlrenderer3.cpp -o build/imgui_impl_sdlrenderer3.o -Isrc/imgui

# Linker
g++ -o bin/random_word_generator build/*.o -lSDL3
▶️ Exécution
bash./bin/random_word_generator
🎮 Utilisation

Sélectionner le type de mot : Prénom, Ville, Pays, Animal ou Objet
Configurer les règles de syllabes :

Ajuster le nombre de syllabes min/max avec les sliders
Activer les consonnes doubles pour plus de variété
Activer/désactiver la majuscule initiale
Autoriser ou non les espaces
Ajouter un préfixe/suffixe (optionnel)


Générer :

Cliquer sur "Générer un mot" pour un seul mot
Cliquer sur "Générer 5 mots" pour générer en lot


Consulter l'historique dans le panneau "Résultats"
Effacer l'historique si nécessaire

Exemples de génération :

Prénom (3 syllabes) : "Marilan", "Alexan", "Thomari"
Ville (2-3 syllabes) : "Parlyon", "Marville", "Montbourg"
Pays (2-4 syllabes) : "Franmagne", "Belgique", "Polsue"
Animal (2-3 syllabes) : "Litigre", "Panloup", "Rephant"

📦 Modules
generator.h/cpp
Module responsable de la génération par syllabes :

Banques de syllabes par catégorie
Consonnes, voyelles et consonnes doubles
Assemblage intelligent des syllabes
Application des règles de génération
Génération unique ou multiple

ui.h/cpp
Module d'interface utilisateur :

Gestion de la fenêtre SDL3
Intégration d'ImGui
Panneaux interactifs
Affichage des résultats

main.cpp
Point d'entrée de l'application
🔄 Personnalisation
Ajouter de nouvelles syllabes
Modifier le fichier src/generator.cpp dans la méthode initializeSyllables() :
cpp// Exemple : ajouter des syllabes pour prénoms
nameSyllables = {
    "ma", "ri", "an", "lu", "ca", "so", "phi",
    // Ajoutez vos syllabes ici
    "zen", "kai", "mila", "etc"
};
Modifier les consonnes/voyelles
cppconsonants = {
    "b", "c", "d", // ... ajoutez plus de consonnes
};

vowels = {
    "a", "e", "i", // ... ajoutez plus de voyelles/diphtongues
};
Ajouter de nouvelles catégories

Ajouter un type dans l'enum WordType (generator.h)
Ajouter un vecteur de syllabes dans WordGenerator (private)
Initialiser ces syllabes dans initializeSyllables()
Mettre à jour le switch dans generate() et getWordTypeName()
Ajouter le nom dans le combo box de ui.cpp

📝 Licence
Projet éducatif - Libre d'utilisation
🤝 Contributions
Les contributions sont les bienvenues ! N'hésitez pas à proposer des améliorations.

Note : Assurez-vous d'avoir bien téléchargé et placé tous les fichiers ImGui avant de compiler.
