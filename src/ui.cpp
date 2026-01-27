#include "ui.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <cstring>
#include <iostream>

// CONSTRUCTEUR - MODIFIÉ (audio ajouté)

UI::UI() 
    : window(nullptr),
      renderer(nullptr),
      backgroundTexture(nullptr),
      audioDeviceID(0),           
      wavBuffer(nullptr),         
      wavLength(0),               
      selectedType(0),
      minSyllables(2),
      maxSyllables(3),
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


// FONCTION init() - MODIFIÉE (audio ajouté)

bool UI::init() {
    // Initialiser SDL3 avec VIDEO ET AUDIO
    // ⚠️ MODIFIÉ : Ajout de | SDL_INIT_AUDIO
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
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

    // ============================================
    // ✨ NOUVEAU : CHARGER LE FICHIER AUDIO
    // ============================================
    if (!SDL_LoadWAV("assets/click.wav", &audioSpec, &wavBuffer, &wavLength)) {
        std::cerr << "⚠️ Impossible de charger click.wav: " << SDL_GetError() << std::endl;
        std::cout << "Le programme continuera sans son." << std::endl;
    } else {
        std::cout << "✓ Fichier WAV chargé avec succès!" << std::endl;
        
        // Ouvrir le périphérique audio
        audioDeviceID = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audioSpec);
        
        if (audioDeviceID == 0) {
            std::cerr << "❌ Erreur SDL_OpenAudioDevice: " << SDL_GetError() << std::endl;
            SDL_free(wavBuffer);
            wavBuffer = nullptr;
        } else {
            std::cout << "✓ Périphérique audio ouvert avec succès!" << std::endl;
        }
    }
    // ============================================

    // Charger l'image de fond
    SDL_Surface* surface = SDL_LoadBMP("E:\\niveau1\\game programming\\c++\\essai3\\assets\\image1.bmp");
    if (surface) {
        backgroundTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);
        if (!backgroundTexture) {
            std::cerr << "Erreur création texture: " << SDL_GetError() << std::endl;
        }
    } else {
        std::cerr << "Impossible de charger l'image: " << SDL_GetError() << std::endl;
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

// ============================================
// ✨ NOUVELLE FONCTION : playSound()
// ============================================
void UI::playSound() {
    // Vérifier que l'audio est disponible
    if (audioDeviceID != 0 && wavBuffer != nullptr) {
        // Créer un stream audio
        SDL_AudioStream* stream = SDL_CreateAudioStream(&audioSpec, &audioSpec);
        
        if (stream) {
            // Charger les données audio dans le stream
            SDL_PutAudioStreamData(stream, wavBuffer, wavLength);
            
            // Finaliser le stream
            SDL_FlushAudioStream(stream);
            
            // Lier le stream au périphérique
            SDL_BindAudioStream(audioDeviceID, stream);
            
            // JOUER LE SON
            SDL_ResumeAudioDevice(audioDeviceID);
        }
    }
}
// ============================================

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
        }
    }
}

// ============================================
// FONCTION render() - MODIFIÉE (appel audio ajouté)
// ============================================
void UI::render() {
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    // Fenêtre principale centrée
    float mainWindowWidth = 480.0f;
    float mainWindowHeight = 560.0f;
    
    ImGui::SetNextWindowPos(
        ImVec2((windowWidth - mainWindowWidth) * 0.5f, (windowHeight - mainWindowHeight) * 0.5f - 80),
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
    
    // ===== BOUTON GÉNÉRER - MODIFIÉ =====
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.15f, 0.5f, 0.15f, 1.0f));
    
    if (ImGui::Button("🎲 GÉNÉRER", ImVec2(-1, 45))) {
        playSound();        // ← ✨ NOUVELLE LIGNE : Jouer le son
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
        
        // ⚠️ BONUS : Ajouter son aussi sur le bouton "Copier"
        if (ImGui::Button("📋 Copier tout", ImVec2(-1, 0))) {
            playSound();    // ← ✨ BONUS : Son sur copie aussi
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
    SDL_SetRenderDrawColor(renderer, 0, 128, 255, 255);
    SDL_RenderClear(renderer);
    
    // Dessiner l'image de fond
    if (backgroundTexture) {
        SDL_FRect destRect = {0, 0, (float)windowWidth, (float)windowHeight};
        SDL_RenderTexture(renderer, backgroundTexture, nullptr, &destRect);
    }
    
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
}

void UI::run() {
    while (running) {
        handleEvents();
        render();
    }
}

// ============================================
// FONCTION cleanup() - MODIFIÉE (nettoyage audio ajouté)
// ============================================
void UI::cleanup() {
    // ============================================
    // ✨ NOUVEAU : NETTOYER L'AUDIO EN PREMIER
    // ============================================
    if (audioDeviceID != 0) {
        SDL_CloseAudioDevice(audioDeviceID);
        audioDeviceID = 0;
        std::cout << "✓ Périphérique audio fermé" << std::endl;
    }
    
    if (wavBuffer) {
        SDL_free(wavBuffer);
        wavBuffer = nullptr;
        std::cout << "✓ Buffer audio libéré" << std::endl;
    }
    // ============================================
    
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