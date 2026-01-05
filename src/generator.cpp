#include "generator.h"
#include <chrono>
#include <algorithm>
#include <cctype>


WordGenerator::WordGenerator() {
    // Initialiser le générateur aléatoire avec le temps actuel
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    rng.seed(static_cast<unsigned int>(seed));
    
    initializeSyllables();
}

void WordGenerator::initializeSyllables() {
    // Consonnes simples
    consonants = {
        "b", "c", "d", "f", "g", "h", "j", "k", "l", "m",
        "n", "p", "r", "s", "t", "v", "w", "x", "z",
        "ch", "ph", "th", "qu", "sh"
    };
    
    // Voyelles et diphtongues
    vowels = {
        "a", "e", "i", "o", "u", "y",
        "ai", "au", "ei", "eu", "ou", "oi",
        "an", "en", "in", "on", "un"
    };
    
    // Consonnes doubles
    doubleConsonants = {
        "ll", "mm", "nn", "pp", "rr", "ss", "tt", "ff"
    };
    
    // Syllabes pour prénoms (sons français courants)
    nameSyllables = {
        "ma", "ri", "an", "lu", "ca", "so", "phi", "je", "pierre",
        "alex", "and", "re", "tho", "mas", "nic", "o", "las",
        "max", "ime", "lou", "is", "ga", "bri", "el", "ar", "thur",
        "pau", "l", "vic", "tor", "jules", "hu", "go", "emma",
        "chlo", "é", "ju", "lie", "lau", "ra", "sa", "rah",
        "cam", "ille", "lé", "a", "ma", "non", "cla", "ra"
    };
    
    // Syllabes pour villes (suffixes typiques français)
    citySyllables = {
        "par", "is", "lyon", "mar", "seille", "tou", "louse",
        "bor", "deaux", "nan", "tes", "stras", "bourg", "mon",
        "pel", "lier", "ren", "nes", "nice", "di", "jon",
        "ville", "sur", "le", "saint", "la", "les",
        "mont", "val", "pont", "fort", "cha", "teau"
    };
    
    // Syllabes pour pays
    countrySyllables = {
        "fran", "ce", "al", "le", "magne", "es", "pagne",
        "i", "ta", "lie", "por", "tu", "gal", "bel", "gique",
        "suisse", "au", "triche", "po", "logne", "sue", "de",
        "nor", "vege", "da", "ne", "mark", "fin", "lande",
        "gre", "ce", "ir", "lande", "is", "lande", "land", "ie"
    };
    
    // Syllabes pour animaux
    animalSyllables = {
        "li", "on", "ti", "gre", "é", "lé", "phant", "gi",
        "rafe", "zè", "bre", "pan", "da", "ko", "a", "la",
        "kan", "gou", "rou", "dau", "phin", "ba", "leine",
        "re", "quin", "ai", "gle", "fau", "con", "per", "ro",
        "quet", "pin", "gouin", "ours", "loup", "re", "nard"
    };
    
    // Syllabes pour objets
    objectSyllables = {
        "ta", "ble", "chaise", "lam", "pe", "li", "vre",
        "sty", "lo", "or", "di", "na", "teur", "té", "lé",
        "phone", "cla", "vier", "sou", "ris", "é", "cran",
        "ta", "bleau", "hor", "loge", "mi", "roir", "fe",
        "nê", "tre", "por", "te", "ta", "pis", "cous", "sin"
    };
}

std::string WordGenerator::generateFromSyllables(const std::vector<std::string>& syllables, int count) {
    if (syllables.empty() || count <= 0) {
        return "";
    }
    
    std::string result;
    std::uniform_int_distribution<size_t> dist(0, syllables.size() - 1);
    
    for (int i = 0; i < count; ++i) {
        result += syllables[dist(rng)];
    }
    
    return result;
}

std::string WordGenerator::generateGenericWord(int syllableCount, bool allowDoubleConsonants) {
    if (syllableCount <= 0) {
        return "";
    }
    
    std::string result;
    std::uniform_int_distribution<size_t> consonantDist(0, consonants.size() - 1);
    std::uniform_int_distribution<size_t> vowelDist(0, vowels.size() - 1);
    std::uniform_int_distribution<size_t> doubleDist(0, doubleConsonants.size() - 1);
    std::uniform_int_distribution<int> choiceDist(0, 99);
    
    for (int i = 0; i < syllableCount; ++i) {
        // Alterner consonne-voyelle
        // Consonne (ou double consonne parfois)
        if (allowDoubleConsonants && choiceDist(rng) < 15) {
            result += doubleConsonants[doubleDist(rng)];
        } else {
            result += consonants[consonantDist(rng)];
        }
        
        // Voyelle
        result += vowels[vowelDist(rng)];
        
        // Parfois ajouter une consonne finale
        if (i == syllableCount - 1 && choiceDist(rng) < 40) {
            result += consonants[consonantDist(rng)];
        }
    }
    
    return result;
}

std::string WordGenerator::applyRules(const std::string& word, const GenerationRules& rules) {
    std::string result = word;
    
    // Gérer les espaces
    if (!rules.allowSpaces) {
        result.erase(std::remove(result.begin(), result.end(), ' '), result.end());
        result.erase(std::remove(result.begin(), result.end(), '-'), result.end());
    }
    
    // Appliquer la capitalisation
    if (!result.empty()) {
        if (rules.startWithCapital) {
            result[0] = std::toupper(result[0]);
            // Mettre en minuscule le reste
            for (size_t i = 1; i < result.length(); ++i) {
                result[i] = std::tolower(result[i]);
            }
        } else {
            for (size_t i = 0; i < result.length(); ++i) {
                result[i] = std::tolower(result[i]);
            }
        }
    }
    
    // Ajouter préfixe et suffixe
    result = rules.prefix + result + rules.suffix;
    
    return result;
}

std::string WordGenerator::generate(const GenerationRules& rules) {
    // Déterminer le nombre de syllabes
    std::uniform_int_distribution<int> syllableDist(rules.minSyllables, rules.maxSyllables);
    int syllableCount = syllableDist(rng);
    
    std::string word;
    
    // Générer selon le type
    switch (rules.type) {
        case WordType::NAME:
            word = generateFromSyllables(nameSyllables, syllableCount);
            break;
        case WordType::CITY:
            word = generateFromSyllables(citySyllables, syllableCount);
            break;
        case WordType::COUNTRY:
            word = generateFromSyllables(countrySyllables, syllableCount);
            break;
        case WordType::ANIMAL:
            word = generateFromSyllables(animalSyllables, syllableCount);
            break;
        case WordType::OBJECT:
            word = generateFromSyllables(objectSyllables, syllableCount);
            break;
    }
    
    // Si le mot est vide ou trop court, générer un mot générique
    if (word.empty() || word.length() < 2) {
        word = generateGenericWord(syllableCount, rules.useDoubleConsonants);
    }
    
    // Appliquer les règles
    return applyRules(word, rules);
}

std::vector<std::string> WordGenerator::generateMultiple(const GenerationRules& rules, int count) {
    std::vector<std::string> result;
    result.reserve(count);
    
    for (int i = 0; i < count; ++i) {
        result.push_back(generate(rules));
    }
    
    return result;
}

const char* WordGenerator::getWordTypeName(WordType type) const {
    switch (type) {
        case WordType::NAME: return "Prénom";
        case WordType::CITY: return "Ville";
        case WordType::COUNTRY: return "Pays";
        case WordType::ANIMAL: return "Animal";
        case WordType::OBJECT: return "Objet";
        default: return "Inconnu";
    }
}