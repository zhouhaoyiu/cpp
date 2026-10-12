#include "Parser.h"
#include <iostream>
#include <exception>

using namespace std;

int main(int argc, char *argv[]) {
    try {
        if (argc < 2) {
            cerr << "usage: " << argv[0] << " <*.js>" <<endl;
            exit(-1);
        }
        string filename(argv[1]);
        Parser parser;
        parser.parseFile(filename);
        Parser::ASTNode *ast = parser.getAST();
        parser.printAST();
    } catch (const std::exception &error) {
        cerr << error.what() << endl;
        return 1;
    }
    return 0;
}