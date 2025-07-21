#include <iostream>

#include "tctaudio.hpp"

int main() {
    try {
        tct::Audio aud{};
        aud.playFile(tct::AudioFileID::TEST, tct::AudioFileType::MUSIC);
        std::this_thread::sleep_for(std::chrono::seconds(100));
        aud.stopFileFadeOut(tct::AudioFileID::TEST);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Hello World!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what();
    }
}
