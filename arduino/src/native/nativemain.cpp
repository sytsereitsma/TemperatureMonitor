#if __AVR__
static_assert (false, "Now why would you want to build this file for AVR?");
#endif

#include <iostream>
#include <cstdio>
#include <thread>
#include <atomic>

extern void setup();
extern void loop();

namespace {
    void arduino(std::atomic_bool& inKeepRunning) {
        setup();
        
        while(inKeepRunning) {
            loop();
        }
    }
}

int main (int argc, char** argv) {
    std::cout << "Starting native temperature monitor application."<< std::endl;

    std::atomic_bool keepRunning { true };
    std::thread arduinoThread (arduino, std::ref(keepRunning));
    std::string command;
    while(keepRunning) {
        std::cin >> command;
        if (command == "Q" || command == "q") {
            keepRunning = false;
        }
    }

    if(arduinoThread.joinable()) {
        arduinoThread.join();
    }

    std::cout << "Bye"<< std::endl;
    return 0;
}
