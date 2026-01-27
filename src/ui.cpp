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
      audioDeviceID(0),
      audioSpec{},
      wavBuffer(nullptr),
      wavLength(0),
      selectedType(0),
      minSyllables(1),
      maxSyllables(2),
      wordCount(1),
      startWithCapital(true),
      allowSpaces(false),
      useDoubleConsonants(false),
      running(false),
      windowWidth(900),
      windowHeight(600),
      currentThemeIndex(0),
      lastThemeChange(0),
      transitionProgress(0.0f),
      isTransitioning(false)
{
    std::memset(prefix, 0, sizeof(prefix));
    std::memset(suffix, 0, sizeof(suffix));
    
    // Initialiser les thèmes
    themes = {
        // Thème 1: Océan Profond (Bleu foncé → Violet)
        {
            "Océan Profond",
            {20, 30, 80, 255},      // gradientStart
            {120, 80, 220, 255},    // gradientEnd
            ImVec4(0.26f, 0.59f, 0.98f, 1.0f),  // accentColor
            ImVec4(0.4f, 0.8f, 1.0f, 1.0f),     // textHighlight
            ImVec4(0.13f, 0.14f, 0.15f, 0.95f)  // windowBg
        },
        
        // Thème 2: Coucher de Soleil (Orange → Rose)
        {
            "Coucher de Soleil",
            {255, 140, 60, 255},    // gradientStart
            {255, 70, 130, 255},    // gradientEnd
            ImVec4(1.0f, 0.5f, 0.2f, 1.0f),     // accentColor
            ImVec4(1.0f, 0.7f, 0.3f, 1.0f),     // textHighlight
            ImVec4(0.15f, 0.12f, 0.13f, 0.95f)  // windowBg
        },
        
        // Thème 3: Forêt Mystique (Vert foncé → Cyan)
        {
            "Forêt Mystique",
            {20, 80, 60, 255},      // gradientStart
            {60, 180, 160, 255},    // gradientEnd
            ImVec4(0.2f, 0.8f, 0.6f, 1.0f),     // accentColor
            ImVec4(0.4f, 1.0f, 0.8f, 1.0f),     // textHighlight
            ImVec4(0.12f, 0.15f, 0.14f, 0.95f)  // windowBg
        },
        
        // Thème 4: Aurore Boréale (Violet → Vert lime)
        {
            "Aurore Boréale",
            {80, 40, 120, 255},     // gradientStart
            {100, 220, 140, 255},   // gradientEnd
            ImVec4(0.6f, 0.3f, 0.9f, 1.0f),     // accentColor
            ImVec4(0.7f, 0.4f, 1.0f, 1.0f),     // textHighlight
            ImVec4(0.14f, 0.13f, 0.15f, 0.95f)  // windowBg
        },
        
        // Thème 5: Feu et Glace (Rouge → Bleu clair)
        {
            "Feu et Glace",
            {200, 40, 40, 255},     // gradientStart
            {80, 160, 240, 255},    // gradientEnd
            ImVec4(0.9f, 0.3f, 0.3f, 1.0f),     // accentColor
            ImVec4(1.0f, 0.5f, 0.5f, 1.0f),     // textHighlight
            ImVec4(0.15f, 0.13f, 0.14f, 0.95f)  // windowBg
        },
        
        // Thème 6: Galaxie (Violet profond → Rose magenta)
        {
            "Galaxie",
            {40, 20, 80, 255},      // gradientStart
            {180, 60, 160, 255},    // gradientEnd
            ImVec4(0.7f, 0.3f, 0.8f, 1.0f),     // accentColor
            ImVec4(0.9f, 0.5f, 1.0f, 1.0f),     // textHighlight
            ImVec4(0.13f, 0.12f, 0.15f, 0.95f)  // windowBg
        }
    };
    
    currentTheme = themes[0];
}

UI::~UI() {
    cleanup();
}

