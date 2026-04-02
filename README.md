# 🎵 Terminal-Based Music Player (C++)

A lightweight terminal-based music player built in C++ with a focus on low-level system programming, performance, and control over system resources. The project demonstrates the use of POSIX-compliant APIs, custom data structures, and SDL2 for audio playback.

---

## 🚀 Features

- ▶️ Play, Pause, Stop, and Next controls  
- 🔁 Automatic playback of the next track  
- 📂 Music file handling using POSIX-compliant APIs  
- ⚡ Custom dynamic array implementation for playlist management  
- 🎧 Audio playback using SDL2  
- 🖥️ Fully terminal-based interface  

---

## 🛠️ Tech Stack

- **Language:** C++  
- **Audio Library:** SDL2  
- **System Programming:** POSIX-compliant APIs  
- **Data Structures:** Custom dynamic array  

---

## 📦 Project Highlights

### 🔹 Custom Dynamic Array
Implemented a dynamic array from scratch to manage the playlist efficiently, providing better control over memory allocation and resizing.

### 🔹 POSIX File Handling
Used POSIX-compliant system calls for reading and managing music files, enabling portability across Unix-like systems and deeper understanding of low-level I/O.

### 🔹 Audio Playback (SDL2)
Integrated SDL2 for handling audio output, ensuring smooth playback and responsive controls.

### 🔹 Playback Controls
Command-driven interface supporting:
- `play`
- `pause`
- `stop`
- `next`

Includes automatic transition to the next track after the current one finishes.

---

## ⚙️ Installation

### Prerequisites

- C++ compiler (`g++` recommended)
- SDL2

### Install SDL2 (Linux)

```bash
sudo apt-get install libsdl2-dev
```
## 🔨 Build & Run

Make sure you have all dependencies installed, then run:

```bash
make && ./build/MusicPlayer
```
## 📁 Usage

1. Place your music files in a directory.
2. Run the program using the command:

```bash
make && ./build/MusicPlayer
```
---

## 🎯 Learning Outcomes

- Low-level system programming using POSIX-compliant APIs  
- Implementing custom data structures (dynamic arrays) in C++  
- Memory management and performance optimization  
- Audio playback integration using SDL2  

---

## 🔮 Future Improvements

- Playlist shuffle  
- Volume control  
- Support for additional audio formats  
- Improved terminal UI  
- Keyboard shortcuts for controls  

<!-- --- -->
<!-- 
## 📜 License

This project is open-source and available under the MIT License. -->