#include <iostream>

#include "body.hpp"
#include "definitions.hpp"
#include "window.hpp"

int main(int argc, char* argv[]) {
    int failVal = 0;

    Window window;
    if ((failVal = window.Setup()) < SUCCESS) {
        return failVal;
    }

    return 0;
}
