#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <iostream>

using namespace std;

int main(){

    cout << "Just a point \n";

    // 1.) Initialisation

    if (SDL_INIT_VIDEO) {
        cout << "Window succesfully initialisiert! \n";
    }

    else {
        cout << SDL_GetError();
        SDL_Quit();
        return -1;
    }


    // 2.) Creating window

    SDL_Window* window = nullptr;
    window = SDL_CreateWindow("Deneme",1280,720,
    SDL_WINDOW_VULKAN | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    
    if(window == nullptr){
        cout << "Error creating wwindow! \n";
        SDL_Quit();
        return -1;
    }
    else {
        cout << "Window created successfully :) \n";
    }

    // // 3 Write to surface directly
    // // You may not combine this with 3D or the rendering API on this window

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    SDL_ClearSurface(surface, 0,0,0,0);

    // ----------------------------------------------
    // Main Game Loop
    // ----------------------------------------------
    SDL_Event event;
    bool prog = true;
    int ax = 0, ay = 0; // Initialize both variables
    int runSpeed = 5;
    while (prog) {
        // Clear the surface FIRST
        SDL_ClearSurface(surface, 0, 0, 0, 0);
    
        // Then draw the pixel
        
    
        for (int i = 0; i < 21; i++) {
            
            for (int j = 0; j<21; j++) {
            
                SDL_WriteSurfacePixel(surface, 60+ax+j, 60+ay+i, 255, 0, 0, 1);
            }
        }

        // Update the window to show the changes
        SDL_UpdateWindowSurface(window);
    
        // Process all pending events
        while (SDL_PollEvent(&event)) { 
            if (event.type == SDL_EVENT_QUIT) {
                prog = false;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN) { // Check for key down event first
                if (event.key.scancode == 41) { // Escape key (typically)
                    prog = false;
                }
                else if (event.key.scancode == 7) {  // Key a
                    ax += runSpeed;
                }
                else if (event.key.scancode == 4) {  // Key d
                    ax -= runSpeed;
                }
                else if (event.key.scancode == 22) { // Key w
                    ay += runSpeed;
                }
                else if (event.key.scancode == 26) { // Key s
                    ay -= runSpeed;
                }
                SDL_Log("Key pressed: %d ", event.key.scancode);
            }
        }
    
        SDL_Delay(16);
    }
    // SDL_Delay(1000);
    
    // 4. Clean & close
    SDL_DestroyWindow(window);
    SDL_DestroySurface(surface);
    SDL_Quit();

    
    return 0;
}
