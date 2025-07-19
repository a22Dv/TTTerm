#include <stdint.h>

namespace TicTacToe {

enum class AudioFile : uint16_t {
    START,
    QUIT,
    TRANSITION,
    BUTTON_CLICK,
    VICTORY,
    DEFEAT,
    MOVE1,
    MOVE2,
    MOVE3,
    AI_EASY_DIALOGUE,
    AI_MEDIUM_DIALOGUE,
    AI_HARD_DIALOGUE,
    AI_UNBEATABLE_DIALOGUE,
    BGM_MAIN,
    BGM_EASY,
    BGM_MEDIUM,
    BGM_HARD,
    BGM_UNBEATABLE,
    COUNT,
};

enum class ImageFile : uint16_t {
    PL_AVATAR,
    AI_EASY_AVATAR,
    AI_MEDIUM_AVATAR,
    AI_HARD_AVATAR,
    AI_UNBEATABLE_AVATAR,
    BOARD_CHAR_1,
    BOARD_CHAR_2,
    BOARD3X3,
    BOARD4X4,
    TITLE,
    PLAY_BUTTON,
    VICTORY,
    DEFEAT,
    COUNT
};

/// @brief About "D[N]" files, each file holds 10 dialogue
/// lines to choose from. The higher the N is the chance the AI
/// thinks it is winning.
enum class DialogueFile : uint16_t {
    AI_EASY_INTRO,
    AI_EASY_D1,
    AI_EASY_D2,
    AI_EASY_D3,
    AI_EASY_D4,
    AI_EASY_D5,
    AI_EASY_VICTORY,
    AI_EASY_DEFEAT,
    AI_MEDIUM_INTRO,
    AI_MEDIUM_D1,
    AI_MEDIUM_D2,
    AI_MEDIUM_D3,
    AI_MEDIUM_D4,
    AI_MEDIUM_D5,
    AI_MEDIUM_VICTORY,
    AI_MEDIUM_DEFEAT,
    AI_HARD_INTRO,
    AI_HARD_D1,
    AI_HARD_D2,
    AI_HARD_D3,
    AI_HARD_D4,
    AI_HARD_D5,
    AI_HARD_VICTORY,
    AI_HARD_DEFEAT,
    AI_UNBEATABLE_INTRO,
    AI_HARD_INTRO,
    AI_HARD_D1,
    AI_HARD_D2,
    AI_HARD_D3,
    AI_HARD_D4,
    AI_HARD_D5,
    AI_HARD_VICTORY,
    AI_HARD_DEFEAT,
};

} // namespace TicTacToe