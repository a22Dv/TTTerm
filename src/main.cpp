#include <algorithm>
#include <atomic>
#include <cstddef>
#include <functional>
#include <iostream>
#include <span>
#include <thread>

typedef struct state {
    std::atomic<bool> shutdown;
} state;

void wthread(const state& st) {
    std::cout << "Hello from worker thread!\n";
    while (true) {
        if (st.shutdown.load()) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main(int argc, char** argv) {
    state st = {st.shutdown = false};
    std::thread thread = std::thread(wthread, std::ref(st));

    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Hello from main thread!\n";
    st.shutdown.store(true);
    thread.join();
    std::vector<int> h{1, 2, 3};

    return 0;
}