bool UI::init() {
    // Initialiser SDL3 avec VIDEO et AUDIO
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

    // Créer une texture pour le fond dégradé
    backgroundTexture = SDL_CreateTexture(renderer, 
                                         SDL_PIXELFORMAT_RGBA8888,
                                         SDL_TEXTUREACCESS_TARGET,
                                         windowWidth, windowHeight);
    
    updateBackground();

    // Charger le fichier WAV
    if (!SDL_LoadWAV("assets/click.wav", &audioSpec, &wavBuffer, &wavLength)) {
        std::cerr << "Avertissement: Impossible de charger click.wav: " << SDL_GetError() << std::endl;
        std::cout << "Le programme continuera sans son." << std::endl;
    } else {
        std::cout << "✓ Fichier WAV chargé avec succès!" << std::endl;
        
        // Ouvrir le périphérique audio
        audioDeviceID = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audioSpec);
        if (audioDeviceID == 0) {
            std::cerr << "Erreur SDL_OpenAudioDevice: " << SDL_GetError() << std::endl;
            SDL_free(wavBuffer);
            wavBuffer = nullptr;
        } else {
            std::cout << "✓ Périphérique audio ouvert avec succès!" << std::endl;
        }
    }

    // Initialiser ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Style ImGui de base
    ImGui::StyleColorsDark();
    
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.FrameRounding = 4.0f;
    style.GrabRounding = 4.0f;
    
    // Appliquer le thème initial
    applyTheme(currentTheme);
    
    // Initialiser les backends
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    // Initialiser le timer pour les thèmes
    lastThemeChange = SDL_GetTicks();

    running = true;
    return true;
}

void UI::updateBackground() {
    if (!backgroundTexture) return;
    
    SDL_SetRenderTarget(renderer, backgroundTexture);
    
    Theme& theme = isTransitioning ? previousTheme : currentTheme;
    
    // Si on est en transition, interpoler entre les deux thèmes
    for (int y = 0; y < windowHeight; y++) {
        float ratio = (float)y / windowHeight;
        
        Uint8 r, g, b;
        
        if (isTransitioning) {
            // Interpoler entre previousTheme et currentTheme
            SDL_Color startPrev = previousTheme.gradientStart;
            SDL_Color endPrev = previousTheme.gradientEnd;
            SDL_Color startCurr = currentTheme.gradientStart;
            SDL_Color endCurr = currentTheme.gradientEnd;
            
            // Calculer la couleur du dégradé pour previousTheme
            Uint8 rPrev = (Uint8)(startPrev.r + ratio * (endPrev.r - startPrev.r));
            Uint8 gPrev = (Uint8)(startPrev.g + ratio * (endPrev.g - startPrev.g));
            Uint8 bPrev = (Uint8)(startPrev.b + ratio * (endPrev.b - startPrev.b));
            
            // Calculer la couleur du dégradé pour currentTheme
            Uint8 rCurr = (Uint8)(startCurr.r + ratio * (endCurr.r - startCurr.r));
            Uint8 gCurr = (Uint8)(startCurr.g + ratio * (endCurr.g - startCurr.g));
            Uint8 bCurr = (Uint8)(startCurr.b + ratio * (endCurr.b - startCurr.b));
            
            // Interpoler entre les deux
            r = (Uint8)(rPrev + transitionProgress * (rCurr - rPrev));
            g = (Uint8)(gPrev + transitionProgress * (gCurr - gPrev));
            b = (Uint8)(bPrev + transitionProgress * (bCurr - bPrev));
        } else {
            // Dégradé normal sans transition
            r = (Uint8)(theme.gradientStart.r + ratio * (theme.gradientEnd.r - theme.gradientStart.r));
            g = (Uint8)(theme.gradientStart.g + ratio * (theme.gradientEnd.g - theme.gradientStart.g));
            b = (Uint8)(theme.gradientStart.b + ratio * (theme.gradientEnd.b - theme.gradientStart.b));
        }
        
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderLine(renderer, 0, y, windowWidth, y);
    }
    
    SDL_SetRenderTarget(renderer, nullptr);
}

