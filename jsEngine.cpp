#include<iostream>
#include<string>
#include<map>

using namespace std;

void astParser(string code, map<string, string> JSMap) {
    int currentPos = 0;
    string arr[100];
    while (currentPos < code.length()) {
        string temp = "";
        while (code[currentPos] != ' ' && code[currentPos] != '\n' && code[currentPos] != '\t' &&
               code[currentPos] != '\r') {
            temp += code[currentPos];
            currentPos++;
        }
        arr[currentPos] = temp;
        currentPos++;
        cout << temp << endl;
    }
}

int main() {
    std::string testJsCode = "let a = 1;\n"
                             "let b = 2;\n"
                             "let c = a + b;\n"
                             "console.log(c);\n";
    map<string, string> JSMap;
    map<string, string>::iterator iter;
    JSMap["CONST"] = "const";
    JSMap["VAR"] = "var";
    JSMap["LET"] = "let";
    JSMap["FUNCTION"] = "function";
    JSMap["RETURN"] = "return";
    JSMap["IF"] = "if";
    JSMap["ELSE"] = "else";
    JSMap["WHILE"] = "while";
    JSMap["DO"] = "do";
    JSMap["FOR"] = "for";
    JSMap["BREAK"] = "break";
    JSMap["CONTINUE"] = "continue";
    JSMap["SWITCH"] = "switch";
    JSMap["CASE"] = "case";
    JSMap["DEFAULT"] = "default";
    JSMap["TRY"] = "try";
    JSMap["CATCH"] = "catch";
    JSMap["THROW"] = "throw";
    JSMap["FINALLY"] = "finally";
    JSMap["NEW"] = "new";
    JSMap["TYPEOF"] = "typeof";
    JSMap["INSTANCEOF"] = "instanceof";
    JSMap["DELETE"] = "delete";
    JSMap["IN"] = "in";
    JSMap["OF"] = "of";
    JSMap["NULL"] = "null";
    JSMap["TRUE"] = "true";
    JSMap["FALSE"] = "false";
    JSMap["UNDEFINED"] = "undefined";
    JSMap["NaN"] = "NaN";
    JSMap["Infinity"] = "Infinity";
    JSMap["undefined"] = "undefined";
    JSMap["NaN"] = "NaN";
    astParser(testJsCode, JSMap);
}  


  



