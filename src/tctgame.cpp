#include "tctgame.hpp"
#include "tctassets.hpp"
#include "tctdisplay.hpp"
#include "tctinput.hpp"
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <random>
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
        // Now obviously this isn't how it should work. But its because this game is just
        // the menu and the game itself that it's fine if I re-init every single time.
        if (sceneCount != sceneStack.size()) {
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
    context.objList.clear();

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
        context.aud.stopFileFade(context.reg.getAsset(AssetId::MENU_BGM));
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
    : gameInstance{game}, context{GameContext(game.getDsp(), game.getAud(), game.getInput(), game.getRegistry())} {
    context.diff = diff;
    context.playerFirst = pFirst;
};

void GameScene::init() {
    context.objList.clear();
    context.objList.push_back(Object{AssetId::BORDER, Vector2{0, 0}});
    context.objList.push_back(Object{AssetId::VERSION, Vector2{210, 110}});
    context.objList.push_back(Object{AssetId::BOARD, Vector2{76, 18}});
    context.objList.push_back(Object{
        [this]() {
            switch (context.diff) {
            case SetDifficulty::AI_EASY: return AssetId::EASY;
            case SetDifficulty::AI_NORMAL: return AssetId::NORMAL;
            case SetDifficulty::AI_HARD: return AssetId::HARD;
            case SetDifficulty::AI_UNBEATABLE: return AssetId::UNBEATABLE;
            };
        }(),
        Vector2{6, 6}
    });

    context.selectorYPos.push_back(26);
    context.selectorYPos.push_back(56);
    context.selectorYPos.push_back(84);

    context.selectorXPosD.push_back(61);
    context.selectorXPosD.push_back(49);
    context.selectorXPosD.push_back(37);

    context.selectorAssetsFDepth.push_back(AssetId::SLCT_D1);
    context.selectorAssetsFDepth.push_back(AssetId::SLCT_D2);
    context.selectorAssetsFDepth.push_back(AssetId::SLCT_D3);

    context.objList.push_back(Object{AssetId::SLCT_D1, Vector2{context.selectorXPosD[0], context.selectorYPos[0]}});
    context.selectorObjIdx = context.objList.size() - 1;
    context.objList.push_back([this]() {
        if (context.playerFirst) {
            return Object{AssetId::TURN_PL, Vector2{101, 6}};
        } else {
            return Object{AssetId::TURN_AI, Vector2{105, 6}};
        }
    }());
    context.labelObjIdx = context.objList.size() - 1;
    context.playerTurn = context.playerFirst;
    std::fill(context.board.begin(), context.board.end(), CellStatus::EMPTY);
    context.aud.playFileFade(
        context.reg.getAsset([this] {
            switch (context.diff) {
            case SetDifficulty::AI_EASY: return AssetId::EASY_BGM;
            case SetDifficulty::AI_NORMAL: return AssetId::NORMAL_BGM;
            case SetDifficulty::AI_HARD: return AssetId::HARD_BGM;
            case SetDifficulty::AI_UNBEATABLE: return AssetId::UNBEATABLE_BGM;
            }
        }()),
        1.0f, 0.3f, true
    );
}

namespace {

void executeTurn(Vector2 pos, GameContext &ctx) {
    bool success{true};
    Vector2 coord{ctx.selectorCDepth, ctx.selectorRow};
    CellStatus &cellState = ctx.board[coord.y * boardSideLength + coord.x];
    success = cellState == CellStatus::EMPTY;
    if (success) {
        ctx.turnCount & 1 ? cellState = CellStatus::O : cellState = CellStatus::X;
        ctx.playerTurn = false;
        ctx.turnCount++;
    }
    ctx.aud.playFile(ctx.reg.getAsset(success ? AssetId::ENTER_MENU : AssetId::ERROR_SFX));
    ctx.selectorCDepth = 0;
    ctx.selectorRow = 0;
}

void easy(GameContext &ctx) {
    static std::random_device rd{};
    static std::mt19937 mersenne{rd()};
    static std::uniform_int_distribution<std::uint16_t> dist{0, 8};
    bool moveSpent{false};
    std::size_t lastValid{};
    for (std::size_t i = 0; i < boardSize; ++i) {
        CellStatus &cell = ctx.board[i];
        if (cell != CellStatus::EMPTY) {
            continue;
        }
        if (dist(mersenne) == 0) {
            ctx.turnCount & 1 ? cell = CellStatus::O : cell = CellStatus::X;
            moveSpent = true;
        }
        lastValid = i;
        if (moveSpent) {
            break;
        }
    }
    if (!moveSpent) {
        ctx.turnCount & 1 ? ctx.board[lastValid] = CellStatus::O : ctx.board[lastValid] = CellStatus::X;
    }
}

enum class BoardState : std::uint8_t { PL_VICTORY, PL_DEFEAT, TIE, UNDECIDABLE };

std::int16_t minimax(
    std::array<CellStatus, boardSize> &boardReference, uint8_t currentDepth, std::uint8_t depthLimit, bool maximizer
) {

};

/*
    These functions assume they will be called on a valid board. Calling it on a
    full board or a game that has already concluded will either overwrite cells or
    behave weirdly.
*/

void normal(GameContext &ctx) {
    std::int16_t maxScore = INT16_MIN;
    std::size_t bestMove = 0;
    const CellStatus aiChar{ctx.turnCount & 1 ? CellStatus::O : CellStatus::X};
    for (std::size_t i = 0; i < boardSize; ++i) {
        CellStatus &cell = ctx.board[i];
        if (cell != CellStatus::EMPTY) {
            continue;
        }
        cell = aiChar;
        std::int16_t score = minimax(ctx.board, 0, 1, false);
        cell = CellStatus::EMPTY;
        if (maxScore > score) {
            continue;
        }
        maxScore = score;
        bestMove = i;
    }
    ctx.board[bestMove] = aiChar;
}

void hard(GameContext &ctx) {}

void unbeatable(GameContext &ctx) {}

void ai_move(GameContext &ctx) {
    switch (ctx.diff) {
    case SetDifficulty::AI_EASY: easy(ctx);
    case SetDifficulty::AI_NORMAL: normal(ctx);
    case SetDifficulty::AI_HARD: hard(ctx);
    case SetDifficulty::AI_UNBEATABLE: unbeatable(ctx);
    }
    ctx.turnCount++;
    ctx.playerTurn = true;
}

BoardState evaluateBoard(std::array<CellStatus, boardSize> &board, bool plFirst) {
    bool hitBreak{};
    const CellStatus plCell{plFirst ? CellStatus::X : CellStatus::O};
    for (std::size_t i = 0; i < boardSideLength; ++i) {
        // Check row/columns.
        std::size_t rowOffset = boardSideLength * i;
        if ((board[rowOffset] == board[rowOffset + 1] && board[rowOffset + 1] == board[rowOffset + 2] &&
             board[rowOffset] != CellStatus::EMPTY)) {
            return board[rowOffset] == plCell ? BoardState::PL_VICTORY : BoardState::PL_DEFEAT;
        }
        if ((board[i] == board[3 + i] && board[3 + i] == board[6 + i] && board[i] != CellStatus::EMPTY)) {
            return board[i] == plCell ? BoardState::PL_VICTORY : BoardState::PL_DEFEAT;
        }
    }
    // Check diagonal.
    if ((board[0] == board[4] && board[4] == board[8] || board[2] == board[4] && board[4] == board[6]) &&
        board[4] != CellStatus::EMPTY) {
        return board[4] == plCell ? BoardState::PL_VICTORY : BoardState::PL_DEFEAT;
    }
    // Check for fill.
    for (CellStatus st : board) {
        if (st == CellStatus::EMPTY) {
            hitBreak = true;
            break;
        }
    }
    if (!hitBreak) {
        return BoardState::TIE;
    }
    return BoardState::UNDECIDABLE;
}

void isFinished(GameContext &context) {
    BoardState eval{evaluateBoard(context.board, context.playerFirst)};
    if (eval != BoardState::UNDECIDABLE) {
        context.aud.stopTypeFade(AudioType::MUSIC);
        context.objList.push_back(Object{AssetId::END_BANNER, Vector2{0, 33}});
        if (eval == BoardState::PL_VICTORY) {
            context.objList.push_back(Object{AssetId::VICTORY, Vector2{79, 52}});
            context.aud.playFile(context.reg.getAsset(AssetId::VICTORY_SFX));
        } else if (eval == BoardState::PL_DEFEAT) {
            context.objList.push_back(Object{AssetId::DEFEAT, Vector2{85, 52}});
            context.aud.playFile(context.reg.getAsset(AssetId::DEFEAT_SFX));
        } else {
            context.objList.push_back(Object{AssetId::TIE, Vector2{102, 52}});
            context.aud.playFile(context.reg.getAsset(AssetId::TIE_SFX));
        }
        context.gameEnd = true;
    }
}

} // namespace

void GameScene::update() {
    InputKey input = context.ipt.pollInput();
    if (!context.gameEnd) {
        if (!context.playerTurn) {
            ai_move(context);
            return;
        }
        switch (input) {
        case InputKey::ENTER: executeTurn(Vector2{context.selectorCDepth, context.selectorRow}, context); break;
        case InputKey::ARROW_DOWN: context.selectorRow = std::min(context.selectorRow + 1, 2); break;
        case InputKey::ARROW_LEFT: context.selectorCDepth = std::max(context.selectorCDepth - 1, 0); break;
        case InputKey::ARROW_UP: context.selectorRow = std::max(context.selectorRow - 1, 0); break;
        case InputKey::ARROW_RIGHT: context.selectorCDepth = std::min(context.selectorCDepth + 1, 2); break;
        case InputKey::NONE: break;
        }
        if (input != InputKey::NONE && input != InputKey::ENTER) {
            context.aud.playFile(context.reg.getAsset(AssetId::SELECT_MENU));
        }
        Object &selector = context.objList[context.selectorObjIdx];
        selector.id = context.selectorAssetsFDepth[context.selectorCDepth];
        selector.pos =
            Vector2{context.selectorXPosD[context.selectorCDepth], context.selectorYPos[context.selectorRow]};
        isFinished(context);

        // Re-check condition as is-finished changes it.
        if (!context.gameEnd) {
            Object &lbl = context.objList[context.labelObjIdx];
            if (context.playerTurn) {
                lbl.id = AssetId::TURN_PL;
                lbl.pos = Vector2{98, 6};
            } else {
                lbl.id = AssetId::TURN_AI;
                lbl.pos = Vector2{102, 6};
            }
        }

    } else {
        if (input == InputKey::ENTER) {
            gameInstance.popScene();
            return;
        }
    }
}

void GameScene::render() {
    static const std::array<Vector2, boardSize> boardCellRenderCoords{
        Vector2{77, 19},  Vector2{106, 19}, Vector2{135, 19}, Vector2{77, 48}, Vector2{106, 48},
        Vector2{135, 48}, Vector2{77, 77},  Vector2{106, 77}, Vector2{135, 77}
    };

    Display &dsp = context.dsp;
    AssetRegistry &reg = context.reg;
    const std::size_t renderCount = context.objList.size();
    for (std::size_t i = 0; i < renderCount; ++i) {
        Object &obj = context.objList[i];
        dsp.render(RenderRequest{reg.getAsset(obj.id), obj.pos});
    }
    for (std::size_t i = 0; i < boardSize; ++i) {
        CellStatus cell{context.board[i]};
        switch (cell) {
        case CellStatus::O: dsp.render(RenderRequest{reg.getAsset(AssetId::O_CHAR), boardCellRenderCoords[i]}); break;
        case CellStatus::X: dsp.render(RenderRequest{reg.getAsset(AssetId::X_CHAR), boardCellRenderCoords[i]}); break;
        case CellStatus::EMPTY: break;
        }
    }

    // Temporary fix to layering issue when board is rendered on top of end banner.
    if (context.gameEnd) {
        for (std::size_t i = 0; i < renderCount; ++i) {
            Object &obj = context.objList[i];
            if (obj.id == AssetId::END_BANNER || obj.id == AssetId::VICTORY || obj.id == AssetId::DEFEAT ||
                obj.id == AssetId::TIE) {
                dsp.render(RenderRequest{reg.getAsset(obj.id), obj.pos});
            }
        }
    }

    // Board render.
    dsp.present();
}

} // namespace tct
