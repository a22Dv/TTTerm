#include "tctgame.hpp"
#include <exception>
#include <iostream>

int main() {
    try {
        tct::Game game{};
        game.run();
    } catch (const std::exception &e) {
        std::cerr << e.what();
    }

    return 0;
}