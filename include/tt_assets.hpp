#include <stdint.h>

#include <cstddef>
#include <vector>

#define AUDIO_ASSET_LIST                                    \
    X(START, "assets\\audio\\start.wav")                    \
    X(QUIT, "assets\\audio\\quit.wav")                      \
    X(TRANSITION, "assets\\audio\\transition.wav")          \
    X(CLICK, "assets\\audio\\click.wav")                    \
    X(VICTORY, "assets\\audio\\victory.wav")                \
    X(DEFEAT, "assets\\audio\\defeat.wav")                  \
    X(MOVE1, "assets\\audio\\move1.wav")                    \
    X(MOVE2, "assets\\audio\\move2.wav")                    \
    X(MOVE3, "assets\\audio\\move3.wav")                    \
    X(AI_EASY_DIALOGUE, "assets\\audio\\ai_edlg.wav")       \
    X(AI_MEDIUM_DIALOGUE, "assets\\audio\\ai_mdlg.wav")     \
    X(AI_HARD_DIALOGUE, "assets\\audio\\ai_hdlg.wav")       \
    X(AI_UNBEATABLE_DIALOGUE, "assets\\audio\\ai_udlg.wav") \
    X(BGM_MAIN, "assets\\audio\\bgm_main.wav")              \
    X(BGM_EASY, "assets\\audio\\bgm_easy.wav")              \
    X(BGM_MEDIUM, "assets\\audio\\bgm_medium.wav")          \
    X(BGM_HARD, "assets\\audio\\bgm_hard.wav")              \
    X(BGM_UNBEATABLE, "assets\\audio\\bgm_unbeatable.wav")

#define IMAGE_ASSET_LIST                                   \
    X(PL_AVATAR, "assets\\images\\pl_avatar.png")          \
    X(AI_EASY_AVATAR, "assets\\images\\ai_eavt.png")       \
    X(AI_MEDIUM_AVATAR, "assets\\images\\ai_mavt.png")     \
    X(AI_HARD_AVATAR, "assets\\images\\ai_havt.png")       \
    X(AI_UNBEATABLE_AVATAR, "assets\\images\\ai_uavt.png") \
    X(BOARD_CHAR1, "assets\\images\\board_char1.png")      \
    X(BOARD_CHAR2, "assets\\images\\board_char2.png")      \
    X(BOARD_3X3, "assets\\images\\board_3x3.png")          \
    X(BOARD_4X4, "assets\\images\\board_4x4.png")          \
    X(TITLE, "assets\\images\\title.png")                  \
    X(PLAY_BUTTON, "assets\\images\\play_button.png")      \
    X(QUIT_BUTTON, "assets\\images\\quit_button.png")      \
    X(VICTORY_BANNER, "assets\\images\\vbanner.png")       \
    X(DEFEAT_BANNER, "assets\\images\\dbanner.png")

#define DIALOGUE_ASSET_LIST                                 \
    X(AI_EASY_INTRO, "assets\\dialogue\\ai_ei.txt")         \
    X(AI_EASY_D1, "assets\\dialogue\\ai_ed1.txt")           \
    X(AI_EASY_D2, "assets\\dialogue\\ai_ed2.txt")           \
    X(AI_EASY_D3, "assets\\dialogue\\ai_ed3.txt")           \
    X(AI_EASY_D4, "assets\\dialogue\\ai_ed4.txt")           \
    X(AI_EASY_D5, "assets\\dialogue\\ai_ed5.txt")           \
    X(AI_EASY_VICTORY, "assets\\dialogue\\ai_ev.txt")       \
    X(AI_EASY_DEFEAT, "assets\\dialogue\\ai_ed.txt")        \
    X(AI_MEDIUM_INTRO, "assets\\dialogue\\ai_mi.txt")       \
    X(AI_MEDIUM_D1, "assets\\dialogue\\ai_md1.txt")         \
    X(AI_MEDIUM_D2, "assets\\dialogue\\ai_md2.txt")         \
    X(AI_MEDIUM_D3, "assets\\dialogue\\ai_md3.txt")         \
    X(AI_MEDIUM_D4, "assets\\dialogue\\ai_md4.txt")         \
    X(AI_MEDIUM_D5, "assets\\dialogue\\ai_md5.txt")         \
    X(AI_MEDIUM_VICTORY, "assets\\dialogue\\ai_mv.txt")     \
    X(AI_MEDIUM_DEFEAT, "assets\\dialogue\\ai_md.txt")      \
    X(AI_HARD_INTRO, "assets\\dialogue\\ai_hi.txt")         \
    X(AI_HARD_D1, "assets\\dialogue\\ai_hd1.txt")           \
    X(AI_HARD_D2, "assets\\dialogue\\ai_hd2.txt")           \
    X(AI_HARD_D3, "assets\\dialogue\\ai_hd3.txt")           \
    X(AI_HARD_D4, "assets\\dialogue\\ai_hd4.txt")           \
    X(AI_HARD_D5, "assets\\dialogue\\ai_hd5.txt")           \
    X(AI_HARD_VICTORY, "assets\\dialogue\\ai_hv.txt")       \
    X(AI_HARD_DEFEAT, "assets\\dialogue\\ai_hd.txt")        \
    X(AI_UNBEATABLE_INTRO, "assets\\dialogue\\ai_ui.txt")   \
    X(AI_UNBEATABLE_D1, "assets\\dialogue\\ai_ud1.txt")     \
    X(AI_UNBEATABLE_D2, "assets\\dialogue\\ai_ud2.txt")     \
    X(AI_UNBEATABLE_D3, "assets\\dialogue\\ai_ud3.txt")     \
    X(AI_UNBEATABLE_D4, "assets\\dialogue\\ai_ud4.txt")     \
    X(AI_UNBEATABLE_D5, "assets\\dialogue\\ai_ud5.txt")     \
    X(AI_UNBEATABLE_VICTORY, "assets\\dialogue\\ai_uv.txt") \
    X(AI_UNBEATABLE_DEFEAT, "assets\\dialogue\\ai_ud.txt")

namespace ttt {

constexpr const char *const audioPaths[] = {
#define X(id, pth) pth,
    AUDIO_ASSET_LIST
#undef X
};

constexpr const char *const imagePaths[] = {
#define X(id, pth) pth,
    IMAGE_ASSET_LIST
#undef X
};

constexpr const char *const dialoguePaths[] = {
#define X(id, pth) pth,
    DIALOGUE_ASSET_LIST
#undef X
};

enum class AssetType : uint16_t { AUDIO, IMAGE, DIALOGUE };
enum class AssetFile : uint16_t {
#define X(id, pth) id,
    AUDIO_ASSET_LIST IMAGE_ASSET_LIST DIALOGUE_ASSET_LIST COUNT
#undef X
};

constexpr const char *const get_path(AssetType type, AssetFile uid) {
    constexpr const char *const *const paths[] = {audioPaths, imagePaths, dialoguePaths};
    return paths[static_cast<size_t>(type)][static_cast<size_t>(uid)];
}

class Asset {
   private:
    mutable std::vector<std::byte> _data;
    AssetType _type;
    AssetFile _uid;

   public:
    Asset(AssetType type, AssetFile uid) : _type{type}, _uid{uid}, _data{} {};
    AssetType get_type() const { return _type; };
    AssetFile get_uid() const { return _uid; };
    const std::vector<std::byte> &read_data() const;
};

} // namespace ttt