#include <iostream>
#include "ui.h"

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    
    std::cout << "==================================" << std::endl;
    std::cout << " Générateur de Mots Aléatoires" << std::endl;
    std::cout << "==================================" << std::endl;
    
    UI ui;
    
    if (!ui.initialize("Générateur de Mots - ImGui + SDL3", 800, 600)) {
        std::cerr << "Erreur: Impossible d'initialiser l'application" << std::endl;
        return 1;
    }
    
    std::cout << "Application démarrée avec succès!" << std::endl;
    std::cout << "Interface graphique en cours d'exécution..." << std::endl;
    
    ui.run();
    
    std::cout << "Fermeture de l'application..." << std::endl;
    ui.shutdown();
    
    return 0;
}