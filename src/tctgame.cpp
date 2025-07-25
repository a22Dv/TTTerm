#include "tctgame.hpp"
#include <chrono>
#include <thread>
#include <iostream>

namespace tct {

void Game::run() {
    // Starting scene.
    pushScene(std::make_unique<MenuScene>(*this));
    std::size_t sceneCount = 0;
    std::chrono::steady_clock::time_point cTime{std::chrono::steady_clock::now()};
    std::chrono::milliseconds threshold{1};
    std::chrono::milliseconds frameInterval{33};
    while (!terminate) {
        // New scene pushed.
        if (sceneCount < sceneStack.size()) {
            sceneStack.back()->init();
            sceneCount = sceneStack.size();
        }
        while (!terminate && sceneCount == sceneStack.size()) {
            cTime = std::chrono::steady_clock::now();
            sceneStack.back()->update();
            sceneStack.back()->render();
            std::chrono::milliseconds delta =
                std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - cTime);
            std::chrono::milliseconds remainder = frameInterval - delta;
            if (remainder < threshold) {
                continue;
            }
            std::this_thread::sleep_for(remainder);
        }
    }
}

void MenuScene::init() {
    return;
}

void MenuScene::update() {
    std::cout << "Hello World from MenuScene.";
}

void MenuScene::render() {
    return;
}

void GameScene::init() {
    return;
}

void GameScene::update() {
    std::cout << "Hello World from GameScene.";
}

void GameScene::render() {
    return;
}

} // namespace tct
