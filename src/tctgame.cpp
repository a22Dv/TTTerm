#include "tctgame.hpp"
#include "tctassets.hpp"
#include "tctdisplay.hpp"
#include "tctinput.hpp"
#include <chrono>
#include <cstdlib>
#include <random>
#include <thread>

/*
    Separate assets into individual assignable components from .aseprite file.
    Get BGM for (AI - EASY, NORMAL, HARD, UNBEATABLE, MAIN MENU),
    Get SFX for Victory/Defeat, selecting buttons, selecting board cells,
    placing board characters. Get game logic working, core loop,
    get minimax AI working (Depth limited for difficulty adjustment, random for easy)
*/

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

namespace {

inline std::size_t asSizeT(AssetId assetId) { return static_cast<std::size_t>(assetId); }

} // namespace
MenuScene::MenuScene(Game &game)
    : gameInstance{game}, context{MenuContext(game.getDsp(), game.getAud(), game.getInput(), game.getRegistry())} {}
void MenuScene::init() {
    // Declare assets.
    context.objList.push_back(Object{AssetId::BORDER, Vector2{0, 0}});
    context.objList.push_back(Object{AssetId::TITLE, Vector2{18, 53}});
    context.objList.push_back(Object{AssetId::AI_MENU_EASY, Vector2{115, 43}});
    context.objList.push_back(Object{AssetId::AI_MENU_NORMAL, Vector2{115, 54}});
    context.objList.push_back(Object{AssetId::AI_MENU_HARD, Vector2{115, 64}});
    context.objList.push_back(Object{AssetId::AI_MENU_UNBEATABLE, Vector2{115, 73}});
    context.objList.push_back(Object{AssetId::SELECTOR_MENU, Vector2{108, 43}});
    

    // Setup.
    context.selectorObjIdx = context.objList.size() - 1;
    context.selectorCPosIdx = 0;
    context.selectorPos = {Vector2{108, 43}, Vector2{108, 54}, Vector2{108, 64}, Vector2{108, 73}};
    context.aud.playFileFade(context.reg.getAsset(AssetId::MENU_BGM), 1.0f, 0.1f, true);
}

void MenuScene::update() {
    InputKey input = context.ipt.pollInput();
    const std::size_t availSelectorPos = context.selectorPos.size();
    switch (input) {
    case InputKey::ARROW_UP: {
        context.selectorCPosIdx = (context.selectorCPosIdx + (availSelectorPos - 1)) % availSelectorPos;
        context.objList[context.selectorObjIdx].pos = context.selectorPos[context.selectorCPosIdx];
        context.aud.playFile(context.reg.getAsset(AssetId::SELECT_MENU));
        return;
    }
    case InputKey::ARROW_DOWN: {
        context.selectorCPosIdx = (context.selectorCPosIdx + 1) % availSelectorPos;
        context.objList[context.selectorObjIdx].pos = context.selectorPos[context.selectorCPosIdx];
        context.aud.playFile(context.reg.getAsset(AssetId::SELECT_MENU), 0.5f);
        return;
    }
    case InputKey::ENTER: {
        context.diff = static_cast<SetDifficulty>(context.selectorCPosIdx);
        std::random_device rd{};
        std::mt19937 gen{rd()};
        std::uniform_int_distribution<> dst{0, 1};
        context.playerFirst = dst(gen) == 0;
        context.aud.playFile(context.reg.getAsset(AssetId::ENTER_MENU), 0.5f);
        gameInstance.pushScene(std::make_unique<GameScene>(gameInstance, context.playerFirst, context.diff));
        return;
    }
    default: {
        return;
    }
    }
}

void MenuScene::render() {
    Display &dsp = context.dsp;
    AssetRegistry &reg = context.reg;
    const std::size_t renderCount = context.objList.size();
    for (std::size_t i = 0; i < renderCount; ++i) {
        Object &obj = context.objList[i];
        dsp.render(RenderRequest{reg.getAsset(obj.id), obj.pos});
    }
    dsp.present();
}

GameScene::GameScene(Game &game, const bool pFirst, const SetDifficulty diff)
    : gameInstance{game}, context{GameContext(game.getDsp(), game.getAud(), game.getInput(), game.getRegistry())} {};
void GameScene::init() { return; }

void GameScene::update() {}

void GameScene::render() { return; }

} // namespace tct
