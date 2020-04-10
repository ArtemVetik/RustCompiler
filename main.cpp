#include <iostream>
#include <string>

int main() {
    std::string _separators = "+-*/%=><{}[];(),.";
    char c = ';';
    std::cout << (_separators.find(c));
    return 0;
}