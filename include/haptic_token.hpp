#ifndef HAPTIC_TOKEN_HPP
#define HAPTIC_TOKEN_HPP

#include <string>
#include <ostream>
#include <map.hpp> // Native DeSignar HashMap

enum class TokenType {
    // Reserved keywords (HapticMelody DSL)
    KW_DECLARATION,      // TEMPO, COMPAS, TONALIDAD, MOTOR, PIN
    KW_NOTE,             // DO, RE, MI, FA, SOL, LA, SI, C, D, E, F, G, A, B
    KW_ACCIDENTAL,       // SOSTENIDO, BEMOL
    KW_DURATION,         // REDONDA, BLANCA, NEGRA, CORCHEA, SEMICORCHEA
    KW_EVENT,            // SILENCIO
    KW_MODIFIER,         // INTENSIDAD
    KW_CONTROL,          // REPETIR, VECES
    KW_MODE,             // MAYOR, MENOR

    // Literals and symbols
    IDENTIFIER,
    INTEGER,
    REAL,
    PERCENTAGE,
    TIME_SIGNATURE,      // e.g., 4/4, 3/4, 6/8
    ACCIDENTAL_SYMBOL,   // # or b
    ASSIGN_OPERATOR,     // :
    BRACE_DELIMITER,     // { or }
    PAREN_DELIMITER,     // ( or )
    COMMA_DELIMITER,     // ,

    // Control and layout
    WHITESPACE,          // Espacios y tabulaciones
    NEWLINE,             // Saltos de línea
    LINE_COMMENT,        // Comentarios de línea
    END_OF_FILE,
    LEXICAL_ERROR
};

struct HapticToken {
    TokenType type;
    std::string lexeme;
    size_t line;
    size_t column;
};

// Maps the string token name from Designar::Lexer to the TokenType enum
inline TokenType getTokenType(const std::string& tokenName) {
    static Designar::HashMap<std::string, TokenType> tokenMap = {
        {"KW_DECLARATION",    TokenType::KW_DECLARATION},
        {"KW_NOTE",           TokenType::KW_NOTE},
        {"KW_ACCIDENTAL",     TokenType::KW_ACCIDENTAL},
        {"KW_DURATION",       TokenType::KW_DURATION},
        {"KW_EVENT",          TokenType::KW_EVENT},
        {"KW_MODIFIER",       TokenType::KW_MODIFIER},
        {"KW_CONTROL",        TokenType::KW_CONTROL},
        {"KW_MODE",           TokenType::KW_MODE},
        {"IDENTIFIER",        TokenType::IDENTIFIER},
        {"INTEGER",           TokenType::INTEGER},
        {"REAL",              TokenType::REAL},
        {"PERCENTAGE",        TokenType::PERCENTAGE},
        {"TIME_SIGNATURE",    TokenType::TIME_SIGNATURE},
        {"ACCIDENTAL_SYMBOL", TokenType::ACCIDENTAL_SYMBOL},
        {"ASSIGN_OPERATOR",   TokenType::ASSIGN_OPERATOR},
        {"BRACE_DELIMITER",   TokenType::BRACE_DELIMITER},
        {"PAREN_DELIMITER",   TokenType::PAREN_DELIMITER},
        {"COMMA_DELIMITER",   TokenType::COMMA_DELIMITER},
        {"WHITESPACE",        TokenType::WHITESPACE},
        {"NEWLINE",           TokenType::NEWLINE},
        {"LINE_COMMENT",      TokenType::LINE_COMMENT}
    };

    auto* res = tokenMap.search(tokenName);
    return (res != nullptr) ? *res : TokenType::LEXICAL_ERROR;
}

// String representation of TokenType for debugging and output
inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::KW_DECLARATION:    return "KW_DECLARATION";
        case TokenType::KW_NOTE:           return "KW_NOTE";
        case TokenType::KW_ACCIDENTAL:     return "KW_ACCIDENTAL";
        case TokenType::KW_DURATION:       return "KW_DURATION";
        case TokenType::KW_EVENT:          return "KW_EVENT";
        case TokenType::KW_MODIFIER:       return "KW_MODIFIER";
        case TokenType::KW_CONTROL:        return "KW_CONTROL";
        case TokenType::KW_MODE:           return "KW_MODE";
        case TokenType::IDENTIFIER:        return "IDENTIFIER";
        case TokenType::INTEGER:           return "INTEGER";
        case TokenType::REAL:              return "REAL";
        case TokenType::PERCENTAGE:        return "PERCENTAGE";
        case TokenType::TIME_SIGNATURE:    return "TIME_SIGNATURE";
        case TokenType::ACCIDENTAL_SYMBOL: return "ACCIDENTAL_SYMBOL";
        case TokenType::ASSIGN_OPERATOR:   return "ASSIGN_OPERATOR";
        case TokenType::BRACE_DELIMITER:   return "BRACE_DELIMITER";
        case TokenType::PAREN_DELIMITER:   return "PAREN_DELIMITER";
        case TokenType::COMMA_DELIMITER:   return "COMMA_DELIMITER";
        case TokenType::WHITESPACE:        return "WHITESPACE";
        case TokenType::NEWLINE:           return "NEWLINE";
        case TokenType::LINE_COMMENT:      return "LINE_COMMENT";
        case TokenType::END_OF_FILE:       return "EOF";
        case TokenType::LEXICAL_ERROR:     return "LEXICAL_ERROR";
        default:                           return "UNKNOWN";
    }
}

inline std::ostream& operator<<(std::ostream& os, const HapticToken& tok) {
    std::string displayLexeme = tok.lexeme;
    if (tok.type == TokenType::NEWLINE) displayLexeme = "\\n";
    else if (tok.type == TokenType::WHITESPACE) displayLexeme = "([spaces])";

    os << "[" << tokenTypeToString(tok.type) << " | \"" << displayLexeme
       << "\" | L" << tok.line << ":C" << tok.column << "]";
    return os;
}

#endif // HAPTIC_TOKEN_HPP