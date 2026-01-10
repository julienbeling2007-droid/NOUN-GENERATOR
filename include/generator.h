#ifndef GENERATOR_H
#define GENERATOR_H

#include <string>
#include <vector>
#include <random>

enum class WordType {
    NAME,
    CITY,
    COUNTRY,
    ANIMAL,
    OBJECT
};

struct GenerationRules {
    WordType type = WordType::NAME;
    int minSyllables = 2;
    int maxSyllables = 3;
    bool startWithCapital = true;
    bool allowSpaces = false;
    bool useDoubleConsonants = false;
    std::string prefix = "";
    std::string suffix = "";
};

struct SyllableSet {
    std::vector<std::string> start;
    std::vector<std::string> middle;
    std::vector<std::string> end;
};

class WordGenerator {
private:
    std::mt19937 rng;
    
    std::vector<std::string> consonants;
    std::vector<std::string> vowels;
    std::vector<std::string> doubleConsonants;
    
    SyllableSet nameSyllableSet;
    SyllableSet citySyllableSet;
    SyllableSet countrySyllableSet;
    SyllableSet animalSyllableSet;
    SyllableSet objectSyllableSet;
    
    void initializeSyllables();
    void initializeStructuredSyllables();
    
    std::string generateFromStructuredSyllables(const SyllableSet& syllableSet, int syllableCount);
    std::string generateGenericWord(int syllableCount, bool allowDoubleConsonants);
    std::string applyRules(const std::string& word, const GenerationRules& rules);
    
public:
    WordGenerator();
    
    std::string generate(const GenerationRules& rules);
    std::vector<std::string> generateMultiple(const GenerationRules& rules, int count);
    const char* getWordTypeName(WordType type) const;
};

#endif // GENERATOR_H