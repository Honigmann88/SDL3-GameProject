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

const int eH = 128*7;
const int eW = 128*7;

// Function to switch between two textures (for background)
void switchTextures(SDL_Texture*& currentTexture, SDL_Texture* tex1, SDL_Texture* tex2, Uint32 currentTime) {
    static Uint32 lastTime = 0;
    static bool showFirst = true;
    
    if (currentTime - lastTime > 500) { // 500ms = 0.5 seconds
        currentTexture = showFirst ? tex1 : tex2;
        showFirst = !showFirst;
        lastTime = currentTime;
    }
}

// Function to animate character with multiple frames
void animateCharacter(SDL_Texture*& currentTexture, const vector<SDL_Texture*>& frames, Uint32 currentTime, bool isMoving) {
    static Uint32 lastTime = 0;
    static int currentFrame = 0;
    
    // Only animate if character is moving
    if (isMoving && currentTime - lastTime > 100) { // 100ms = 0.1 seconds per frame
        currentFrame = (currentFrame + 1) % frames.size();
        currentTexture = frames[currentFrame];
        lastTime = currentTime;
    }
    // If not moving, reset to first frame
    else if (!isMoving && currentFrame != 0) {
        currentFrame = 0;
        currentTexture = frames[0];
    }
}

// Character class to manage character state
class Character {
public:
    float x, y;
    float speed;
    bool isMoving;
    
    Character(float startX, float startY, float moveSpeed) 
        : x(startX), y(startY), speed(moveSpeed), isMoving(false) {}
    
    void move(int dx, int dy) {
        x += dx * speed;
        y += dy * speed;
        isMoving = (dx != 0 || dy != 0);
    }
};

