#include "math.hpp"

#include <chrono>

#include "definitions.hpp"

#ifdef _WIN32
    #include <windows.h>
#endif

// credit to https://gist.github.com/Youka/4153f12cf2e17a77314c
bool nanosleepWin(long long microseconds) {
    #ifdef _WIN32
        // Declarations
        HANDLE timer;	// Timer handle
        LARGE_INTEGER li;	// Time defintion
        // Create timer
        if (!(timer = CreateWaitableTimer(NULL, TRUE, NULL))) {
            return false;
        }
        // Set timer properties
        li.QuadPart = -(10 * microseconds);
        if (!SetWaitableTimer(timer, &li, 0, NULL, NULL, FALSE)) {
            CloseHandle(timer);
            return false;
        }
        // Start & wait for timer
        WaitForSingleObject(timer, INFINITE);
        // Clean resources
        CloseHandle(timer);
        // Slept without problems
        return true;
    #endif

    return false;
}


// public

void Math::sleep(double seconds) {
    while (seconds > 0.002) { // takes "up to" 2 ms when sleeping for 1 ms
        auto start = std::chrono::high_resolution_clock::now();

        #ifdef __linux__
            struct timespec t = {0, 950 * 1000}, r;
            nanosleep(&t, &r); // sleep for ~ 1 ms
        #endif
        #ifdef _WIN32
            nanosleepWin(1000); // sleep ~ 1 ms (1000 us), but it varies up a bit
        #endif

        auto end = std::chrono::high_resolution_clock::now();
        double observed = (end - start).count() / 1e9;
        seconds -= observed;
    }
    // spin lock
    auto start = std::chrono::high_resolution_clock::now();
    auto spinNs = int64_t(seconds * 1e9);
    auto delay = std::chrono::nanoseconds(spinNs);
    while (std::chrono::high_resolution_clock::now() - start < delay);
}

Math::Math() {
    _targetTickSpeed = 60;
}

int Math::SetTickSpeed(double tickSpeed) {
    _targetTickSpeed = tickSpeed;
    return SUCCESS;
}

void Math::TickStart() {
    _tickStart = std::chrono::steady_clock::now();
}

void Math::TickEnd() {
    _tickEnd = std::chrono::steady_clock::now();
}

void Math::TickEndAndSleep() {
    _tickEnd = std::chrono::steady_clock::now();
    double tickDuration = (_tickEnd - _tickStart).count() / 1e9;
    double totalDuration = (1.0 / _targetTickSpeed);
    sleep(totalDuration - tickDuration);
}
