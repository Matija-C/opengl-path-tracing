#include <iostream>
#include "SDL3/SDL.h"
#include "window.h"

Window::Window(const char* title, int width, int height)
    : w(width), h(height) 
    {
    window = SDL_CreateWindow(
        title,
        width, height,
        SDL_WINDOW_OPENGL
    );
    if(!window){
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
    }
}
