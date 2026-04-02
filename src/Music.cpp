#include <dirent.h>
#include <cstring>
#include <string>
#include <iostream>

#include <csignal>
#include <atomic>

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

#include "DArrray.h"

using namespace std;

atomic<bool> stop(false);

enum class Command
{
    NONE,
    PAUSE,
    PLAY,
    STOP,
    NEXT
};

atomic<Command> currentState(Command::NONE);

void listMusic(DArray *list, string musicdir = "")
{
    DIR *dir = opendir(".");
    int countMusic = 0;

    dirent *entry;
    while((entry = readdir(dir)) != nullptr)
        {
            if(entry->d_name[0] == '.') { continue; }

            if(strcmp(entry->d_name, musicdir.c_str()) == 0)
                {
                    cout << entry->d_name << "\n";
                    DIR *Music = opendir(musicdir.c_str());

                    while((entry = readdir(Music)) != nullptr)
                        {
                            if(entry->d_name[0] == '.') { continue; }

                            string songName(entry->d_name);

                            if(songName.substr(songName.length() - 4,
                                               songName.length() - 1) == ".wav")
                                {
                                    cout << countMusic + 1 << ")."
                                         << entry->d_name << endl;
                                    list->push(musicdir + "/" + entry->d_name);
                                    countMusic += 1;
                                }
                        }
                    closedir(Music);
                }
        }

    closedir(dir);
    cout << "Music count = " << countMusic << endl;
}

void handle_sigint(int) { stop = true; }

void InputFunction()
{
    string input;
    while(!stop.load())
        {
            getline(cin, input);
            if(input == "stop")
                {
                    currentState.store(Command::STOP);
                    fclose(stdin);
                }
            else if(input == "pause") { currentState.store(Command::PAUSE); }
            else if(input == "play") { currentState.store(Command::PLAY); }
            else if(input == "next") { currentState.store(Command::NEXT); }
        }
}

string PlayMusic(string name)
{
    signal(SIGINT, handle_sigint);

    SDL_Init(SDL_INIT_AUDIO);

    Uint32 wav_length;
    Uint8 *wav_buffer;
    SDL_AudioSpec wav_spec;

    if(SDL_LoadWAV(name.c_str(), &wav_spec, &wav_buffer, &wav_length) == NULL)
        {
            cout << "SONG DOES NOT EXITS" << endl;
            return "stop";
        }
    SDL_AudioDeviceID deviceId =
      SDL_OpenAudioDevice(NULL, 0, &wav_spec, NULL, 0);

    SDL_QueueAudio(deviceId, wav_buffer, wav_length);
    SDL_PauseAudioDevice(deviceId, 0);

    while(SDL_GetQueuedAudioSize(deviceId) > 0 && !stop)
        {
            if(currentState.load() == Command::STOP)
                {
                    SDL_CloseAudioDevice(deviceId);
                    SDL_FreeWAV(wav_buffer);
                    SDL_Quit();
                    stop = true;
                    return "stop";
                }
            else if(currentState.load() == Command::PAUSE)
                {
                    SDL_PauseAudioDevice(deviceId, 1);
                }
            else if(currentState.load() == Command::PLAY)
                {
                    SDL_PauseAudioDevice(deviceId, 0);
                }
            else if(currentState.load() == Command::NEXT)
                {
                    SDL_CloseAudioDevice(deviceId);
                    SDL_FreeWAV(wav_buffer);
                    SDL_Quit();
                    currentState.store(Command::NONE);
                    stop = false;
                    return "next";
                }
            SDL_Delay(10);  // small delay
        }
    SDL_CloseAudioDevice(deviceId);
    SDL_FreeWAV(wav_buffer);
    SDL_Quit();
    return "next";
}
