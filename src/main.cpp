#include <SDL3/SDL.h>
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

    // 3 Write to surface directly
    // You may not combine this with 3D or the rendering API on this window

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    SDL_ClearSurface(surface, 0,0,0,0);

    if (SDL_WriteSurfacePixel(surface, 10 , 10 , 60 , 70 , 28 , 1)) {
        cout << "Pixel written succesfully! \n";
        
    }

    else {
        cout << "ERROR: Pixel \n";
    }
    

    SDL_UpdateWindowSurface(window);

    // 10 Sekunden warten
    SDL_Delay(10000);
    
    // 4. Clean & close
    SDL_DestroyWindow(window);
    SDL_Quit();

    
    return 0;
}