int main() {
    cout << "Welcome!\n";

    // 1.) Initialisation
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        cout << SDL_GetError();
        SDL_Quit();
        return -1;
    }

    // 2.) Creating window
    SDL_Window* window = SDL_CreateWindow("Deneme", eW, eH, SDL_WINDOW_VULKAN);
    
    if (window == nullptr) {
        cout << "Error creating window!\n";
        SDL_Quit();
        return -1;
    }

    // 3.) Creating Renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    // 4.) Loading background textures
    filesystem::path pVil = "src/assets/villiage.bmp";
    string villiage = filesystem::absolute(pVil);
    cout << villiage << "\n";

    filesystem::path pDene = "src/assets/playG1.bmp";
    string playG1 = filesystem::absolute(pDene);
    cout << playG1 << "\n";

    // Create Surfaces for background
    SDL_Surface* surface1 = SDL_LoadBMP(villiage.c_str());
    SDL_Surface* surface2 = SDL_LoadBMP(playG1.c_str());
    
    if (surface1 == nullptr || surface2 == nullptr) {
        cout << "Failed to load background BMP files!\n";
        return -1;
    }

    // Create background textures
    SDL_Texture* bgTexture1 = SDL_CreateTextureFromSurface(renderer, surface1);
    SDL_Texture* bgTexture2 = SDL_CreateTextureFromSurface(renderer, surface2);
    SDL_Texture* currentBgTexture = bgTexture1;
    
    // Free background surfaces
    SDL_DestroySurface(surface1);
    SDL_DestroySurface(surface2);

    // 5.) Loading character walking frames
    vector<SDL_Texture*> characterFrames;
    
    // Load 4 character walking frames
    for (int i = 1; i <= 4; i++) {
        string framePath = "src/assets/charFrames/pixil-frame-" + to_string(i) + ".bmp";
        filesystem::path pFrame = framePath;
        string absoluteFramePath = filesystem::absolute(pFrame);
        
        SDL_Surface* frameSurface = SDL_LoadBMP(absoluteFramePath.c_str());
        if (frameSurface == nullptr) {
            cout << "Failed to load character frame: " << framePath << "\n";
            // Continue with available frames
            continue;
        }
        
        SDL_Texture* frameTexture = SDL_CreateTextureFromSurface(renderer, frameSurface);
        characterFrames.push_back(frameTexture);
        SDL_DestroySurface(frameSurface);
        
        cout << "Loaded character frame: " << absoluteFramePath << "\n";
    }
    
    // If no character frames loaded, create a placeholder
    if (characterFrames.empty()) {
        cout << "No character frames found, creating placeholder...\n";
        // Create a simple colored surface as placeholder - SDL3 FIX
        SDL_Surface* placeholderSurface = SDL_CreateSurface(64, 64, SDL_PIXELFORMAT_RGBA32);
        // SDL3 uses different color mapping - create a red pixel array
        Uint32* pixels = (Uint32*)placeholderSurface->pixels;
        for (int i = 0; i < 64 * 64; i++) {
            pixels[i] = 0xFFFF0000; // Red color in RGBA
        }
        SDL_Texture* placeholderTexture = SDL_CreateTextureFromSurface(renderer, placeholderSurface);
        characterFrames.push_back(placeholderTexture);
        SDL_DestroySurface(placeholderSurface);
    }
    
    SDL_Texture* currentCharacterTexture = characterFrames[0];

    // Create character
    Character player(100, 100, 5.0f);

    SDL_Event event;
    bool prog = true;

    SDL_FRect destBg;
    destBg.h = eH;
    destBg.w = eW;
    destBg.x = (eW - destBg.w) / 2;
    destBg.y = (eH - destBg.h) / 2;
    
    // Character destination rectangle
    SDL_FRect characterDest;
    characterDest.w = 100;  // Character width
    characterDest.h = 100;  // Character height
    
    Uint32 lastFrameTime = SDL_GetTicks();

    while (prog) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastFrameTime) / 1000.0f; // Delta time in seconds
        lastFrameTime = currentTime;

        // Process all pending events
        while (SDL_PollEvent(&event)) { 
            if (event.type == SDL_EVENT_QUIT) {
                prog = false;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
                // Movement will be handled by keyboard state checking below
            }
        }
        
        // Continuous keyboard state checking for smooth movement - SDL3 FIX
        const bool* keyboardState = SDL_GetKeyboardState(NULL);
        int moveX = 0, moveY = 0;
        
        if (keyboardState[SDL_SCANCODE_A]) {  // Key A - left
            moveX -= 1;
        }
        if (keyboardState[SDL_SCANCODE_D]) {  // Key D - right
            moveX += 1;
        }
        if (keyboardState[SDL_SCANCODE_W]) {  // Key W - up
            moveY -= 1;
        }
        if (keyboardState[SDL_SCANCODE_S]) {  // Key S - down
            moveY += 1;
        }
        if (keyboardState[SDL_SCANCODE_ESCAPE]) {  // Key Esc
            prog = false;
        }
        
        // Update character position
        player.move(moveX, moveY);
        
        // Update character destination
        characterDest.x = player.x;
        characterDest.y = player.y;

        // Set Renderer
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);

        // Update background animation
        // switchTextures(currentBgTexture, bgTexture1, bgTexture2, currentTime);
        SDL_RenderTexture(renderer, currentBgTexture, nullptr, &destBg);
        
        // Update character animation
        animateCharacter(currentCharacterTexture, characterFrames, currentTime, player.isMoving);
        SDL_RenderTexture(renderer, currentCharacterTexture, nullptr, &characterDest);
        
        // Draw other game objects (optional)
        // YAS_DrawCircle(220, 90, renderer, 20, 255, 244, 233, 1);
        // YAS_DrawTri(330, 90, 0, renderer, 20, 24, 120, 40, 1);
        
        // Renderer double buffer
        SDL_RenderPresent(renderer);
    
        // Cap frame rate
        SDL_Delay(32); // ~30 FPS
    }
    
    // Clean & close
    SDL_DestroyTexture(bgTexture1);
    SDL_DestroyTexture(bgTexture2);
    for (auto frame : characterFrames) {
        SDL_DestroyTexture(frame);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
