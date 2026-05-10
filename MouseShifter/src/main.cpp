#include <iostream>
#include "MouseShifter.h"

int main() {
    // Set locale for console support
    setlocale(LC_ALL, "");
    
    try {
        MouseShifter shifter;
        shifter.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
