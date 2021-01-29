#include <iostream>

#include "Application.h"

int main(int argc, char **argv) {
    try {
        Application app;
        app.run();
    } catch (const std::exception &e) {
        std::cout << "E: " << e.what() << std::endl;
    }

    return 0;
}

