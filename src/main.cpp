#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include <filesystem>
#include "shapes.cpp"

using namespace std;

const int eH = 966;
const int eW = 966;
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
    window = SDL_CreateWindow("Deneme", eW, eH,
    SDL_WINDOW_VULKAN );
    
    if(window == nullptr){
        cout << "Error creating wwindow! \n";
        SDL_Quit();
        return -1;
    }
    else {
        cout << "Window created successfully :) \n";
    }

    // 3 Write to surface directly

    // SDL_Surface* surface = SDL_GetWindowSurface(window);
    // SDL_ClearSurface(surface, 0,0,0,0);

    
    // ----------------------------------------------
    // Main Game Loop
    // ----------------------------------------------

    // Create Renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window,nullptr);
    // Create Surface for texture
    filesystem::path p = "src/assets/villiage.bmp";
    string nig = filesystem::absolute(p);
    cout << nig << "\n";

    SDL_Surface* surface = SDL_LoadBMP(nig.c_str());
    // SDL_Surface* surface = SDL_LoadBMP("/home/ekipcalismasi/Documents/GitHub/SDL3-GameProject/src/assets/gameBg.bmp");
    if (surface == nullptr) {
        cout << "Couldn't open BMP \n";
    }
    // Create Texture
    SDL_Texture* gameBg = SDL_CreateTextureFromSurface(renderer,surface);
    
    if (gameBg == nullptr) {
        cout << "Couldn't generate BMP \n";
    }

    SDL_Event event;
    bool prog = true;
    int ax = 0, ay = 0; 
    int runSpeed = 20;

    SDL_FRect dest;
    dest.h = 960;
    dest.w = 960;
    dest.x = (eW-dest.h)/2;
    dest.y = (eH-dest.w)/2;
    while (prog) {
        // Set Renderer
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        // SDL_DestroySurface(surface); // free memory
    
        SDL_RenderTexture(renderer, gameBg, nullptr, &dest);
        // Basic draw functions
        YAS_DrawRect(90 + ax, 90 + ay, renderer, 40, 40, 70, 100, 39, 9);    
        YAS_DrawCircle(220 + ax, 90 + ay, renderer, 20, 255, 244, 233, 1);
        YAS_DrawTri(330 + ax , 90 + ay , 0, renderer, 20, 24, 120, 40, 1);

        // Renderer double buffer
        SDL_RenderPresent(renderer);
        
    
        // Process all pending events
        while (SDL_PollEvent(&event)) { 
            if (event.type == SDL_EVENT_QUIT) {
                prog = false;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN) { // Check for key down event first
                if (event.key.scancode == 41) {      // Key esc
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
    
        SDL_UpdateWindowSurface(window);
        SDL_Delay(32);
    }
    
    // Clean & close
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    // SDL_DestroySurface(surface);
    SDL_DestroyTexture(gameBg);
    SDL_Quit();

    
    return 0;
}
