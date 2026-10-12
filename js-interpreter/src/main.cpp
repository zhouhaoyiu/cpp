#include "Interpreter.h"
#include <iostream>
#include <exception>
#include <cstring>

using namespace std;

int main(int argc, char *argv[]) {
    try {
        Interpreter interpreter;
        if (argc == 1) {
            interpreter.shell();
            return 0;
        }
        if (argc >= 3 && strncmp(argv[2], "-d", 2) == 0) {
            interpreter.setDebugMode(true);
        }
        string filename(argv[1]);
        interpreter.interpretFile(filename);
    } catch (const std::exception &error) {
        cerr << error.what() << endl;
        return 1;
    }
    return 0;
}
