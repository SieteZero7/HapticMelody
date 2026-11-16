#include "haptic_lexer.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>

using namespace std;

// Helper to generate case-insensitive regex using supported DeSignar syntax (...) and |
static string toCaseInsensitive(const string& word) {
    string pattern;
    for (char c : word) {
        if (isalpha(static_cast<unsigned char>(c))) {
            pattern += '(';
            pattern += static_cast<char>(toupper(static_cast<unsigned char>(c)));
            pattern += '|';
            pattern += static_cast<char>(tolower(static_cast<unsigned char>(c)));
            pattern += ')';
        } else {
            pattern += c;
        }
    }
    return pattern;
}

// DeSignar regex does NOT support character classes like [0-9] or [a-z].
// We build valid regex strings using only literals, (...), |, +, *, ?.
static string getDigitRegex() {
    return "(0|1|2|3|4|5|6|7|8|9)";
}

static string getLetterRegex() {
    string s = "(";
    for (char c = 'a'; c <= 'z'; ++c) { s += c; s += "|"; }
    for (char c = 'A'; c <= 'Z'; ++c) { s += c; s += "|"; }
    s += "_)";
    return s;
}

LexicalAnalyzer::LexicalAnalyzer() {
    initializeRules();
}

void LexicalAnalyzer::initializeRules() {
    // 1. Whitespace and Newlines (avoiding [...] brackets)
    lexerEngine.add_token("WHITESPACE", "( |\t|\r)+");
    lexerEngine.add_token("NEWLINE", "\n");

    // 2. Declarations
    lexerEngine.add_token("KW_DECLARATION", toCaseInsensitive("TEMPO"));
    lexerEngine.add_token("KW_DECLARATION", toCaseInsensitive("COMPAS"));
    lexerEngine.add_token("KW_DECLARATION", toCaseInsensitive("TONALIDAD"));
    lexerEngine.add_token("KW_DECLARATION", toCaseInsensitive("MOTOR"));
    lexerEngine.add_token("KW_DECLARATION", toCaseInsensitive("PIN"));

    // 3. Durations
    lexerEngine.add_token("KW_DURATION", toCaseInsensitive("REDONDA"));
    lexerEngine.add_token("KW_DURATION", toCaseInsensitive("BLANCA"));
    lexerEngine.add_token("KW_DURATION", toCaseInsensitive("NEGRA"));
    lexerEngine.add_token("KW_DURATION", toCaseInsensitive("CORCHEA"));
    lexerEngine.add_token("KW_DURATION", toCaseInsensitive("SEMICORCHEA"));

    // 4. Events, Modifiers, Control, Modes, and Accidentals
    lexerEngine.add_token("KW_EVENT",      toCaseInsensitive("SILENCIO"));
    lexerEngine.add_token("KW_MODIFIER",   toCaseInsensitive("INTENSIDAD"));
    lexerEngine.add_token("KW_CONTROL",    toCaseInsensitive("REPETIR"));
    lexerEngine.add_token("KW_CONTROL",    toCaseInsensitive("VECES"));
    lexerEngine.add_token("KW_MODE",       toCaseInsensitive("MAYOR"));
    lexerEngine.add_token("KW_MODE",       toCaseInsensitive("MENOR"));
    lexerEngine.add_token("KW_ACCIDENTAL", toCaseInsensitive("SOSTENIDO"));
    lexerEngine.add_token("KW_ACCIDENTAL", toCaseInsensitive("BEMOL"));

    // 5. Musical Notes (Spanish and English, with optional #/b and octave 0-9)
    const string notes[] = {
        "DO", "RE", "MI", "FA", "SOL", "LA", "SI",
        "C", "D", "E", "F", "G", "A", "B"
    };
    string digit = getDigitRegex();
    for (const auto& note : notes) {
        string notePattern = "(" + toCaseInsensitive(note) + ")((#|b)?)" + "(" + digit + ")?";
        lexerEngine.add_token("KW_NOTE", notePattern);
    }

    // 6. Identifiers (must go after reserved keywords)
    string letter = getLetterRegex();
    string alnum = "(" + letter + "|" + digit + ")";
    string idPattern = letter + "+" + alnum + "*";
    lexerEngine.add_token("IDENTIFIER", idPattern);

    // 7. Numeric Literals (most specific to most general)
    string dPlus = digit + "+";
    lexerEngine.add_token("TIME_SIGNATURE", dPlus + "/" + dPlus);
    lexerEngine.add_token("PERCENTAGE",     digit + digit + "?" + digit + "?%");
    lexerEngine.add_token("REAL",           dPlus + "\\." + dPlus);
    lexerEngine.add_token("INTEGER",        dPlus);

    // 8. Standalone Accidentals, Operators, and Delimiters
    lexerEngine.add_token("ACCIDENTAL_SYMBOL", "#");
    lexerEngine.add_token("ACCIDENTAL_SYMBOL", "b");
    lexerEngine.add_token("ASSIGN_OPERATOR",   ":");
    lexerEngine.add_token("BRACE_DELIMITER",   "\\{");
    lexerEngine.add_token("BRACE_DELIMITER",   "\\}");
    lexerEngine.add_token("PAREN_DELIMITER",   "\\(");
    lexerEngine.add_token("PAREN_DELIMITER",   "\\)");
    lexerEngine.add_token("COMMA_DELIMITER",   ",");
}

Designar::SLList<HapticToken> LexicalAnalyzer::scanString(const string& sourceCode) {
    // Preprocess to handle line comments cleanly without regex bracket issues
    string cleanedSource = "";
    stringstream ss(sourceCode);
    string line;
    while (getline(ss, line)) {
        size_t commentPos = line.find("//");
        if (commentPos != string::npos) {
            line = line.substr(0, commentPos);
        }
        cleanedSource += line + "\n";
    }

    Designar::SLList<HapticToken> tokenList;
    Designar::DynArray<Designar::Token> rawTokens = lexerEngine.tokenize(cleanedSource);

    size_t currentLine = 1;
    size_t currentColumn = 1;
    size_t lastPos = 0;

    for (size_t i = 0; i < rawTokens.size(); ++i) {
        const auto& rawTok = rawTokens[i];

        for (size_t p = lastPos; p < rawTok.position; ++p) {
            if (cleanedSource[p] == '\n') {
                currentLine++;
                currentColumn = 1;
            } else {
                currentColumn++;
            }
        }
        lastPos = rawTok.position;

        TokenType type = getTokenType(rawTok.name);
        const string& lexeme = rawTok.lexeme;

        tokenList.append(HapticToken{type, lexeme, currentLine, currentColumn});

        for (char c : lexeme) {
            if (c == '\n') {
                currentLine++;
                currentColumn = 1;
            } else {
                currentColumn++;
            }
            lastPos++;
        }
    }

    for (size_t p = lastPos; p < cleanedSource.size(); ++p) {
        if (cleanedSource[p] == '\n') {
            currentLine++;
            currentColumn = 1;
        } else {
            currentColumn++;
        }
    }

    tokenList.append(HapticToken{TokenType::END_OF_FILE, "EOF", currentLine, currentColumn});
    return tokenList;
}

Designar::SLList<HapticToken> LexicalAnalyzer::scanFile(const string& filePath) {
    if (filePath.size() < 4 || filePath.substr(filePath.size() - 4) != ".mus") {
        throw invalid_argument("Error: Input file must have a .mus extension");
    }

    ifstream file(filePath);
    if (!file.is_open()) {
        throw runtime_error("Error: Could not open file: " + filePath);
    }

    stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return scanString(buffer.str());
}