#include <iostream>
#include <exception>
#include "haptic_lexer.hpp" // 1. Usar la cabecera correcta del proyecto[cite: 1]

using namespace std;
using namespace Designar;

int main(int argc, char* argv[]) {
    // Use command-line argument if provided, otherwise default to test file
    string filePath = (argc > 1) ? argv[1] : "test/melodia_prueba.mus";

    try {
        LexicalAnalyzer lexer;
        cout << "Scanning file: " << filePath << "\n" << endl;

        // 2. Usar HapticToken para evitar el choque de nombres con Designar::Token
        SLList<HapticToken> tokens = lexer.scanFile(filePath);

        size_t errorCount = 0;
        tokens.for_each([&errorCount](const HapticToken& tok) {
            cout << tok << endl;
            if (tok.type == TokenType::LEXICAL_ERROR) {
                errorCount++;
            }
        });

        cout << "\nScan completed. Total tokens: " << tokens.size()
             << " | Lexical errors: " << errorCount << endl;

        return (errorCount > 0) ? 1 : 0;

    } catch (const exception& ex) {
        cerr << ex.what() << endl;
        return 1;
    }
}