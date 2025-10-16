#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
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

    // if (SDL_WriteSurfacePixel(surface, 60 , 60 , 60 , 70 , 28 , 1)) {
    //     cout << "Pixel written succesfully! \n";
    
    // }

    // else {
    //     cout << "ERROR: Pixel \n";
    // }

    SDL_UpdateWindowSurface(window);
   
   
    SDL_Event event;
    bool prog = 1;
    while (prog) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                prog = false;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                SDL_Log("Key pressed: %d ", event.key.scancode);
            }
        }
    }

    // SDL_Delay(1000);
    
    // 4. Clean & close
    SDL_DestroyWindow(window);
    SDL_Quit();

    
    return 0;
}
