#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <iostream>
// #include <filesystem>
// #include "shapes.cpp"
#include "scenes.cpp"

using namespace std;

const int eH = 966;
const int eW = 966;
int main(){

    SDL_Window* theGame = SDL_CreateWindow("Deneme", eW, eH, SDL_WINDOW_VULKAN);
    cout << "Just a point \n";

    SDL_UpdateWindowSurface(theGame);
    Scenes first = Scenes(1, "Home Screen", eW, eH);
    first.setup(theGame);
    first.sceneStart();

    SDL_DestroyWindow(theGame);
    SDL_Quit;
    
    return 0;
}
