#include "ui.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <cstring>
#include <iostream>
#include <cmath>

UI::UI() 
    : window(nullptr),
      renderer(nullptr),
      backgroundTexture(nullptr),
      selectedType(0),
      minSyllables(0),
      maxSyllables(0),
      wordCount(1),
      startWithCapital(true),
      allowSpaces(false),
      useDoubleConsonants(false),
      running(false),
      windowWidth(900),
      windowHeight(600)
{
    std::memset(prefix, 0, sizeof(prefix));
    std::memset(suffix, 0, sizeof(suffix));
}

UI::~UI() {
    cleanup();
}

bool UI::init() {
    // Initialiser SDL3
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Erreur SDL_Init: " << SDL_GetError() << std::endl;
        return false;
    }

    // Créer la fenêtre
    window = SDL_CreateWindow(
        "Générateur de Mots - ImGui + SDL3",
        windowWidth, windowHeight,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY
    );
    
    if (!window) {
        std::cerr << "Erreur SDL_CreateWindow: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    // Créer le renderer
    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "Erreur SDL_CreateRenderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    SDL_SetRenderVSync(renderer, 1);

    // Créer une texture pour le fond dégradé
    backgroundTexture = SDL_CreateTexture(renderer, 
                                         SDL_PIXELFORMAT_RGBA8888,
                                         SDL_TEXTUREACCESS_TARGET,
                                         windowWidth, windowHeight);
    
    if (backgroundTexture) {
        // Dessiner le dégradé sur la texture
        SDL_SetRenderTarget(renderer, backgroundTexture);
        
        for (int y = 0; y < windowHeight; y++) {
            float ratio = (float)y / windowHeight;
            
            // Dégradé du bleu foncé vers le violet/rose
            Uint8 r = (Uint8)(20 + ratio * 100);   // 20 -> 120
            Uint8 g = (Uint8)(30 + ratio * 50);    // 30 -> 80
            Uint8 b = (Uint8)(80 + ratio * 140);   // 80 -> 220
            
            SDL_SetRenderDrawColor(renderer, r, g, b, 255);
            SDL_RenderLine(renderer, 0, y, windowWidth, y);
        }
        
        // Remettre le rendu sur la fenêtre principale
        SDL_SetRenderTarget(renderer, nullptr);
        std::cout << "✓ Fond dégradé créé avec succès!" << std::endl;
    }

    // Initialiser ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Style ImGui
    ImGui::StyleColorsDark();
    
    // Personnaliser les couleurs
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.FrameRounding = 4.0f;
    style.GrabRounding = 4.0f;
    
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 0.95f);
    colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    
    // Initialiser les backends
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    running = true;
    return true;
}

void UI::generateWords() {
    GenerationRules rules;
    rules.type = static_cast<WordType>(selectedType);
    rules.minSyllables = minSyllables;
    rules.maxSyllables = maxSyllables;
    rules.startWithCapital = startWithCapital;
    rules.allowSpaces = allowSpaces;
    rules.useDoubleConsonants = useDoubleConsonants;
    rules.prefix = prefix;
    rules.suffix = suffix;
    
    generatedWords = generator.generateMultiple(rules, wordCount);
}

void UI::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL3_ProcessEvent(&event);
        
        if (event.type == SDL_EVENT_QUIT) {
            running = false;
        }
        
        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
            running = false;
        }
        
        if (event.type == SDL_EVENT_WINDOW_RESIZED) {
            SDL_GetWindowSize(window, &windowWidth, &windowHeight);
            
            // Recréer la texture de fond avec la nouvelle taille
            if (backgroundTexture) {
                SDL_DestroyTexture(backgroundTexture);
            }
            
            backgroundTexture = SDL_CreateTexture(renderer, 
                                                 SDL_PIXELFORMAT_RGBA8888,
                                                 SDL_TEXTUREACCESS_TARGET,
                                                 windowWidth, windowHeight);
            
            if (backgroundTexture) {
                SDL_SetRenderTarget(renderer, backgroundTexture);
                
                for (int y = 0; y < windowHeight; y++) {
                    float ratio = (float)y / windowHeight;
                    Uint8 r = (Uint8)(20 + ratio * 100);
                    Uint8 g = (Uint8)(30 + ratio * 50);
                    Uint8 b = (Uint8)(80 + ratio * 140);
                    
                    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
                    SDL_RenderLine(renderer, 0, y, windowWidth, y);
                }
                
                SDL_SetRenderTarget(renderer, nullptr);
            }
        }
    }
}

