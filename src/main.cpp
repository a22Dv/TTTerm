#include <iostream>
#include <chrono>
#include <thread>
#include "tctassets.hpp"
#include "tctaudio.hpp"

int main() {
    tct::AssetRegistry registry{};
    tct::Audio audio{};
    audio.playFileFade(registry.getAsset(tct::AssetId::TEST_MUSIC), 2.0f);
    std::this_thread::sleep_for(std::chrono::seconds{100});
    audio.stopFileFade(registry.getAsset(tct::AssetId::TEST_MUSIC), 2.0f);
     std::this_thread::sleep_for(std::chrono::seconds{2});
    std::cout << "Hello World!\n";
    return 0;
}