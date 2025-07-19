#pragma once
#include "SDL3/SDL.h"
#include "window.h"
#include "viewport.h"

class Engine{
private:
    Window window;
    SDL_GLContext glContext;
    SDL_Event event;
    Viewport viewport;
public:
    Engine();
    void run();
};
