# Compiler
CXX = g++

# SDL2 flags
SDL_CFLAGS  = $(shell sdl2-config --cflags)
SDL_LDFLAGS = $(shell sdl2-config --libs)

# FTXUI flags (install once — see README / setup instructions below)
FTXUI_LDFLAGS = -lftxui-component -lftxui-dom -lftxui-screen

# Project flags
CXXFLAGS = -I src -std=c++20 $(SDL_CFLAGS)

# Source files
SRCS = $(wildcard src/*.cpp)

# Output binary
TARGET = build/MusicPlayer

# Default rule
all: $(TARGET)

# Link object files into the final binary
$(TARGET): $(SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(SDL_LDFLAGS) $(FTXUI_LDFLAGS)

# Clean rule
clean:
	rm -f $(TARGET)
