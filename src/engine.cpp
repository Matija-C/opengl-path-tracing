#include <iostream>
#include <glad/glad.h>
#include "SDL3/SDL.h"
#include "engine.h"
#include "window.h"

Engine::Engine()
    : window("Path tracer", 1280, 720)
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    glContext = SDL_GL_CreateContext(window.window);
    if(!glContext){
        std::cerr << "SDL_GL_CreateContext failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window.window);
        SDL_Quit();
    }
    
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize glad!" << std::endl;
        SDL_GL_DestroyContext(glContext);
        SDL_DestroyWindow(window.window);
        SDL_Quit();
    }

    viewport.initialize();
}

void Engine::run(){
    bool running = true;
    
    while(running){
        
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_EVENT_QUIT){
                running = false;
            }
            if(event.type == SDL_EVENT_WINDOW_RESIZED){
                SDL_GetWindowSize(window.window, &window.w, &window.h);
                glViewport(0, 0, window.w, window.h);
            }
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
       
        viewport.shader->setInt("windowWidth", window.w);
        viewport.shader->setInt("windowHeight", window.h);
        viewport.draw();
        
        SDL_GL_SwapWindow(window.window);
    }

    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(window.window);
    SDL_Quit();
}
