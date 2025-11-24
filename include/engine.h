#pragma once
#include "SDL3/SDL.h"
#include "window.h"
#include "viewport.h"
#include "camera.h"

class Engine{
private:
    Window window;
    Viewport viewport;
    SDL_GLContext glContext;
    SDL_Event event;
    Camera camera;
public:
    Engine();
    void run();
};
