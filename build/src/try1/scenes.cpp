#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <iostream>
#include <filesystem>
#include "shapes.cpp"

using namespace std;


class Scenes {

  public:
      Scenes(int const sNumber, string const sName, int windowW, int windowH){
      sceneNumber = sNumber;
      sceneName = sName;
      eW = windowW;
      eH = windowH;
    }

    void setup(SDL_Window* window){
      winOfTheScene = window;
      rendererOfTheScene = SDL_CreateRenderer(window, nullptr);
      if (rendererOfTheScene == nullptr) {
        cout << "ERROR: The Window Initialisation has been unsuccesfull. \n";
      }

    }

    void loadTextureToScene(filesystem::path p, SDL_FRect dest){
      string pathOfTheTexture = filesystem::absolute(p);
      cout << "INFO: Looking for asset in: " << pathOfTheTexture << endl;
      surfaceForBg = SDL_LoadBMP(pathOfTheTexture.c_str());
      if (surfaceForBg == nullptr) {
        cout << "ERROR: BMP couldn't loaded to the Surface" << endl;
      }

      textureOfTheSceneBg = SDL_CreateTextureFromSurface(rendererOfTheScene, surfaceForBg);
    }
    void sceneStart(){
      bool running = 1;
      while (running) {
        SDL_SetRenderDrawColor(rendererOfTheScene, 0, 0, 0, 0);
        SDL_RenderClear(rendererOfTheScene);
        
        SDL_RenderPresent(rendererOfTheScene);


        YAS_DrawRect(100, 100, rendererOfTheScene, 30, 30, 100, 255, 120, 1);


        SDL_UpdateWindowSurface(winOfTheScene);
        SDL_Delay(sceneFrameRateInMSeconds);
      }      
    }


  private:
    int sceneNumber;
    string sceneName;
    int sceneFrameRateInMSeconds = 16; // 16ms => 60fps
    SDL_Texture* textureOfTheSceneBg;
    SDL_Renderer* rendererOfTheScene;
    SDL_Surface* surfaceForBg;
    SDL_FRect destOfBg;
    SDL_Window* winOfTheScene;
    int eW;
    int eH;

};
