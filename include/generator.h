#ifndef GENERATOR_H
#define GENERATOR_H

#include <string>
#include <vector>
#include <random>

/**
 * Énumération des types de mots disponibles
 */
enum class WordType {
    NAME,
    CITY,
    COUNTRY,
    ANIMAL,
    OBJECT
};

/**
 * Structure représentant les règles de génération
 */
struct GenerationRules {
    WordType type;
    int minSyllables;
    int maxSyllables;
    bool startWithCapital;
    bool allowSpaces;
    std::string prefix;
    std::string suffix;
    bool useDoubleConsonants;
    
    GenerationRules() 
        : type(WordType::NAME), 
          minSyllables(2), 
          maxSyllables(4),
          startWithCapital(true),
          allowSpaces(false),
          prefix(""),
          suffix(""),
          useDoubleConsonants(false) {}
};

/**
 * Structure représentant une syllabe
 */
struct Syllable {
    std::string text;
    bool isVowel;
    
    Syllable(const std::string& t) : text(t), isVowel(false) {}
};

/**
 * Classe gérant la génération de mots aléatoires par syllabes
 */
class WordGenerator {
private:
    std::mt19937 rng;
    
    // Syllabes pour différents types
    std::vector<std::string> consonants;
    std::vector<std::string> vowels;
    std::vector<std::string> nameSyllables;
    std::vector<std::string> citySyllables;
    std::vector<std::string> countrySyllables;
    std::vector<std::string> animalSyllables;
    std::vector<std::string> objectSyllables;
    std::vector<std::string> doubleConsonants;
    
    void initializeSyllables();
    std::string generateFromSyllables(const std::vector<std::string>& syllables, int count);
    std::string generateGenericWord(int syllableCount, bool allowDoubleConsonants);
    std::string applyRules(const std::string& word, const GenerationRules& rules);
    
public:
    WordGenerator();
    
    std::string generate(const GenerationRules& rules);
    std::vector<std::string> generateMultiple(const GenerationRules& rules, int count);
    
    const char* getWordTypeName(WordType type) const;
};

#endif // GENERATOR_H