#include "ui.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <cstring>

UI::UI() 
    : window(nullptr), 
      renderer(nullptr), 
      imguiContext(nullptr),
      generator(nullptr),
      running(false),
      currentTypeIndex(0),
      showDebugInfo(false) {
    
    std::memset(prefixBuffer, 0, sizeof(prefixBuffer));
    std::memset(suffixBuffer, 0, sizeof(suffixBuffer));
}

UI::~UI() {
    shutdown();
}

bool UI::initialize(const char* title, int width, int height) {
    // Initialiser SDL3
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Erreur SDL_Init: %s", SDL_GetError());
        return false;
    }
    
    // Créer la fenêtre
    window = SDL_CreateWindow(title, width, height, SDL_WINDOW_RESIZABLE);
    if (!window) {
        SDL_Log("Erreur SDL_CreateWindow: %s", SDL_GetError());
        return false;
    }
    
    // Créer le renderer
    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        SDL_Log("Erreur SDL_CreateRenderer: %s", SDL_GetError());
        return false;
    }
    
    SDL_SetRenderVSync(renderer, 1);
    
    // Initialiser ImGui
    IMGUI_CHECKVERSION();
    imguiContext = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    // Style
    ImGui::StyleColorsDark();
    
    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
    
    // Créer le générateur
    generator = new WordGenerator();
    
    running = true;
    return true;
}

void UI::renderRulesPanel() {
    ImGui::Begin("Règles de génération");
    
    // Type de mot
    const char* types[] = { "Prénom", "Ville", "Pays", "Animal", "Objet" };
    ImGui::Combo("Type de mot", &currentTypeIndex, types, IM_ARRAYSIZE(types));
    currentRules.type = static_cast<WordType>(currentTypeIndex);
    
    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Syllabes");
    
    // Nombre de syllabes
    ImGui::SliderInt("Syllabes minimales", &currentRules.minSyllables, 1, 8);
    ImGui::SliderInt("Syllabes maximales", &currentRules.maxSyllables, 
                     currentRules.minSyllables, 10);
    
    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Options");
    
    // Options
    ImGui::Checkbox("Commencer par majuscule", &currentRules.startWithCapital);
    ImGui::Checkbox("Autoriser les espaces", &currentRules.allowSpaces);
    ImGui::Checkbox("Consonnes doubles (ll, mm, etc.)", &currentRules.useDoubleConsonants);
    
    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Personnalisation");
    
    // Préfixe et suffixe
    ImGui::InputText("Préfixe", prefixBuffer, sizeof(prefixBuffer));
    currentRules.prefix = prefixBuffer;
    
    ImGui::InputText("Suffixe", suffixBuffer, sizeof(suffixBuffer));
    currentRules.suffix = suffixBuffer;
    
    ImGui::End();
}

void UI::renderControlPanel() {
    ImGui::Begin("Contrôles");
    
    // Bouton de génération
    if (ImGui::Button("Générer un mot", ImVec2(200, 40))) {
        if (generator) {
            std::string word = generator->generate(currentRules);
            generatedWords.insert(generatedWords.begin(), word);
            
            // Limiter l'historique à 50 mots
            if (generatedWords.size() > 50) {
                generatedWords.resize(50);
            }
        }
    }
    
    ImGui::SameLine();
    
    // Bouton pour générer plusieurs mots
    if (ImGui::Button("Générer 5 mots", ImVec2(200, 40))) {
        if (generator) {
            auto words = generator->generateMultiple(currentRules, 5);
            generatedWords.insert(generatedWords.begin(), words.begin(), words.end());
            
            if (generatedWords.size() > 50) {
                generatedWords.resize(50);
            }
        }
    }
    
    ImGui::Separator();
    
    // Bouton pour effacer l'historique
    if (ImGui::Button("Effacer l'historique", ImVec2(-1, 30))) {
        generatedWords.clear();
    }
    
    ImGui::End();
}

void UI::renderResultsPanel() {
    ImGui::Begin("Résultats");
    
    ImGui::Text("Historique des mots générés :");
    ImGui::Separator();
    
    if (generatedWords.empty()) {
        ImGui::TextDisabled("Aucun mot généré pour le moment");
    } else {
        // Afficher les mots générés dans une liste scrollable
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), true, 
                          ImGuiWindowFlags_HorizontalScrollbar);
        
        for (size_t i = 0; i < generatedWords.size(); ++i) {
            ImGui::BulletText("%zu. %s", i + 1, generatedWords[i].c_str());
        }
        
        ImGui::EndChild();
    }
    
    ImGui::End();
}

void UI::renderMainWindow() {
    // Démarrer la frame ImGui
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    
    // Fenêtre principale avec statistiques
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(350, 120), ImGuiCond_FirstUseEver);
    ImGui::Begin("Générateur de Mots");
    ImGui::Text("🎲 Génération par syllabes");
    ImGui::Separator();
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Mots générés: %zu", generatedWords.size());
    ImGui::Checkbox("Info debug", &showDebugInfo);
    ImGui::End();
    
    // Panneaux
    ImGui::SetNextWindowPos(ImVec2(10, 140), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(350, 380), ImGuiCond_FirstUseEver);
    renderRulesPanel();
    
    ImGui::SetNextWindowPos(ImVec2(370, 140), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(420, 150), ImGuiCond_FirstUseEver);
    renderControlPanel();
    
    ImGui::SetNextWindowPos(ImVec2(370, 300), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(420, 220), ImGuiCond_FirstUseEver);
    renderResultsPanel();
    
    // Rendre ImGui
    ImGui::Render();
}

void UI::run() {
    SDL_Event event;
    
    while (running) {
        // Gérer les événements
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
                event.window.windowID == SDL_GetWindowID(window)) {
                running = false;
            }
        }
                    float temps= SDL_GetTicks()/2000.0f;    

                    int rouge = 128+(int)((cos(temps) + 1.0f) * 127.5f);
                    int vert = 128+(int)((cos(temps + 2.0f) + 1.0f) * 127.5f);
                    int bleu = 128+(int)((cos(temps + 4.0f) + 1.0f) * 127.5f);
        
                    
        
        // Effacer l'écran
        SDL_SetRenderDrawColor(renderer, rouge, vert, bleu, 255);
        SDL_RenderClear(renderer);
        
        // Rendre l'interface
        renderMainWindow();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        
        // Présenter
        SDL_RenderPresent(renderer);
    }
}

void UI::shutdown() {
    if (generator) {
        delete generator;
        generator = nullptr;
    }
    
    if (imguiContext) {
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext(imguiContext);
        imguiContext = nullptr;
    }
    
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    
    SDL_Quit();
}