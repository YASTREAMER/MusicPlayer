#include <vlc/libvlc.h>
#include <vlc/libvlc_renderer_discoverer.h>
#include <vlc/libvlc_media.h>
#include <vlc/libvlc_media_player.h>

#include <stdio.h>
#include <unistd.h>

#include <atomic>
#include <cstring>
#include <dirent.h>
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
                                }
                        }
                    closedir(mdir);
                }
        }
    closedir(dir);
}

void handle_sigint(int) { stop = true; }

string PlayMusic(libvlc_instance_t *inst, string name)
{
    // 1. Create media object for this song
    libvlc_media_t *media = libvlc_media_new_path(inst, name.c_str());
    if(media == NULL) { return "next"; }  // skip missing files gracefully

    // 2. Create player from media, then release media
    //    (player holds its own reference)
    libvlc_media_player_t *player = libvlc_media_player_new_from_media(media);
    libvlc_media_release(media);

    // 3. Start playback
    libvlc_media_player_play(player);

    // 4. Wait for playback to actually start before checking state
    sleep(1);

    // 5. Main playback loop — mirrors your old SDL loop
    while(1)
        {
            libvlc_state_t state = libvlc_media_player_get_state(player);

            // Song finished naturally
            if(state == libvlc_Ended || state == libvlc_Error)
                {
                    libvlc_media_player_stop(player);
                    libvlc_media_player_release(player);
                    return "next";
                }

            // Check commands from UI thread
            Command cmd = currentState.load();

            if(cmd == Command::STOP)
                {
                    libvlc_media_player_stop(player);
                    libvlc_media_player_release(player);
                    stop.store(true);
                    return "stop";
                }
            else if(cmd == Command::NEXT)
                {
                    libvlc_media_player_stop(player);
                    libvlc_media_player_release(player);
                    return "next";
                }
            else if(cmd == Command::PAUSE)
                {
                    libvlc_media_player_set_pause(player, 1);
                }
            else if(cmd == Command::PLAY)
                {
                    libvlc_media_player_set_pause(player, 0);
                }

            usleep(10000);  // 10ms — same as your old SDL_Delay(10)
        }
}
