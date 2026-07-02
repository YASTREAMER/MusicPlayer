#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <atomic>
#include <cstring>
#include <dirent.h>
#include <iostream>
#include <string>

#include "DArrray.h"
#include "Music.h"

using namespace std;

// Definitions of the shared atomics declared in Music.h
atomic<bool> stop(false);
atomic<Command> currentState(Command::NONE);

void listMusic(DArray *list, string musicdir)
{
    DIR *dir = opendir(".");
    dirent *entry;
    int count = 0;

    while((entry = readdir(dir)) != nullptr)
        {
            if(entry->d_name[0] == '.') continue;

            if(strcmp(entry->d_name, musicdir.c_str()) == 0)
                {
                    DIR *mdir = opendir(musicdir.c_str());
                    while((entry = readdir(mdir)) != nullptr)
                        {
                            if(entry->d_name[0] == '.') continue;

                            string name(entry->d_name);
                            if(name.size() > 4 &&
                               name.substr(name.size() - 4) == ".wav")
                                {
                                    list->push(musicdir + "/" + name);
                                    count++;
                                }
                        }
                    closedir(mdir);
                }
        }
    closedir(dir);
}

void handle_sigint(int) { stop = true; }

