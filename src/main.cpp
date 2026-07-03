#include <atomic>
#include <csignal>
#include <string>
#include <thread>
#include <vector>

#include <sys/stat.h>
#include <unistd.h>

#include <vlc/libvlc.h>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include "DArrray.h"
#include "Music.h"

using namespace std;
using namespace ftxui;

// ── Shared state between UI thread and playback thread ────────────────────────
atomic<int> gCurrentSong(0);     // which song is playing right now
atomic<bool> gIsPlaying(false);  // paused or playing
atomic<bool>
  gUserSelected(false);  // true when user jumped to a song via Enter
                         // prevents PlaybackThread from auto-advancing

// ── Helpers ───────────────────────────────────────────────────────────────────
bool directoryExists(const string &path)
{
    struct stat info;
    if(stat(path.c_str(), &info) != 0) return false;
    return (info.st_mode & S_IFDIR);
}

// Strip directory prefix and .wav extension for clean display
string displayName(const string &path)
{
    size_t slash = path.rfind('/');
    string name = (slash != string::npos) ? path.substr(slash + 1) : path;
    if((name.size() > 4 && name.substr(name.size() - 4) == ".wav") ||
       (name.size() > 4 && name.substr(name.size() - 4) == ".mp3") ||
       (name.size() > 5 && name.substr(name.size() - 5) == ".flac"))
        name = name.substr(0, name.size() - 4);
    return name;
}

// ── Playback thread ───────────────────────────────────────────────────────────
void PlaybackThread(libvlc_instance_t *inst, DArray *list, size_t sizeList,
                    ScreenInteractive *screen)
{
    while(!stop.load())
        {
            int idx = gCurrentSong.load();
            if((size_t)idx >= sizeList) idx = 0;

            gIsPlaying.store(true);
            screen->PostEvent(Event::Custom);  // refresh UI to show new song

            string result = PlayMusic(inst, list->access((size_t)idx));

            if(stop.load()) break;

            if(result == "prev")
                {
                    int prev = (idx - 1 + (int)sizeList) % (int)sizeList;
                    gCurrentSong.store(prev);
                }

            // Song finished or skipped — decide what plays next
            else if(!gUserSelected.exchange(false))
                {
                    // Natural end or [N] key: advance to next in playlist
                    int next = (idx + 1) % (int)sizeList;
                    gCurrentSong.store(next);
                }
            // If gUserSelected was true, gCurrentSong is already set by the UI
            // thread

            currentState.store(Command::NONE);
            screen->PostEvent(Event::Custom);
        }

    gIsPlaying.store(false);
    screen->PostEvent(Event::Custom);
}