void UI::applyTheme(const Theme& theme) {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;
    
    // Appliquer les couleurs du thème
    colors[ImGuiCol_WindowBg] = theme.windowBg;
    colors[ImGuiCol_Header] = ImVec4(theme.accentColor.x, theme.accentColor.y, theme.accentColor.z, 0.31f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(theme.accentColor.x, theme.accentColor.y, theme.accentColor.z, 0.80f);
    colors[ImGuiCol_HeaderActive] = theme.accentColor;
    colors[ImGuiCol_Button] = ImVec4(theme.accentColor.x, theme.accentColor.y, theme.accentColor.z, 0.40f);
    colors[ImGuiCol_ButtonHovered] = theme.accentColor;
    colors[ImGuiCol_ButtonActive] = ImVec4(theme.accentColor.x * 0.8f, theme.accentColor.y * 0.8f, theme.accentColor.z * 0.8f, 1.0f);
    colors[ImGuiCol_SliderGrab] = ImVec4(theme.accentColor.x * 0.9f, theme.accentColor.y * 0.9f, theme.accentColor.z * 0.9f, 1.0f);
    colors[ImGuiCol_SliderGrabActive] = theme.accentColor;
}

void UI::updateTheme() {
    Uint64 currentTime = SDL_GetTicks();
    
    // Vérifier si on doit changer de thème
    if (currentTime - lastThemeChange >= THEME_CHANGE_INTERVAL) {
        // Démarrer une transition
        if (!isTransitioning) {
            isTransitioning = true;
            transitionProgress = 0.0f;
            previousTheme = currentTheme;
            
            // Passer au thème suivant
            currentThemeIndex = (currentThemeIndex + 1) % themes.size();
            currentTheme = themes[currentThemeIndex];
            
            std::cout << "🎨 Changement de thème vers: " << currentTheme.name << std::endl;
        }
    }
    
    // Animer la transition
    if (isTransitioning) {
        transitionProgress += 0.02f; // Vitesse de transition (ajustable)
        
        if (transitionProgress >= 1.0f) {
            transitionProgress = 1.0f;
            isTransitioning = false;
            lastThemeChange = currentTime;
            
            // Appliquer le nouveau thème
            applyTheme(currentTheme);
        }
        
        // Mettre à jour le fond avec la transition
        updateBackground();
    }
}

void UI::playSound() {
    if (audioDeviceID != 0 && wavBuffer != nullptr) {
        SDL_AudioStream* stream = SDL_CreateAudioStream(&audioSpec, &audioSpec);
        if (stream) {
            SDL_PutAudioStreamData(stream, wavBuffer, wavLength);
            SDL_FlushAudioStream(stream);
            SDL_BindAudioStream(audioDeviceID, stream);
            SDL_ResumeAudioDevice(audioDeviceID);
        }
    }
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
            
            updateBackground();
        }
    }
}

void UI::render() {
    // Mettre à jour le système de thèmes
    updateTheme();
    
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
    
    // Afficher le nom du thème actuel dans le titre
    std::string windowTitle = "🎲 Générateur de Mots - Thème: " + currentTheme.name;
    ImGui::Begin(windowTitle.c_str(), nullptr, 
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    
    // ===== TYPE DE MOT =====
    ImGui::PushStyleColor(ImGuiCol_Text, currentTheme.textHighlight);
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
    ImGui::PushStyleColor(ImGuiCol_Text, currentTheme.textHighlight);
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
    ImGui::PushStyleColor(ImGuiCol_Text, currentTheme.textHighlight);
    ImGui::Text("🎯 Quantité à générer");
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();
    
    ImGui::SetNextItemWidth(-1);
    ImGui::SliderInt("##count", &wordCount, 1, 50);
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // ===== OPTIONS =====
    ImGui::PushStyleColor(ImGuiCol_Text, currentTheme.textHighlight);
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
        playSound();
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
        
        ImGui::PushStyleColor(ImGuiCol_Text, currentTheme.textHighlight);
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
            playSound();
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
    // Libérer les ressources audio
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
    
    // Libérer la texture de fond
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }
    
    // Libérer ImGui et SDL
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
    std::cout << "✓ Nettoyage terminé" << std::endl;
}