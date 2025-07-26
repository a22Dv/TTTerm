#include "tctgame.hpp"
#include "tctdisplay.hpp"
#include <chrono>
#include <thread>

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

void MenuScene::init() { return; }

void MenuScene::update() {}

void MenuScene::render() {
    Display &dsp{gameInstance.getDsp()};
    AssetRegistry &registry{gameInstance.getRegistry()};
    dsp.render(RenderRequest{registry.getAsset(AssetId::TEST_IMAGE), Vector2{.x = 0, .y = 0}});
    dsp.present();
}

void GameScene::init() { return; }

void GameScene::update() {}

void GameScene::render() { return; }

} // namespace tct
