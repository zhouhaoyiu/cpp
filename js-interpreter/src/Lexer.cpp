#include "Lexer.h"
#include <iostream>
#include <string>
#include <utility>
#include <stdexcept>
#include <sstream>
#include <cctype>

using namespace std;

Lexer::Lexer() {
    rowNumber = 0;
    rowBufferPos = 0;
    initKeywordsAndSymbols();
}

void Lexer::tokenizeInput(string i) {
    inputMode = true;
    canRollBack = false;
    input = std::move(i);
    rowNumber++;
    rowBuffer = input;
    rowBuffer += '\n';
    rowBufferPos = 0;
}

void Lexer::openFile(const string &filename) {
    closeFile();
    file.clear();
    rowBuffer.clear();
    rowBufferPos = rowNumber = 0;
    inputMode = canRollBack = false;
    file.open(filename);
    if (file.fail()) {
        throw runtime_error("cannot open file: " + filename);
    }
}

void Lexer::closeFile() {
    file.close();
}

void Lexer::initKeywordsAndSymbols() {
    keywords.insert("function");
    keywords.insert("var");
    keywords.insert("let");
    keywords.insert("const");
    keywords.insert("true");
    keywords.insert("false");
    keywords.insert("this");
    keywords.insert("if");
    keywords.insert("else");
    keywords.insert("while");
    keywords.insert("return");
    keywords.insert("undefined");
    keywords.insert("null");
    keywords.insert("for");
    keywords.insert("break");
    keywords.insert("continue");
    keywords.insert("class");
    keywords.insert("extends");
    keywords.insert("new");
    keywords.insert("super");
    keywords.insert("switch");
    keywords.insert("case");
    keywords.insert("default");
    keywords.insert("throw");
    keywords.insert("try");
    keywords.insert("catch");
    keywords.insert("finally");
    keywords.insert("typeof");
    keywords.insert("void");
    keywords.insert("delete");
    keywords.insert("instanceof");
    keywords.insert("in");
    keywords.insert("of");
    keywords.insert("as");
    keywords.insert("import");
    keywords.insert("export");
    keywords.insert("from");
    keywords.insert("static");
    keywords.insert("readonly");

    symbols.insert("{");
    symbols.insert("}");
    symbols.insert("(");
    symbols.insert(")");
    symbols.insert("[");
    symbols.insert("]");
    symbols.insert(".");
    symbols.insert(",");
    symbols.insert(";");
    symbols.insert("+");
    symbols.insert("-");
    symbols.insert("*");
    symbols.insert("/");
    symbols.insert("%");
    symbols.insert("&");
    symbols.insert("|");
    symbols.insert("!");
    symbols.insert("<");
    symbols.insert(">");
    symbols.insert("=");
    symbols.insert(">=");
    symbols.insert("<=");
    symbols.insert("==");
    symbols.insert("!=");
    symbols.insert("&&");
    symbols.insert("||");
    symbols.insert("?");
    symbols.insert(":");
    symbols.insert("~");
    symbols.insert("^");
    symbols.insert("<<");
    symbols.insert(">>");
    symbols.insert(">>>");
    symbols.insert("+=");
    symbols.insert("-=");
    symbols.insert("*=");
    symbols.insert("/=");
    symbols.insert("%=");
    symbols.insert("&=");
    symbols.insert("|=");
    symbols.insert("^=");
    symbols.insert("<<=");
    symbols.insert(">>=");
    symbols.insert(">>>=");
    symbols.insert("=");
}

void Lexer::resetRow() {
    rowNumber = 0;
}

bool Lexer::isKeyword(const string &str) {
    return keywords.find(str) != keywords.end();
}

bool Lexer::isSymbol(const string &str) {
    return symbols.find(str) != symbols.end();
}

int Lexer::nextChar() {
    canRollBack = false;
    if (rowBufferPos >= rowBuffer.size()) {
        if (inputMode) return '\0';
        if (!getline(file, rowBuffer)) return EOF;
        ++rowNumber;
        rowBuffer += '\n';
        rowBufferPos = 0;
    }
    canRollBack = true;
    return static_cast<unsigned char>(rowBuffer[rowBufferPos++]);
}

void Lexer::rollBack() {
    if (canRollBack && rowBufferPos > 0) --rowBufferPos;
    canRollBack = false;
}

