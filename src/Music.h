#include <dirent.h>
#include <cstring>
#include <string>
#include <iostream>
#include <atomic>

#include "DArrray.h"

#ifndef MUSIC_H
#define MUSIC_H

extern std::atomic<bool> stop;

void listMusic(DArray *list, std::string musicdir = "");
std::string PlayMusic(std::string name);
void handle_sigint(int);
void InputFunction();

#endif
