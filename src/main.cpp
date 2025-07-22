#include <iostream>

#include "tctaudio.hpp"

int main() {
    try {
        tct::Audio aud{};
        std::shared_ptr<tct::Asset> testAudio = std::make_shared<tct::Asset>(tct::Asset(tct::AssetID::TEST_AUDIO));
        aud.playFile(testAudio, tct::AudioFileType::MUSIC);
        std::this_thread::sleep_for(std::chrono::seconds(150));
        aud.stopFileFadeOut(testAudio);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Hello World!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what();
    }
}