Lexer::Token Lexer::nextToken() {
    int currentChar = nextChar();
    Token token;
    while (true) {
        // Check EOF.
        if (currentChar == EOF) {
            token.type = END_OF_FILE;
            break;
        }
        // Check dnd of line.
        if (currentChar == '\0') {
            token.type = END_OF_LINE;
            break;
        }
        // Skip all blank characters.
        if (isspace(currentChar)) {
            currentChar = nextChar();
            continue;
        }
        // Construct identifier and keyword token.
        if (isalpha(currentChar) || currentChar == '_') {
            token.type = ID;
            token.value += currentChar;
            currentChar = nextChar();
            while (isalpha(currentChar) || currentChar == '_' || isdigit(currentChar)) {
                token.value += currentChar;
                currentChar = nextChar();
            }
            rollBack();
            if (isKeyword(token.value)) token.type = KEYWORD;
            // if(token.value == "true" || token.value == "false") token.type = BOOL;
            break;
        }
        // Construct int and real token.
        if (isdigit(currentChar)) {
            token.value += currentChar;
            currentChar = nextChar();
            while (true) {
                if (isdigit(currentChar)) {
                    token.value += currentChar;
                    currentChar = nextChar();
                    continue;
                }
                if (currentChar == '.') {
                    if (token.type != REAL) {
                        token.value += currentChar;
                        token.type = REAL;
                        currentChar = nextChar();
                        continue;
                    } else {
                        break;
                    }
                }
                break;
            }
            rollBack();
            if (token.type != REAL) token.type = INT;
            break;
        }
        // Construct string token.
        if (currentChar == '"') {
            bool escaped = false;
            token.type = STRING;
            while (true) {
                currentChar = nextChar();
                if (currentChar == EOF || currentChar == '\0' ||
                    currentChar == '\n' || currentChar == '\r') {
                    error("unterminated string literal", currentChar);
                }
                if (currentChar == '"' && !escaped) break;
                token.value += static_cast<char>(currentChar);
                escaped = currentChar == '\\' && !escaped;
            }
            break;
        }
        // Construct char token.
        if (currentChar == '\'') {
            currentChar = nextChar();
            if (currentChar != '\\') {
                token.value = currentChar;
                currentChar = nextChar();
            } else {
                token.value = currentChar;
                currentChar = nextChar();
                token.value += currentChar;
                currentChar = nextChar();
            }
            if (currentChar != '\'') {
                error("too much characters in single quote", currentChar);
            }
            token.type = CHAR;
            break;
        }
        // Construct symbol token.
        std::string str(1, currentChar);
        if (Lexer::isSymbol(str)) {
            token.type = Lexer::SYMBOL;
            token.value += str;
            if (currentChar == '=' || currentChar == '>' || currentChar == '<' || currentChar == '!') {
                currentChar = nextChar();
                if (currentChar == '=') {
                    token.value += '=';
                } else {
                    rollBack();
                }
            } else if (currentChar == '&') {
                currentChar = nextChar();
                if (currentChar == '&') {
                    token.value += '&';
                } else {
                    rollBack();
                }
            } else if (currentChar == '|') {
                currentChar = nextChar();
                if (currentChar == '|') {
                    token.value += '|';
                } else {
                    rollBack();
                }
            }
            break;
        }
        error("unexpected character", currentChar);
    }
    token.rowNumber = rowNumber;
    return token;
}

void Lexer::print(const Lexer::Token &token) {
    cout << tokenToString(token) << endl;
}

string Lexer::tokenToString(const Lexer::Token &token) {
    std::string tokenType;
    std::string tokenValue = token.value;
    switch (token.type) {
        case Lexer::ID:
            tokenType = "ID";
            break;
        case Lexer::KEYWORD:
            tokenType = "KEYWORD";
            break;
        case Lexer::REAL:
            tokenType = "REAL";
            break;
        case Lexer::INT:
            tokenType = "INT";
            break;
        case Lexer::STRING:
            tokenType = "STRING";
            tokenValue = "\"" + tokenValue + "\"";
            break;
        case Lexer::CHAR:
            tokenType = "CHAR";
            tokenValue = "'" + tokenValue + "'";
            break;
        case Lexer::SYMBOL:
            tokenType = "SYMBOL";
            tokenValue = "\"" + tokenValue + "\"";
            break;
        case Lexer::NONE:
            tokenType = "NONE";
            break;
        case Lexer::ERROR:
            tokenType = "ERROR";
            break;
        case Lexer::END_OF_FILE:
            tokenType = "END_OF_FILE";
            break;
        case Lexer::END_OF_LINE:
            tokenType = "END_OF_LINE";
            break;
        default:
            error("unexpected token type: " + to_string(token.type), '\0');
    }
    return "<" + tokenType + ", " + tokenValue + ", " + to_string(token.rowNumber) + ">";
}


void Lexer::error(const std::string &message, int currentChar) {
    ostringstream diagnostic;
    diagnostic << "[Lexer] " << message << " at row " << rowNumber
               << " col " << rowBufferPos;
    if (currentChar != EOF && currentChar != '\0')
        diagnostic << " (character code " << currentChar << ")";
    throw runtime_error(diagnostic.str());
}

void Lexer::setDebugMode(bool enable) {
    debug = enable;
}

