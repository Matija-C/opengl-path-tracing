#include <iostream>
#include "glad/glad.h"
#include "SDL3/SDL.h"
#include "engine.h"
#include "window.h"
#include "camera.h"

Engine::Engine()
    : window("Path tracer", 1600, 900), viewport(1600, 900)
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
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

    SDL_SetWindowRelativeMouseMode(window.window, true);
    viewport.initialize();
}

void Engine::run(){
    bool running = true;
    bool justResized = true;  
    float cameraSpeed = 0.003f;
    int deltaTime = 0;
    int lastFrame = 0;
    int sample = 1;

    while(running){
        int currentFrame = SDL_GetTicks();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        sample++;
        
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_EVENT_QUIT){
                running = false;
            }
            else if(event.type == SDL_EVENT_WINDOW_RESIZED){
                SDL_GetWindowSize(window.window, &window.w, &window.h);
                glViewport(0, 0, window.w, window.h);
                viewport.updateResolution(window.w, window.h);
                justResized = true;
            }
        }

        const bool* keyboardState = SDL_GetKeyboardState(NULL);

        if(keyboardState[SDL_SCANCODE_W]){
            camera.moveForward(cameraSpeed * deltaTime);
            viewport.resetAccumulation();
            sample = 1;
        }
        if(keyboardState[SDL_SCANCODE_S]){
            camera.moveBackward(cameraSpeed * deltaTime);
            viewport.resetAccumulation();
            sample = 1;
        }
        if(keyboardState[SDL_SCANCODE_A]){
            camera.moveLeft(cameraSpeed * deltaTime);
            viewport.resetAccumulation();
            sample = 1;
        }
        if(keyboardState[SDL_SCANCODE_D]){
            camera.moveRight(cameraSpeed * deltaTime);
            viewport.resetAccumulation();
            sample = 1;
        }
        if(keyboardState[SDL_SCANCODE_SPACE]){
            camera.moveUpward(cameraSpeed * deltaTime);
            viewport.resetAccumulation();
            sample = 1;
        }
        if(keyboardState[SDL_SCANCODE_LSHIFT]){
            camera.moveDownward(cameraSpeed * deltaTime);
            viewport.resetAccumulation();
            sample = 1;
        }
        
        float mouseX, mouseY;
        SDL_GetRelativeMouseState(&mouseX, &mouseY);
        camera.updateOrientation(mouseX, mouseY);
        camera.updateDirections();

        viewport.draw();

        if(justResized){
            viewport.shader->setInt("viewportWidth", window.w);
            viewport.shader->setInt("viewportHeight", window.h);
            viewport.updateResolution(window.w, window.h);
            sample = 1;
            justResized = false;
        }

        if(mouseX || mouseY){
            viewport.resetAccumulation();
            sample = 1;
        }

        viewport.shader->setVec3("cameraPosition", camera.position);
        viewport.shader->setFloat("yaw", camera.yaw);
        viewport.shader->setFloat("pitch", camera.pitch);
        viewport.shader->setInt("numSamples", sample);
        
        SDL_GL_SwapWindow(window.window);
    }

    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(window.window);
    SDL_Quit();
}
