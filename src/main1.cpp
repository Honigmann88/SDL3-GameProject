#include <SDL3/SDL.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <iostream>
#include <filesystem>
#include <string>
#include "shapes.cpp"

using namespace std;

const int eH = 128*7;
const int eW = 128*7;

// Alternative better approach - preload both textures and just switch between them
void switchTextures(SDL_Texture*& currentTexture, SDL_Texture* tex1, SDL_Texture* tex2, Uint32 currentTime) {
    static Uint32 lastTime = 0;
    static bool showFirst = true;
    
    if (currentTime - lastTime > 500) { // 500ms = 0.5 seconds
        currentTexture = showFirst ? tex1 : tex2;
        showFirst = !showFirst;
        lastTime = currentTime;
    }
}

int main() {
    cout << "Welcome!\n";

    // 1.) Initialisation - FIXED: Use SDL_Init() instead of SDL_INIT_VIDEO
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        cout << SDL_GetError();
        SDL_Quit();
        return -1;
    }

    // 2.) Creating window
    SDL_Window* window = nullptr;
    window = SDL_CreateWindow("Deneme", eW, eH, SDL_WINDOW_VULKAN);
    
    if (window == nullptr) {
        cout << "Error creating window!\n";
        SDL_Quit();
        return -1;
    }

    // 3.) Creating Renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    // 4.) Loading textures
    filesystem::path pVil = "src/assets/villiage.bmp";
    string villiage = filesystem::absolute(pVil);
    cout << villiage << "\n";

    filesystem::path pDene = "src/assets/playG1.bmp";
    string playG1 = filesystem::absolute(pDene);
    cout << playG1 << "\n";

    // Create Surfaces
    SDL_Surface* surface1 = SDL_LoadBMP(villiage.c_str());
    SDL_Surface* surface2 = SDL_LoadBMP(playG1.c_str());
    
    if (surface1 == nullptr || surface2 == nullptr) {
        cout << "Failed to load BMP files!\n";
        return -1;
    }

    // BETTER APPROACH: Pre-create both textures
    SDL_Texture* texture1 = SDL_CreateTextureFromSurface(renderer, surface1);
    SDL_Texture* texture2 = SDL_CreateTextureFromSurface(renderer, surface2);
    SDL_Texture* currentTexture = texture1; // Start with first texture
    
    // Free surfaces since we have textures now
    SDL_DestroySurface(surface1);
    SDL_DestroySurface(surface2);

    SDL_Event event;
    bool prog = true;
    int ax = 0, ay = 0; 
    int runSpeed = 20;

    SDL_FRect destBg;
    destBg.h = eH;
    destBg.w = eW;
    destBg.x = (eW - destBg.w) / 2; // Fixed calculation
    destBg.y = (eH - destBg.h) / 2; // Fixed calculation
    
    Uint32 lastFrameTime = SDL_GetTicks();

    while (prog) {
        Uint32 currentTime = SDL_GetTicks();
        
        // Set Renderer
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);

        // Use the better approach - switch between preloaded textures
        switchTextures(currentTexture, texture1, texture2, currentTime);
        
        // Render the current texture
        SDL_RenderTexture(renderer, currentTexture, nullptr, &destBg);
        
        // Basic draw functions
        YAS_DrawRect(90 + ax, 90 + ay, renderer, 40, 40, 70, 100, 39, 9);    
        YAS_DrawCircle(220 + ax, 90 + ay, renderer, 20, 255, 244, 233, 1);
        YAS_DrawTri(330 + ax, 90 + ay, 0, renderer, 20, 24, 120, 40, 1);
        
        // Renderer double buffer
        SDL_RenderPresent(renderer);
     
        // Process all pending events
        while (SDL_PollEvent(&event)) { 
            if (event.type == SDL_EVENT_QUIT) {
                prog = false;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
                    prog = false;
                }
                else if (event.key.scancode == SDL_SCANCODE_A) {
                    ax -= runSpeed;
                }
                else if (event.key.scancode == SDL_SCANCODE_D) {
                    ax += runSpeed;
                }
                else if (event.key.scancode == SDL_SCANCODE_W) {
                    ay -= runSpeed;
                }
                else if (event.key.scancode == SDL_SCANCODE_S) {
                    ay += runSpeed;
                }
                SDL_Log("Key pressed: %d", event.key.scancode);
            }
        }
    
        // Cap frame rate
        SDL_Delay(32); // ~60 FPS
    }
    
    // Clean & close
    SDL_DestroyTexture(texture1);
    SDL_DestroyTexture(texture2);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
