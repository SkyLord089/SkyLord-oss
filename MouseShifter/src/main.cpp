#include "MouseShifter.h"
#include <iostream>

int main() {
    try {
        MouseShifter shifter;
        shifter.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
