#pragma once

#include <string>
#include "frame.h"

class Display {
public:
    Display();
    ~Display();

    void showFrame(const Frame& frame);
    void close();

private:
    std::string windowName;
};