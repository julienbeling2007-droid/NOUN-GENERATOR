#include "generator.h"
#include <algorithm>
#include <chrono>
#include <cctype>

WordGenerator::WordGenerator() {
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    rng.seed(static_cast<unsigned int>(seed));
    
    initializeSyllables();
    initializeStructuredSyllables();
}

void WordGenerator::initializeSyllables() {
    consonants = {
        "b", "c", "d", "f", "g", "h", "j", "k", "l", "m",
        "n", "p", "r", "s", "t", "v", "w", "x", "z",
        "ch", "ph", "th", "qu", "sh"
    };
    
    vowels = {
        "a", "e", "i", "o", "u", "y",
        "ai", "au", "ei", "eu", "ou", "oi",
        "an", "en", "in", "on", "un"
    };
    
    doubleConsonants = {
        "ll", "mm", "nn", "pp", "rr", "ss", "tt", "ff"
    };
}

void WordGenerator::initializeStructuredSyllables() {
    // ===== PRÉNOMS RÉALISTES =====
    nameSyllableSet.start = {
        "Alex", "And", "Ant", "Ben", "Bru", "Carl", "Chris", "Claud", "Dan", "Dav",
        "Ed", "Em", "Eth", "Fr", "Gabr", "Geo", "Henr", "Is", "Jack", "Jam",
        "Jean", "Jul", "Kev", "Laur", "Leo", "Luc", "Marc", "Mar", "Max", "Mich",
        "Nic", "Ol", "Patr", "Paul", "Phil", "Raph", "Rich", "Rob", "Sam", "Seb",
        "Steph", "Thom", "Tim", "Vict", "Vinc", "Will", "Xav", "Zach"
    };
    
    nameSyllableSet.middle = {
        "an", "ar", "bert", "el", "en", "er", "i", "ian", "iel",
        "il", "in", "is", "li", "o", "ol", "on", "ri", "tin"
    };
    
    nameSyllableSet.end = {
        "ael", "ander", "andre", "ard", "as", "bert", "ce", "dre", "el",
        "elle", "en", "ence", "ent", "er", "es", "et", "ette", "iana",
        "ien", "ier", "ine", "io", "is", "las", "le", "lie", "line",
        "lius", "lo", "lotte", "ly", "man", "mie", "na", "nce", "ne",
        "niel", "nis", "o", "ola", "ole", "ome", "on", "otte", "ric",
        "rie", "ry", "son", "tan", "teur", "tien", "tine", "ton", "us"
    };
    
    // ===== VILLES RÉALISTES =====
    citySyllableSet.start = {
        "Beau", "Bel", "Ber", "Bor", "Bour", "Brest", "Cler", "Fon", "Gre", "La",
        "Le", "Lil", "Lyon", "Mar", "Mont", "Nan", "Nice", "Or", "Pa", "Poi",
        "Ren", "Rou", "Saint", "Stras", "Tou", "Tours", "Val", "Ver", "Vil"
    };
    
    citySyllableSet.middle = {
        "belle", "fort", "land", "mont", "pont", "ville"
    };
    
    citySyllableSet.end = {
        "ais", "ay", "bourg", "court", "cy", "deaux", "den", "dieu", "fort",
        "gny", "heim", "leans", "les", "lin", "lon", "louse", "me", "mer",
        "mes", "mont", "nay", "nes", "ney", "ois", "pel", "pont", "rac",
        "ret", "ris", "ron", "sanne", "seille", "tan", "tiers", "tin", "ton",
        "val", "var", "vaux", "vic", "ville", "y"
    };
    
    // ===== PAYS RÉALISTES =====
    countrySyllableSet.start = {
        "Al", "And", "Ang", "Ar", "Aus", "Bel", "Bos", "Bul", "Ca", "Chi",
        "Croa", "Da", "Egyp", "Es", "Fin", "Fra", "Ger", "Grè", "Hon", "In",
        "Ir", "Is", "Ita", "Ja", "Ko", "Lux", "Ma", "Mexi", "Mol", "Mon",
        "Néer", "Nigé", "Nor", "Polo", "Portu", "Rou", "Rus", "Slo", "Sué",
        "Suis", "Thaï", "Tur", "Ukrai", "Vene", "Viet"
    };
    
    countrySyllableSet.middle = {
        "ba", "ga", "la", "ma", "na", "ra", "ta", "va"
    };
    
    countrySyllableSet.end = {
        "bie", "ca", "ce", "da", "de", "do", "dor", "ga", "gal", "gne",
        "grie", "ie", "ien", "ique", "la", "land", "lande", "le", "li",
        "lie", "ma", "manie", "mark", "na", "nam", "ne", "ni", "nie",
        "nésie", "pa", "pagne", "pie", "que", "ra", "rée", "ria", "rie",
        "se", "si", "sie", "stan", "sue", "tan", "talie", "tie", "ton",
        "tuanie", "uay", "uie", "va", "ven", "vie", "ya", "zil"
    };
    
    // ===== ANIMAUX RÉALISTES =====
    animalSyllableSet.start = {
        "A", "Ai", "Al", "Anti", "Bal", "Bé", "Bou", "Cha", "Che", "Chi",
        "Co", "Cro", "Dau", "Élé", "Fau", "Gaz", "Gi", "Gue", "Hé", "Hi",
        "Hy", "Jam", "Ka", "Koa", "Lé", "Li", "Loup", "Mou", "Ours", "Pan",
        "Pa", "Per", "Pin", "Po", "Re", "Rhi", "San", "Ser", "Si", "Ti",
        "Tor", "Vau", "Zè"
    };
    
    animalSyllableSet.middle = {
        "co", "go", "la", "le", "lo", "ma", "nard", "no", "pard", "po", "quin"
    };
    
    animalSyllableSet.end = {
        "ble", "bou", "brion", "bu", "chat", "cle", "co", "con", "cro", "deau",
        "dile", "don", "dor", "dre", "eau", "elle", "fan", "gou", "gouin", "gre",
        "ille", "jon", "lard", "le", "leon", "lion", "lope", "lou", "loup",
        "man", "meau", "mine", "mite", "moth", "mouche", "mure", "nard", "nelle",
        "noc", "pard", "phin", "phant", "pi", "pon", "porc", "pot", "poule",
        "quin", "ra", "rafe", "rain", "rat", "reau", "ret", "rinthe", "ron",
        "rosse", "rue", "rus", "seau", "son", "tau", "teau", "tigre", "ton",
        "tour", "tue", "vache", "veau", "vre", "zard", "zon"
    };
    
    // ===== OBJETS RÉALISTES =====
    objectSyllableSet.start = {
        "Ar", "As", "Ba", "Bal", "Bou", "Bro", "Ca", "Chai", "Char", "Cla",
        "Cou", "Cra", "Cul", "Cui", "É", "Four", "Gla", "Hor", "La", "Lam",
        "Li", "Mar", "Mi", "Or", "Pa", "Pla", "Poi", "Ri", "Sa", "Sty",
        "Ta", "Té", "Ti", "Va", "Ver", "Voi"
    };
    
    objectSyllableSet.middle = {
        "ble", "chette", "lier", "lou", "neau", "reau", "seau", "teau"
    };
    
    objectSyllableSet.end = {
        "ble", "bleau", "bol", "bon", "cage", "cal", "carte", "chaise", "chette",
        "chet", "chon", "chure", "clé", "co", "cou", "coupe", "deau", "din",
        "doir", "don", "fon", "gnon", "lard", "le", "let", "lier", "lin",
        "lo", "lon", "lope", "lot", "lume", "meau", "mer", "moire", "neau",
        "nette", "nier", "nière", "pe", "peau", "pier", "plat", "po", "pon",
        "pot", "que", "ra", "reau", "ret", "rin", "roir", "ron", "sa",
        "seau", "sel", "sier", "sin", "sse", "ssin", "ta", "teau", "te",
        "tier", "tin", "to", "ton", "ture", "veau", "vet", "vier", "vre"
    };
}

