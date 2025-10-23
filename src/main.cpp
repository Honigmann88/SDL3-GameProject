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

// Direction enum for character movement
enum class Direction { RIGHT, LEFT, UP, DOWN };

// Character class
class Character {
public:
    float x, y;
    float speed;
    bool isMoving;
    Direction currentDirection;
    
    Character(float startX, float startY, float moveSpeed) 
        : x(startX), y(startY), speed(moveSpeed), isMoving(false), currentDirection(Direction::RIGHT) {}
    
    void move(int dx, int dy, Direction dir) {
        x += dx * speed;
        y += dy * speed;
        isMoving = (dx != 0 || dy != 0);
        if (isMoving) {
            currentDirection = dir;
        }
    }
};

// Texture management functions
SDL_Texture* loadTexture(SDL_Renderer* renderer, const string& path) {
    SDL_Surface* surface = SDL_LoadBMP(path.c_str());
    if (surface == nullptr) {
        cout << "Failed to load: " << path << "\n";
        return nullptr;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    
    return texture;
}

vector<SDL_Texture*> loadCharacterFrames(SDL_Renderer* renderer, const string& direction, int frameCount) {
    vector<SDL_Texture*> frames;
    
    for (int i = 1; i <= frameCount; i++) {
        string framePath = "src/assets/charFrames/char_" + direction + "_" + to_string(i) + ".bmp";
        filesystem::path pFrame = framePath;
        string absolutePath = filesystem::absolute(pFrame);
        
        SDL_Texture* frame = loadTexture(renderer, absolutePath);
        if (frame != nullptr) {
            frames.push_back(frame);
            cout << "Loaded: " << absolutePath << "\n";
        }
    }
    
    return frames;
}

// Animation functions
void animateBackground(SDL_Texture*& currentTexture, SDL_Texture* tex1, SDL_Texture* tex2, Uint32 currentTime) {
    static Uint32 lastTime = 0;
    static bool showFirst = true;
    
    if (currentTime - lastTime > 500) {
        currentTexture = showFirst ? tex1 : tex2;
        showFirst = !showFirst;
        lastTime = currentTime;
    }
}

void animateCharacter(SDL_Texture*& currentTexture, 
                     const vector<vector<SDL_Texture*>>& allFrames, 
                     Uint32 currentTime, 
                     bool isMoving, 
                     Direction direction) {
    static Uint32 lastTime = 0;
    static int currentFrame = 0;
    
    // Get frames for current direction
    const vector<SDL_Texture*>& frames = allFrames[static_cast<int>(direction)];
    
    if (frames.empty()) return;
    
    // Only animate if character is moving
    if (isMoving && currentTime - lastTime > 100) {
        currentFrame = (currentFrame + 1) % frames.size();
        currentTexture = frames[currentFrame];
        lastTime = currentTime;
    }
    // If not moving, reset to first frame of current direction
    else if (!isMoving) {
        currentFrame = 0;
        currentTexture = frames[0];
    }
}

// Input handling
void handleInput(const bool* keyboardState, Character& player, bool& quit) {
    int moveX = 0, moveY = 0;
    Direction dir = player.currentDirection; // Default to current direction
    
    if (keyboardState[SDL_SCANCODE_A]) {  // Left
        moveX = -1;
        dir = Direction::LEFT;
    }
    if (keyboardState[SDL_SCANCODE_D]) {  // Right
        moveX = 1;
        dir = Direction::RIGHT;
    }
    if (keyboardState[SDL_SCANCODE_W]) {  // Up
        moveY = -1;
        dir = Direction::UP;
    }
    if (keyboardState[SDL_SCANCODE_S]) {  // Down
        moveY = 1;
        dir = Direction::DOWN;
    }
    if (keyboardState[SDL_SCANCODE_ESCAPE]) {
        quit = false;
    }
    
    player.move(moveX, moveY, dir);
}

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
