#include <cstddef>
#include <cstring>
#include <iostream>
#include <string>
#include "DArrray.h"

using namespace std;

void DArray::reSize()
{
    string *tempMusic = new string[capacity * 2];

    for(size_t i = 0; i < currentSize; i++) { tempMusic[i] = listMusic[i]; }

    delete[] listMusic;
    listMusic = tempMusic;
    capacity = capacity * 2;
}
void DArray::push(string element)
{
    if(currentSize >= capacity) { reSize(); }

    listMusic[currentSize] = element;
    currentSize += 1;
}

// TODO: Create a way for contious update of the list of Music
string DArray::access(size_t index)
{
    if(index >= currentSize)
        {
            cout << "OUT OF SCOPE" << endl;
            return "";
        }
    return listMusic[index];
}

DArray::~DArray() { delete[] listMusic; }

DArray::DArray()
{
    capacity = 1;
    currentSize = 0;
    listMusic = new string[capacity];
}

size_t DArray::size() { return currentSize; }
