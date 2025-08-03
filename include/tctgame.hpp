#pragma once
#include "tctassets.hpp"
#include "tctaudio.hpp"
#include "tctdisplay.hpp"
#include "tctinput.hpp"
#include <cstdint>
#include <variant>


namespace tct {

constexpr const std::size_t boardSideLength = 3;
constexpr const std::size_t boardSize = boardSideLength * boardSideLength;

class Game;

enum class SetDifficulty : std::uint8_t { AI_EASY, AI_NORMAL, AI_HARD, AI_UNBEATABLE };
enum class CellStatus : std::uint8_t { O, X, EMPTY };
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

    bool playerFirst{};
    bool playerTurn{};
    bool gameEnd{};

    SetDifficulty diff{};
    std::array<CellStatus, boardSize> board{};
    std::vector<Object> objList{};
    std::uint8_t selectorRow{0};
    std::uint8_t selectorCDepth{0};
    std::vector<std::uint16_t> selectorYPos{};
    std::vector<std::uint16_t> selectorXPosD{}; // X-position based on depth.
    std::vector<AssetId> selectorAssetsFDepth{};
    std::size_t selectorObjIdx{};
    std::size_t labelObjIdx{};
    
    GameContext(Display &dsp, Audio &aud, Input &ipt, AssetRegistry &reg) : dsp{dsp}, aud{aud}, ipt{ipt}, reg{reg} {};
    std::size_t turnCount{};
    
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