#include <string>
#include <thread>

// CUSTOM LIBRARIES
#include "Music.h"
#include "DArrray.h"
using namespace std;

void PlayBack(int currentSong, DArray *list, size_t sizeList)
{
    string musicdir = "Music";
    string name = list->access((size_t)currentSong);

    string state = "play";
    thread InputThread(InputFunction);
    while(state != "stop")
        {
            if(state == "next")
                {
                    if((size_t)currentSong + 1 >= sizeList) { currentSong = 0; }
                    else
                        {
                            currentSong += 1;
                        }
                    cout << currentSong << endl;
                    name = list->access(currentSong);
                    state = "play";
                }
            state = PlayMusic(name);
        }
    stop = true;
    InputThread.join();
}

int chooseMusic(size_t sizeList)
{
    int currentSong = 1;
    while(true)
        {
            cout << "enter your choice" << endl;
            cin >> currentSong;
            if(currentSong < 1 || currentSong > (int)sizeList)
                {
                    cout << "PLEASE ENTER A VALID INPUT" << endl;
                }
            else
                {
                    break;
                }
        }
    cin.ignore();
    return currentSong - 1;
}

int main()
{
    DArray list;
    string musicdir = "Music";

    listMusic(&list, musicdir);
    size_t sizeList = list.size();

    int currentSong = chooseMusic(sizeList);
    PlayBack(currentSong, &list, sizeList);
    stop = true;

    cout << "EXIT" << endl;

    return 0;
}
