#include <iostream>

#include "tctaudio.hpp"
#include "tctdisplay.hpp"

int main() {
    try {
        tct::Audio aud{};
        tct::Display dsp{};
        std::shared_ptr<tct::Asset> testAudio = std::make_shared<tct::Asset>(tct::Asset(tct::AssetID::TEST_AUDIO));
        std::shared_ptr<tct::Asset> testDonut = std::make_shared<tct::Asset>(tct::Asset(tct::AssetID::TEST_IMAGE));
        
        aud.playFile(testAudio, tct::AudioFileType::MUSIC);
        while (true) {
            dsp.render(std::vector<tct::Renderable>{tct::Renderable { .position{tct::Vector2D {.x{40}, .y{10}}}, .asset{testDonut} }});
            std::this_thread::sleep_for(std::chrono::milliseconds(33));
        }
        
        aud.stopFileFadeOut(testAudio);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Hello World!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what();
    }
}
