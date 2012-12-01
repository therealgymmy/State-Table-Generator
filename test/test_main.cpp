#include <iostream>
#include "Command.h"

int main ()
{
    std::string command = "CREATE VERTEX AT @15";
    std::vector<cmd::Token> list = cmd::tokenize(command);
    if (list[0].type() != cmd::Token::CREATE) {
        std::cout << "CREATE is not matched";
    }
    if (list[1].type() != cmd::Token::VERTEX) {
        std::cout << "VERTEX is not matched";
    }
    if (list[2].type() != cmd::Token::AT) {
        std::cout << "AT is not matched";
    }
    if (list[3].type() != cmd::Token::ID) {
        std::cout << "ID is not matched";
    }
}
