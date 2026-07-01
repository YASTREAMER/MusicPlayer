#ifndef MUSIC_H
#define MUSIC_H

#include <atomic>
#include <string>

#include "DArrray.h"

// Shared between playback engine (Music.cpp) and UI (main.cpp)
enum class Command
{
    NONE,
    PAUSE,
    PLAY,
    STOP,
    NEXT
};

extern std::atomic<bool> stop;
extern std::atomic<Command> currentState;

void listMusic(DArray *list, std::string musicdir = "");
std::string PlayMusic(std::string name);
void handle_sigint(int);

#endif
