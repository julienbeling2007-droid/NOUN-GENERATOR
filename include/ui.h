#ifndef UI_H
#define UI_H

#include "generator.h"
#include "imgui.h"
#include <SDL3/SDL.h>
#include <vector>
#include <string>

// Structure pour définir un thème
struct Theme {
    std::string name;
    // Couleurs du dégradé de fond (début et fin)
    SDL_Color gradientStart;
    SDL_Color gradientEnd;
    // Couleurs ImGui
    ImVec4 accentColor;      // Couleur principale des boutons/headers
    ImVec4 textHighlight;    // Couleur des titres
    ImVec4 windowBg;         // Fond des fenêtres
};

class UI {
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
    void playSound();
    void updateBackground();
    void applyTheme(const Theme& theme);
    void updateTheme();  // Nouvelle mét hode pour gérer le changement de thème
    
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* backgroundTexture;
    
    // Audio
    SDL_AudioDeviceID audioDeviceID;
    SDL_AudioSpec audioSpec;
    Uint8* wavBuffer;
    Uint32 wavLength;
    
    // Générateur de mots
    WordGenerator generator;
    std::vector<std::string> generatedWords;
    
    // Paramètres UI
    int selectedType;
    int minSyllables;
    int maxSyllables;
    int wordCount;
    bool startWithCapital;
    bool allowSpaces;
    bool useDoubleConsonants;
    char prefix[64];
    char suffix[64];
    
    bool running;
    int windowWidth;
    int windowHeight;
    
    // Système de thèmes
    std::vector<Theme> themes;
    int currentThemeIndex;
    Uint64 lastThemeChange;
    const Uint64 THEME_CHANGE_INTERVAL = 60000; // 60 secondes en millisecondes
    
    // Pour l'animation de transition
    float transitionProgress;
    bool isTransitioning;
    Theme previousTheme;
    Theme currentTheme;
};

#endif