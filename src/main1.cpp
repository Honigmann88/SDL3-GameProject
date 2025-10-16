// This commands are sdl3 headers
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
// This command is C++ header
#include <iostream>

using namespace std;

int main() {
    // Initialize the SDL video subsystem
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return 1;
    }

    // Create a window
    SDL_Window* window = SDL_CreateWindow("My SDL3 Game", 640, 480, 0);
    if (window == nullptr) {
        SDL_Log("Couldn't create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // (Optional) Create a renderer for graphics
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr) {
        SDL_Log("Couldn't create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    //--------------------------------------------------------
    if (!TTF_Init()) {
        SDL_Log("Couldn't initialize SDL_ttf: %s", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    TTF_Font* font = TTF_OpenFont("deneme.ttf", 25);

    if (font == nullptr) {
        SDL_Log("Couldn't load font: %s", SDL_GetError());
        TTF_Quit();
        // ... (clean up other resources)
        SDL_Quit();
        return 1;
    }

    
    // Path and font size
    //--------------------------------------------------------

    // Main loop flag and event handler
    bool done = false;
    SDL_Event event;

    // Main game loop
    while (!done) {
        // Event handling: check for the quit event
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }
        }

        // Set draw color to dark gray and clear the screen
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        // --- Your drawing code (like text) will go here later ---

        // Define a color for the text (Red in this case)
        SDL_Color textColor = {255, 0, 0, 255}; // RGBA values
        
        // Create a surface containing the text
        SDL_Surface* textSurface =
        TTF_RenderText_Blended(font, "Hello, Nigger!", textColor);

        if (textSurface != nullptr) {
            // Convert the surface into a texture
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_DestroySurface(textSurface); // Surface is no longer needed

            if (textTexture != nullptr) {
                // Define where to draw the texture (x, y, width, height)
                SDL_FRect textRect = {50.0f, 50.0f, 200.0f, 50.0f};
                SDL_RenderTexture(renderer, textTexture, nullptr, &textRect);
                SDL_DestroyTexture(textTexture);
            }
        }
            
        // Update the screen with the rendered content
        SDL_RenderPresent(renderer);
    }

    // Clean up resources
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
  
