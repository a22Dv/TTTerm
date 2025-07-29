#pragma once
#include "tctassets.hpp"
#include "tctaudio.hpp"
#include "tctdisplay.hpp"
#include "tctinput.hpp"
#include <variant>


namespace tct {

class Game;

enum class SetDifficulty { AI_EASY, AI_NORMAL, AI_HARD, AI_UNBEATABLE };

struct Object {
    AssetId id{};
    Vector2 pos{};
    Object() {};
    Object(const AssetId id, const Vector2 pos) : id{id}, pos{pos} {};
};

struct MenuContext {
    Display &dsp;
    Audio &aud;
    Input &ipt;
    AssetRegistry &reg;
    MenuContext(Display &dsp, Audio &aud, Input &ipt, AssetRegistry &reg) : dsp{dsp}, aud{aud}, ipt{ipt}, reg{reg} {};
    SetDifficulty diff{};
    std::vector<Object> objList{};
    std::vector<Vector2> selectorPos{};
    std::size_t selectorCPosIdx{};
    std::size_t selectorObjIdx{};
    bool playerFirst{};
};

struct GameContext {
    Display &dsp;
    Audio &aud;
    Input &ipt;
    AssetRegistry &reg;
    SetDifficulty diff{};
    bool playerFirst{};
    GameContext(Display &dsp, Audio &aud, Input &ipt, AssetRegistry &reg) : dsp{dsp}, aud{aud}, ipt{ipt}, reg{reg} {};
};

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
    MenuContext context;
    Game &gameInstance;

  public:
    void init() override;
    void update() override;
    void render() override;
    MenuScene(Game &game);
};

class GameScene : public Scene {
  private:
    GameContext context;
    Game &gameInstance;

  public:
    void init() override;
    void update() override;
    void render() override;
    GameScene(Game &game, const bool pFirst, const SetDifficulty diff);
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
    Display &getDsp() { return displayModule; };
    Audio &getAud() { return audioModule; };
    Input &getInput() { return inputModule; };
    AssetRegistry &getRegistry() { return registry; };
    void popScene() { sceneStack.pop_back(); };
    void pushScene(std::unique_ptr<Scene> scene) { sceneStack.push_back(std::move(scene)); };
    void quit() { terminate = true; };
    void run();
};

} // namespace tct