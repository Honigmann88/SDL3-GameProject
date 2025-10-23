#include <SDL3/SDL.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include "shapes.cpp"

using namespace std;

// Constants
const int SCREEN_HEIGHT = 128 * 7;
const int SCREEN_WIDTH = 128 * 7;
const int CHARACTER_SIZE = 64 * 1.5;

int main() {
    cout << "Game Starting...\n";

    // 1. SDL Initialization
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        cout << "SDL Init Error: " << SDL_GetError() << "\n";
        return -1;
    }

    // 2. Create Window
    SDL_Window* window = SDL_CreateWindow("Character Game", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_VULKAN);
    if (window == nullptr) {
        cout << "Window Creation Error\n";
        SDL_Quit();
        return -1;
    }

    // 3. Create Renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    // 4. Load Background Textures
    SDL_Texture* bgTexture1 = loadTexture(renderer, filesystem::absolute("src/assets/villiage.bmp"));
    SDL_Texture* bgTexture2 = loadTexture(renderer, filesystem::absolute("src/assets/playG1.bmp"));
    SDL_Texture* currentBgTexture = bgTexture1;

    // 5. Load Character Frames for All Directions
    vector<vector<SDL_Texture*>> characterFrames(4); // 4 directions
    
    characterFrames[static_cast<int>(Direction::RIGHT)] = loadCharacterFrames(renderer, "right", 4);
    characterFrames[static_cast<int>(Direction::LEFT)] = loadCharacterFrames(renderer, "left", 4);
    characterFrames[static_cast<int>(Direction::UP)] = loadCharacterFrames(renderer, "up", 4);
    characterFrames[static_cast<int>(Direction::DOWN)] = loadCharacterFrames(renderer, "down", 4);
    
    // Create placeholder if no frames loaded
    for (auto& frames : characterFrames) {
        if (frames.empty()) {
            // Create a simple colored rectangle as placeholder
            SDL_Surface* placeholder = SDL_CreateSurface(CHARACTER_SIZE, CHARACTER_SIZE, SDL_PIXELFORMAT_RGBA32);
            Uint32* pixels = (Uint32*)placeholder->pixels;
            for (int i = 0; i < CHARACTER_SIZE * CHARACTER_SIZE; i++) {
                pixels[i] = 0xFFFF0000; // Red
            }
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, placeholder);
            frames.push_back(texture);
            SDL_DestroySurface(placeholder);
            cout << "Created placeholder texture\n";
        }
    }
    
    SDL_Texture* currentCharacterTexture = characterFrames[static_cast<int>(Direction::RIGHT)][0];

    // 6. Initialize Character
    Character player(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 5.0f);

    // 7. Setup Rectangles
    SDL_FRect backgroundRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_FRect characterRect = {player.x, player.y, CHARACTER_SIZE, CHARACTER_SIZE};

    // 8. Main Game Loop
    bool running = true;
    Uint32 lastFrameTime = SDL_GetTicks();
    SDL_Event event;

    while (running) {
        Uint32 currentTime = SDL_GetTicks();
        
        // Handle Events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }
        
        // Handle Input
        const bool* keyboardState = SDL_GetKeyboardState(NULL);
        handleInput(keyboardState, player, running);
        
        // Update Character Position
        characterRect.x = player.x;
        characterRect.y = player.y;
        
        // Rendering
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);
        
        // Animate and Render Background
        // animateBackground(currentBgTexture, bgTexture1, bgTexture2, currentTime);
        SDL_RenderTexture(renderer, currentBgTexture, nullptr, &backgroundRect);
        
        // Animate and Render Character
        animateCharacter(currentCharacterTexture, characterFrames, currentTime, 
                        player.isMoving, player.currentDirection);
        SDL_RenderTexture(renderer, currentCharacterTexture, nullptr, &characterRect);
        
        // Render Other Game Objects
        // YAS_DrawCircle(200, 100, renderer, 30, 255, 200, 100, 1);
        
        // Present Frame
        SDL_RenderPresent(renderer);
        
        // Frame Rate Control
        SDL_Delay(32);
    }

    // 9. Cleanup
    SDL_DestroyTexture(bgTexture1);
    SDL_DestroyTexture(bgTexture2);
    
    for (auto& frames : characterFrames) {
        for (auto texture : frames) {
            SDL_DestroyTexture(texture);
        }
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    cout << "Game Ended\n";
    return 0;
}
