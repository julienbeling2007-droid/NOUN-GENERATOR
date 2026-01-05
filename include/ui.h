#ifndef UI_H
#define UI_H

#include "generator.h"
#include <SDL3/SDL.h>
#include <vector>
#include <string>

// Forward declarations pour ImGui
struct ImGuiContext;

/**
 * Classe gérant l'interface utilisateur avec ImGui et SDL3
 */
class UI {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    ImGuiContext* imguiContext;
    
    WordGenerator* generator;
    GenerationRules currentRules;
    std::vector<std::string> generatedWords;
    
    bool running;
    
    // Variables pour l'interface
    int currentTypeIndex;
    char prefixBuffer[128];
    char suffixBuffer[128];
    bool showDebugInfo;
    
    void renderMainWindow();
    void renderRulesPanel();
    void renderResultsPanel();
    void renderControlPanel();
    
public:
    UI();
    ~UI();
    
    bool initialize(const char* title, int width, int height);
    void run();
    void shutdown();
    
    bool isRunning() const { return running; }
};

#endif // UI_H