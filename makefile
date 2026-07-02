# Compiler
CXX = g++

# libVLC flags
VLC_CFLAGS  = $(shell pkg-config --cflags libvlc)
VLC_LDFLAGS = $(shell pkg-config --libs libvlc)

# FTXUI flags
FTXUI_LDFLAGS = -lftxui-component -lftxui-dom -lftxui-screen

# Project flags
CXXFLAGS = -I src -std=c++20 $(VLC_CFLAGS)

# Source files
SRCS = $(wildcard src/*.cpp)

# Output binary
TARGET = build/MusicPlayer

# Default rule
all: $(TARGET)

# Link object files into the final binary
$(TARGET): $(SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(VLC_LDFLAGS) $(FTXUI_LDFLAGS)

# Clean rule
clean:
	rm -f $(TARGET)
