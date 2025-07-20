#pragma once
#include <atomic>
#include <thread>

#include "tt_assets.hpp"

namespace ttt {

class Display {
   private:
    bool running_thread{};

   public:
    Display();
    void exec_dthread();
    ~Display();
};

class Audio {
   private:
    bool running_thread{};

   public:
    Audio();
    void exec_athread();
    void play_file(const AssetFile aFile);
    void stop_file(const AssetFile aFile);
    void start();
    void stop();
    ~Audio();
};

class Application {
   private:
    std::atomic<bool> terminate{};
    Display dsp{};
    Audio aud{};
    std::thread dspThread{};
    std::thread audThread{};

   public:
    Application();
    void exec_app();
    ~Application();
};

} // namespace ttt
