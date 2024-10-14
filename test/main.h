#ifndef MAIN_H
#define MAIN_H

#include <string>
#include "config.h"
#include "logger.h"

// Utilities test functions
void runTrieTests();
void runRingBufferTests();
void runLoggerTests();

int runVideo(const std::string& videoPath);
int runCamera();

#endif // MAIN_H