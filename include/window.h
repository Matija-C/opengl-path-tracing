#pragma once
#include "SDL3/SDL.h"
#include <iostream>

class Window{
public:
    int w;
    int h;
    SDL_Window* window;
public:
    Window(const char* title, int width, int height);
};
