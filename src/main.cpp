#include <iostream>
#include <string>
#include <vector>
#include "parser.h"
#include "ast.h"

int main() {
    std::cout << "--- DBMS CLI (Variant 7) ---\n";
    std::cout << "Type your commands. End them with ';'. Type 'exit;' to quit.\n\n";

    std::string buffer = "";
    std::string line;

    while (true) {
        if (buffer.empty()) std::cout << "> ";
        else std::cout << "... ";

        if (!std::getline(std::cin, line)) break;

        bool in_string = false;
        bool cmd_ready = false;

        for (char c : line) {
            if (c == '"') {
                in_string = !in_string;
            }
            if (c == ';' && !in_string) {
                cmd_ready = true;
                break;
            }
            buffer += c;
        }

        if (cmd_ready) {
            if (ToUpper(buffer).find("EXIT") != std::string::npos) {
                std::cout << "Exiting...\n";
                break;
            }

            try {
                auto tokens = Tokenize(buffer);
                if (!tokens.empty()) {
                    Parser parser(tokens);
                    auto command = parser.Parse();
                    
                    if (command) {
                        std::cout << "\n[SUCCESS] Command parsed successfully.\n";
                        command->Print();
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "\n[ERROR] " << e.what() << "\n";
            }

            buffer.clear();
            std::cout << "\n";
        } else {
            buffer += " "; 
        }
    }

    return 0;
}