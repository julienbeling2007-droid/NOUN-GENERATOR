#ifndef UI_H
#define UI_H

#include "generator.h"
#include <SDL3/SDL.h>
#include <vector>
#include <string>

class UI {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* backgroundTexture;  // Ajout pour l'image de fond
    WordGenerator generator;
    
    // État de l'UI
    int selectedType;
    int minSyllables;
    int maxSyllables;
    int wordCount;
    bool startWithCapital;
    bool allowSpaces;
    bool useDoubleConsonants;
    char prefix[64];
    char suffix[64];
    
    // Résultats
    std::vector<std::string> generatedWords;
    
    // État de la fenêtre
    bool running;
    int windowWidth;
    int windowHeight;
    
public:
    UI();
    ~UI();
    
    bool init();
    void run();
    void cleanup();
    
private:
    void handleEvents();
    void render();
    void generateWords();
};

#endif // UI_H