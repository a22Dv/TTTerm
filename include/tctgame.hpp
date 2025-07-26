#pragma once
#include "tctassets.hpp"
#include "tctaudio.hpp"
#include "tctdisplay.hpp"
#include "tctinput.hpp"
#include <variant>
namespace tct {

class Game;

struct GameContext {};

struct MenuContext {};

using SceneContext = std::variant<GameContext, MenuContext>;

class Scene {
  public:
    virtual void init() = 0;
    virtual void update() = 0;
    virtual void render() = 0;
    virtual ~Scene() = default;
};

class MenuScene : public Scene {
  private:
    MenuContext context{};
    Game &gameInstance;

  public:
    void init() override;
    void update() override;
    void render() override;
    MenuScene(Game &game) : gameInstance{game} {};
};

class GameScene : public Scene {
  private:
    GameContext context{};
    Game &gameInstance;

  public:
    void init() override;
    void update() override;
    void render() override;
    GameScene(Game &game) : gameInstance{game} {};
};

class Game {
  private:
    bool terminate{};
    Display displayModule{};
    Audio audioModule{};
    Input inputModule{};
    AssetRegistry registry{};
    std::vector<std::unique_ptr<Scene>> sceneStack{};

  public:
    Display& getDsp() { return displayModule; };
    Audio& getAud() { return audioModule; };
    Input& getInput() { return inputModule; };
    AssetRegistry& getRegistry() { return registry; };
    void popScene() { sceneStack.pop_back(); };
    void pushScene(std::unique_ptr<Scene> scene) { sceneStack.push_back(std::move(scene)); };
    void quit() { terminate = true; };
    void run();
};

} // namespace tct