std::string WordGenerator::generateFromStructuredSyllables(const SyllableSet& syllableSet, int syllableCount) {
    if (syllableCount <= 0) {
        return "";
    }
    
    std::string result;
    
    if (!syllableSet.start.empty()) {
        std::uniform_int_distribution<size_t> startDist(0, syllableSet.start.size() - 1);
        result += syllableSet.start[startDist(rng)];
    }
    
    if (syllableCount > 2 && !syllableSet.middle.empty()) {
        std::uniform_int_distribution<size_t> middleDist(0, syllableSet.middle.size() - 1);
        for (int i = 1; i < syllableCount - 1; ++i) {
            result += syllableSet.middle[middleDist(rng)];
        }
    }
    
    if (syllableCount > 1 && !syllableSet.end.empty()) {
        std::uniform_int_distribution<size_t> endDist(0, syllableSet.end.size() - 1);
        result += syllableSet.end[endDist(rng)];
    } else if (syllableCount == 1 && !syllableSet.end.empty()) {
        std::uniform_int_distribution<size_t> endDist(0, syllableSet.end.size() - 1);
        result = syllableSet.end[endDist(rng)];
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
        if (allowDoubleConsonants && choiceDist(rng) < 15) {
            result += doubleConsonants[doubleDist(rng)];
        } else {
            result += consonants[consonantDist(rng)];
        }
        
        result += vowels[vowelDist(rng)];
        
        if (i == syllableCount - 1 && choiceDist(rng) < 40) {
            result += consonants[consonantDist(rng)];
        }
    }
    
    return result;
}

std::string WordGenerator::applyRules(const std::string& word, const GenerationRules& rules) {
    std::string result = word;
    
    if (!rules.allowSpaces) {
        result.erase(std::remove(result.begin(), result.end(), ' '), result.end());
        result.erase(std::remove(result.begin(), result.end(), '-'), result.end());
    }
    
    if (!result.empty()) {
        if (rules.startWithCapital) {
            result[0] = std::toupper(result[0]);
            for (size_t i = 1; i < result.length(); ++i) {
                result[i] = std::tolower(result[i]);
            }
        } else {
            for (size_t i = 0; i < result.length(); ++i) {
                result[i] = std::tolower(result[i]);
            }
        }
    }
    
    result = rules.prefix + result + rules.suffix;
    
    return result;
}

std::string WordGenerator::generate(const GenerationRules& rules) {
    std::uniform_int_distribution<int> syllableDist(rules.minSyllables, rules.maxSyllables);
    int syllableCount = syllableDist(rng);
    
    std::string word;
    
    switch (rules.type) {
        case WordType::NAME:
            word = generateFromStructuredSyllables(nameSyllableSet, syllableCount);
            break;
        case WordType::CITY:
            word = generateFromStructuredSyllables(citySyllableSet, syllableCount);
            break;
        case WordType::COUNTRY:
            word = generateFromStructuredSyllables(countrySyllableSet, syllableCount);
            break;
        case WordType::ANIMAL:
            word = generateFromStructuredSyllables(animalSyllableSet, syllableCount);
            break;
        case WordType::OBJECT:
            word = generateFromStructuredSyllables(objectSyllableSet, syllableCount);
            break;
    }
    
    if (word.empty() || word.length() < 2) {
        word = generateGenericWord(syllableCount, rules.useDoubleConsonants);
    }
    
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