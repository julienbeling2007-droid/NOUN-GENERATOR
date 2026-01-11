#include "ui.h"
#include <iostream>
#include <windows.h>

int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    UI ui;
    
    if (!ui.init()) {
        std::cerr << "Erreur lors de l'initialisation de l'UI!" << std::endl;
        return -1;
    }
    
    std::cout << "==================================" << std::endl;
    std::cout << "  Générateur de Mots - SDL3" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "Application lancée avec succès!😁😁" << std::endl;
    std::cout << std::endl;
    
    ui.run();
    
    std::cout << "Application terminée..👌👌  :" << std::endl;
    
    return 0;
}