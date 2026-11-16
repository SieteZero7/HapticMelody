#ifndef HAPTIC_LEXER_HPP
#define HAPTIC_LEXER_HPP

#include <string>
#include <lexer.hpp> // Cabecera de DeSignar
#include <list.hpp>  // Lista SLList de DeSignar
#include "haptic_token.hpp"


class LexicalAnalyzer {
private:
    Designar::Lexer lexerEngine;
    void initializeRules();

public:
    LexicalAnalyzer();

    Designar::SLList<HapticToken> scanString(const std::string& sourceCode);
    Designar::SLList<HapticToken> scanFile(const std::string& filePath);
};

#endif // HAPTIC_LEXER_HPP