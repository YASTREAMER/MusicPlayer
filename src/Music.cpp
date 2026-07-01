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

string PlayMusic(string name)
{
    SDL_Init(SDL_INIT_AUDIO);

    Uint32 wav_length;
    Uint8 *wav_buffer;
    SDL_AudioSpec wav_spec;

    if(SDL_LoadWAV(name.c_str(), &wav_spec, &wav_buffer, &wav_length) == NULL)
        {
            SDL_Quit();
            return "next";  // skip broken / missing files gracefully
        }

    SDL_AudioDeviceID deviceId =
      SDL_OpenAudioDevice(NULL, 0, &wav_spec, NULL, 0);
    SDL_QueueAudio(deviceId, wav_buffer, wav_length);
    SDL_PauseAudioDevice(deviceId, 0);

    while(SDL_GetQueuedAudioSize(deviceId) > 0 && !stop)
        {
            Command cmd = currentState.load();

            if(cmd == Command::STOP)
                {
                    SDL_CloseAudioDevice(deviceId);
                    SDL_FreeWAV(wav_buffer);
                    SDL_Quit();
                    stop = true;
                    return "stop";
                }
            else if(cmd == Command::NEXT)
                {
                    SDL_CloseAudioDevice(deviceId);
                    SDL_FreeWAV(wav_buffer);
                    SDL_Quit();
                    return "next";
                }
            else if(cmd == Command::PAUSE)
                {
                    SDL_PauseAudioDevice(deviceId, 1);
                }
            else if(cmd == Command::PLAY) { SDL_PauseAudioDevice(deviceId, 0); }

            SDL_Delay(10);
        }

    SDL_CloseAudioDevice(deviceId);
    SDL_FreeWAV(wav_buffer);
    SDL_Quit();
    return "next";
}