int main()
{
    libvlc_instance_t *inst = libvlc_new(0, NULL);
    if(inst == NULL) { return -1; }
    DArray list;
    string musicdir = "Music";

    if(!directoryExists(musicdir)) { return -1; }

    signal(SIGINT, handle_sigint);
    listMusic(&list, musicdir);

    size_t sizeList = list.size();
    if(sizeList == 0) return -1;

    // Pre-compute display names once
    vector<string> names;
    for(size_t i = 0; i < sizeList; i++)
        names.push_back(displayName(list.access(i)));

    auto screen = ScreenInteractive::Fullscreen();

    int navIdx = 0;  // keyboard browse cursor (independent of playing song)
    auto formatTime = [](int ms) -> string {
        if(ms < 0) ms = 0;
        int secs = ms / 1000;
        int mins = secs / 60;
        secs = secs % 60;
        char buf[8];
        snprintf(buf, sizeof(buf), "%d:%02d", mins, secs);
        return string(buf);
    };

    // ── Renderer ─────────────────────────────────────────────────────────────
    auto ui = Renderer([&] {
        int cur = gCurrentSong.load();
        bool playing = gIsPlaying.load();

        string curName = (cur < (int)sizeList) ? names[cur] : "—";
        string statusText = playing ? " ▶  Playing" : " ⏸  Paused";
        Color statusCol = playing ? Color::Green : Color::Yellow;

        // ── Playlist rows ─────────────────────────────────────────────────────
        Elements rows;
        for(int i = 0; i < (int)sizeList; i++)
            {
                bool isCur = (i == cur);
                bool isNav = (i == navIdx);

                string prefix = isCur ? " ♪ " : "   ";
                string num = to_string(i + 1) + ".  ";

                auto row = hbox({text(prefix), text(num), text(names[i])});

                if(isCur && isNav)
                    row = row | color(Color::Green) | bold | inverted;
                else if(isCur)
                    row = row | color(Color::Green) | bold;
                else if(isNav)
                    row = row | inverted;

                rows.push_back(row);
            }

        // ── Layout ────────────────────────────────────────────────────────────
        return vbox({
          // Header
          text(" ♫  C++ Music Player ") | bold | center | color(Color::Cyan),
          separator(),

          // Now playing
          vbox({
            hbox({text("  Now Playing : ") | bold,
                  text(curName) | color(Color::Green) | bold}),
            hbox({text("  Status      : ") | bold,
                  text(statusText) | color(statusCol)}),
            hbox({
              text("  Volume  : ") | bold,
              gauge((float)gVolume.load() / 100.0f) | flex | color(Color::Cyan),
              text(" " + to_string(gVolume.load()) + "%") | bold,
            }),
            hbox({
              text("  " + formatTime(gCurrentTime.load())) | bold,
              text(" "),
              gauge(gTotalTime.load() > 0
                      ? (float)gCurrentTime.load() / (float)gTotalTime.load()
                      : 0.0f) |
                flex | color(Color::Cyan),
              text(" " + formatTime(gTotalTime.load()) + " ") | bold,
            }),
          }),
          separator(),

          // Playlist (scrollable)
          text("  Playlist") | bold,
          vbox(rows) | frame | flex,
          separator(),

          // Controls legend
          hbox({
            text(" [Space] Pause/Play") | color(Color::Yellow),
            text("  [K/J] Browse") | color(Color::Yellow),
            text("  [Enter] Jump") | color(Color::Yellow),
            text("  [N] Skip") | color(Color::Yellow),
            text("  [P] Prev") | color(Color::Yellow),
            text("  [Q] Quit ") | color(Color::Yellow),
            text("  [U/O] Volume") | color(Color::Yellow),
          }) |
            center,
        });
    });

    // ── Keyboard events ───────────────────────────────────────────────────────
    auto component = CatchEvent(ui, [&](Event e) -> bool {
        // Browse playlist up / down
        if(e == Event::Character('k') || e == Event::Character('K'))
            {
                navIdx = max(0, navIdx - 1);
                return true;
            }
        if(e == Event::Character('j') || e == Event::Character('J'))
            {
                navIdx = min((int)sizeList - 1, navIdx + 1);
                return true;
            }
        if(e == Event::Character('u') || e == Event::Character('U'))
            {
                int newVol = min(100, gVolume.load() + 5);
                gVolume.store(newVol);
                currentState.store(Command::VOLUMEUP);
                screen.PostEvent(Event::Custom);
                return true;
            }
        if(e == Event::Character('o') || e == Event::Character('O'))
            {
                int newVol = max(0, gVolume.load() - 5);
                gVolume.store(newVol);
                currentState.store(Command::VOLUMEDOWN);
                screen.PostEvent(Event::Custom);
                return true;
            }

        // Enter — jump to browsed song
        if(e == Event::Return)
            {
                gCurrentSong.store(navIdx);
                gUserSelected.store(true);  // tell thread not to auto-advance
                currentState.store(Command::NEXT);
                gIsPlaying.store(true);
                return true;
            }

        // Space — pause / resume
        if(e == Event::Character(' '))
            {
                if(gIsPlaying.load())
                    {
                        currentState.store(Command::PAUSE);
                        gIsPlaying.store(false);
                    }
                else
                    {
                        currentState.store(Command::PLAY);
                        gIsPlaying.store(true);
                    }
                return true;
            }

        // N — skip to next song
        if(e == Event::Character('n') || e == Event::Character('N'))
            {
                // gUserSelected stays false → thread auto-advances to idx+1
                currentState.store(Command::NEXT);
                gIsPlaying.store(true);
                return true;
            }
        // P - skip to previous song
        if(e == Event::Character('p') || e == Event::Character('P'))
            {
                currentState.store(Command::PREV);
                return true;
            }

        // Q — quit
        if(e == Event::Character('q') || e == Event::Character('Q'))
            {
                stop.store(true);
                currentState.store(Command::STOP);
                screen.ExitLoopClosure()();
                return true;
            }

        return false;
    });

    // Start playback in background before entering the UI loop
    thread playThread(PlaybackThread, inst, &list, sizeList, &screen);
    thread timerThread([&] {
        while(!stop.load())
            {
                screen.PostEvent(Event::Custom);
                usleep(500000);  // refresh every 500ms
            }
    });

    screen.Loop(component);

    // Cleanup
    stop.store(true);
    currentState.store(Command::STOP);
    if(playThread.joinable()) playThread.join();
    if(timerThread.joinable()) timerThread.join();
    libvlc_release(inst);
    return 0;
}
