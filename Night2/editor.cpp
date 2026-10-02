#include <iostream>
#include <stack>
#include <string>
#include<cstddef>
#include<sstream>

class Editor {
private:
    std::string document;
    std::stack<std::string> undoHistory;
    std::stack<std::string> redoHistory;

public:
    void append(const std::string& text) {
      
        if (text.empty()) {
            return;
        }

       
        undoHistory.push(document);

        document += text;

        
        while (!redoHistory.empty()) {
            redoHistory.pop();
        }
    }

    bool undo() {
    if (undoHistory.empty()) {
        return false;
    }

    
    redoHistory.push(document);

    
    document = undoHistory.top();
    undoHistory.pop();

    return true;
}
    bool redo() {
    if (redoHistory.empty()) {
        return false;
    }

    
    undoHistory.push(document);

    
    document = redoHistory.top();
    redoHistory.pop();

    return true;
}

const std::string& text() const {
        return document;
    }


bool erase(std::size_t count) {
    
    if (count == 0 || count > document.size()) {
        return false;
    }

   
    undoHistory.push(document);

    document.erase(document.size() - count);

    
    while (!redoHistory.empty()) {
        redoHistory.pop();
    }

    return true;
}
};
int main() {
    Editor editor;
    std::string line;

    const auto printHelp = []() {
        std::cout
            << "Commands:\n"
            << "  append TEXT - add text to the end\n"
            << "  erase N     - remove the last N characters\n"
            << "  undo        - reverse the last edit\n"
            << "  redo        - restore an undone edit\n"
            << "  show        - display the document\n"
            << "  help        - show these commands\n"
            << "  quit        - exit\n";
    };

    printHelp();

    while (true) {
        std::cout << "> ";

        if (!std::getline(std::cin, line)) {
            break; // End of input.
        }

        std::istringstream input(line);
        std::string command;
        input >> command;

        if (command.empty()) {
            continue;
        }

       
        if (command != "append" && command != "erase") {
            std::string extra;

            if (input >> extra) {
                std::cout << "Unexpected argument. Type help.\n";
                continue;
            }
        }

        if (command == "append") {
            std::string text;
            std::getline(input, text);

            
            if (!text.empty() &&
                (text.front() == ' ' || text.front() == '\t')) {
                text.erase(0, 1);
            }

            if (text.empty()) {
                std::cout << "Nothing to append.\n";
            } else {
                editor.append(text);
            }
        } else if (command == "erase") {
            long long count;
            std::string extra;

            if (!(input >> count) ||
                count <= 0 ||
                (input >> extra) ||
                static_cast<unsigned long long>(count) >
                    editor.text().size()) {
                std::cout
                    << "Use erase N with a positive whole number "
                    << "no larger than the document length.\n";
                continue;
            }

            editor.erase(static_cast<std::size_t>(count));
        } else if (command == "undo") {
            if (!editor.undo()) {
                std::cout << "Nothing to undo.\n";
            }
        } else if (command == "redo") {
            if (!editor.redo()) {
                std::cout << "Nothing to redo.\n";
            }
        } else if (command == "show") {
            std::cout << "[" << editor.text() << "]\n";
        } else if (command == "help") {
            printHelp();
        } else if (command == "quit") {
            break;
        } else {
            std::cout << "Unknown command. Type help.\n";
        }
    }

    std::cout << "Goodbye!\n";
    return 0;
}