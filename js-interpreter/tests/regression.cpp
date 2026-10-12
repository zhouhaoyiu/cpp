#include "Parser.h"
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

void require(bool condition, const std::string &message) {
    if (!condition) throw std::runtime_error(message);
}
void destroy(Parser::ASTNode *node) {
    if (!node) return;
    for (auto child : node->child) destroy(child);
    destroy(node->next);
    delete node;
}
void expectLexError(const std::string &source, const std::string &message) {
    Lexer lexer;
    lexer.tokenizeInput(source);
    try {
        for (int i = 0; i < 20; ++i) {
            if (lexer.nextToken().type == Lexer::END_OF_LINE) break;
        }
    } catch (const std::runtime_error &error) {
        require(std::string(error.what()).find(message) != std::string::npos, error.what());
        return;
    }
    throw std::runtime_error("missing lexer diagnostic for " + source);
}
int main() {
    const std::vector<std::pair<std::string, std::function<void()>>> tests = {
        {"empty input and AST", [] {
            Parser parser;
            require(parser.parseInput("") == nullptr, "empty program has nodes");
            parser.printAST();
        }},
        {"multiple statements and empty statements", [] {
            Parser parser;
            auto root = parser.parseInput("; let a=1; ; let b=2; a=a+b;;");
            require(root && root->token.value == "a", "missing first statement");
            require(root->next && root->next->token.value == "b", "missing second statement");
            require(root->next->next && root->next->next->type == Parser::VAR_ASSIGN_NODE, "missing assignment");
            require(!root->next->next->next, "extra statement");
            destroy(root);
        }},
        {"nested and sibling AST printing", [] {
            Parser parser;
            auto root = parser.parseInput("let first=1; if(first){let inner=2; inner=3;} let last=4;");
            require(root->next->child[1]->next != nullptr, "nested list was truncated");
            std::ostringstream output;
            auto previous = std::cout.rdbuf(output.rdbuf());
            parser.printAST();
            std::cout.rdbuf(previous);
            for (const auto &name : {"first", "inner", "last"})
                require(output.str().find(name) != std::string::npos, "AST omitted a branch");
            require(output.str().find("  Node") != std::string::npos, "AST lost child indentation");
            destroy(root);
        }},
        {"empty nested blocks", [] {
            Parser parser;
            auto root = parser.parseInput("if(true){} else{} let after=1;");
            require(!root->child[1] && !root->child[2], "empty block was parsed as a statement");
            require(root->next && root->next->token.value == "after", "lost trailing statement");
            destroy(root);
        }},
        {"parser reuse clears lookahead", [] {
            Parser parser;
            destroy(parser.parseInput("let old=1;"));
            auto root = parser.parseInput("let fresh=2; let next=3;");
            require(root && root->token.value == "fresh" && root->next, "stale lookahead");
            destroy(root);
        }},
        {"invalid ASCII character", [] { expectLexError("let a=@;", "unexpected character"); }},
        {"invalid high-bit character", [] { expectLexError(std::string(1, '\xff'), "unexpected character"); }},
        {"unterminated string", [] { expectLexError("let s=\"abc", "unterminated string"); }},
        {"unterminated string at newline", [] { expectLexError("\"abc\nlet a=1;", "unterminated string"); }},
        {"unterminated escaped string", [] { expectLexError("\"abc\\", "unterminated string"); }},
        {"valid strings and escapes", [] {
            Lexer lexer;
            lexer.tokenizeInput(R"("" "abc" "a\"b" "a\\")");
            for (const auto &expected : {"", "abc", "a\\\"b", "a\\\\"}) {
                const auto token = lexer.nextToken();
                require(token.type == Lexer::STRING && token.value == expected, "incorrect string token");
            }
            auto end = lexer.nextToken();
            require(end.type == Lexer::END_OF_LINE, "missing input terminator");
            require(lexer.tokenToString(end).find("END_OF_LINE") != std::string::npos, "EOL cannot be printed");
        }},
        {"unsupported statement fails promptly", [] {
            Parser parser;
            try { parser.parseInput("break;"); }
            catch (const std::runtime_error &) { return; }
            throw std::runtime_error("unsupported statement was accepted");
        }},
    };
    for (const auto &test : tests) {
        try { test.second(); }
        catch (const std::exception &error) {
            std::cerr << "FAIL " << test.first << ": " << error.what() << '\n';
            return 1;
        }
        std::cout << "PASS " << test.first << '\n';
    }
}