void UI::render() {
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    // Fenêtre principale centrée
    float mainWindowWidth = 600.0f;
    float mainWindowHeight = 700.0f;
    
    ImGui::SetNextWindowPos(
        ImVec2((windowWidth - mainWindowWidth) * 0.5f, (windowHeight - mainWindowHeight) * 0.3f - 80),
        ImGuiCond_Always
    );
    ImGui::SetNextWindowSize(ImVec2(mainWindowWidth, mainWindowHeight), ImGuiCond_Always);
    
    ImGui::Begin("🎲 Générateur de Mots", nullptr, 
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    
    // ===== TYPE DE MOT =====
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 1.0f, 1.0f));
    ImGui::Text("📝 Type de mot");
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();
    
    const char* types[] = { "Prénom", "Ville", "Pays", "Animal", "Objet" };
    ImGui::SetNextItemWidth(-1);
    ImGui::Combo("##type", &selectedType, types, IM_ARRAYSIZE(types));
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // ===== SYLLABES =====
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 1.0f, 1.0f));
    ImGui::Text("🔢 Nombre de syllabes");
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();
    
    ImGui::Text("Minimum:");
    ImGui::SameLine(120);
    ImGui::SetNextItemWidth(-1);
    ImGui::SliderInt("##min", &minSyllables, 1, 5);
    
    ImGui::Text("Maximum:");
    ImGui::SameLine(120);
    ImGui::SetNextItemWidth(-1);
    ImGui::SliderInt("##max", &maxSyllables, minSyllables, 6);
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // ===== QUANTITÉ =====
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 1.0f, 1.0f));
    ImGui::Text("🎯 Quantité à générer");
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();
    
    ImGui::SetNextItemWidth(-1);
    ImGui::SliderInt("##count", &wordCount, 1, 50);
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // ===== OPTIONS =====
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 1.0f, 1.0f));
    ImGui::Text("⚙️ Options");
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();
    
    ImGui::Checkbox("Majuscule au début", &startWithCapital);
    ImGui::Checkbox("Autoriser les espaces", &allowSpaces);
    ImGui::Checkbox("Consonnes doubles", &useDoubleConsonants);
    
    ImGui::Spacing();
    
    ImGui::Text("Préfixe:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(150);
    ImGui::InputText("##prefix", prefix, sizeof(prefix));
    
    ImGui::Text("Suffixe:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(150);
    ImGui::InputText("##suffix", suffix, sizeof(suffix));
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // ===== BOUTON GÉNÉRER =====
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.15f, 0.5f, 0.15f, 1.0f));
    
    if (ImGui::Button("🎲 GÉNÉRER", ImVec2(-1, 45))) {
        generateWords();
    }
    
    ImGui::PopStyleColor(3);
    
    ImGui::End();

    // ===== FENÊTRE DES RÉSULTATS =====
    if (!generatedWords.empty()) {
        float resultsWindowWidth = 480.0f;
        float resultsWindowHeight = 380.0f;
        
        ImGui::SetNextWindowPos(
            ImVec2((windowWidth - resultsWindowWidth) * 0.5f, 
                   (windowHeight - mainWindowHeight) * 0.5f + mainWindowHeight - 60),
            ImGuiCond_Always
        );
        ImGui::SetNextWindowSize(ImVec2(resultsWindowWidth, resultsWindowHeight), ImGuiCond_Always);
        
        ImGui::Begin("📋 Résultats", nullptr, 
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
        
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 1.0f, 1.0f));
        ImGui::Text("✨ %d mots générés:", (int)generatedWords.size());
        ImGui::PopStyleColor();
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -35), true);
        
        int columns = 3;
        ImGui::Columns(columns, "results", false);
        
        for (size_t i = 0; i < generatedWords.size(); i++) {
            if (i % 2 == 0) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
            }
            
            ImGui::Selectable(generatedWords[i].c_str());
            
            ImGui::PopStyleColor();
            
            if ((i + 1) % columns != 0) {
                ImGui::NextColumn();
            }
        }
        
        ImGui::Columns(1);
        ImGui::EndChild();
        
        ImGui::Spacing();
        if (ImGui::Button("📋 Copier tout", ImVec2(-1, 0))) {
            std::string allWords;
            for (const auto& word : generatedWords) {
                allWords += word + "\n";
            }
            ImGui::SetClipboardText(allWords.c_str());
        }
        
        ImGui::End();
    }

    // Rendu
    ImGui::Render();
    
    // Effacer l'écran et afficher le fond dégradé
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    // Afficher la texture de fond
    if (backgroundTexture) {
        SDL_FRect destRect = {0, 0, (float)windowWidth, (float)windowHeight};
        SDL_RenderTexture(renderer, backgroundTexture, nullptr, &destRect);
    }
    
    // Rendu ImGui par-dessus
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
}

void UI::run() {
    while (running) {
        handleEvents();
        render();
    }
}

void UI::cleanup() {
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }
    
    if (renderer) {
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
        
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    
    SDL_Quit();
}