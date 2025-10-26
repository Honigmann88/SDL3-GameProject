#include <SDL3/SDL.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>
#include "shapes.cpp"

using namespace std;

// Constants
const int SCREEN_HEIGHT = 128 * 8;
const int SCREEN_WIDTH = 128 * 10;
const int CHARACTER_SIZE = 128;
const int EGG_SIZE = 32;
const int BASKET_SIZE = 48 * 2;
const int CHICKEN_SIZE = 40;
const int MAX_EGGS = 3;
const int MAX_CHICKENS = 5;
const int HATCH_TIME = 10000; // 10 seconds to hatch a chicken

// Grid positions (based on your layout)
const int CHICKEN_START_X = 100;
const int CHICKEN_START_Y = 400;
const int CHICKEN_SPACING = 80;
const int DRAGON_X = SCREEN_WIDTH - 200;
const int DRAGON_Y = 400;
const int CHARACTER_START_X = SCREEN_WIDTH / 2;
const int CHARACTER_START_Y = 400;

// Direction enum for character movement
enum class Direction { RIGHT, LEFT, UP, DOWN };

// Egg class
class Egg {
public:
    float x, y;
    bool collected;
    bool fertilized;
    
    Egg(float posX, float posY, bool isFertilized = false) 
        : x(posX), y(posY), collected(false), fertilized(isFertilized) {}
    
    SDL_FRect getRect() const {
        return {x, y, EGG_SIZE, EGG_SIZE};
    }
};

// Chicken class
class Chicken {
public:
    float x, y;
    bool hasEgg;
    Uint32 lastEggTime;
    Uint32 eggLayInterval;
    
    Chicken(float posX, float posY) 
        : x(posX), y(posY), hasEgg(false), lastEggTime(SDL_GetTicks()), 
          eggLayInterval(15000 + (rand() % 15000)) {} // 15-30 seconds
    
    void update(Uint32 currentTime) {
        if (!hasEgg && (currentTime - lastEggTime > eggLayInterval)) {
            hasEgg = true;
            lastEggTime = currentTime;
            eggLayInterval = 15000 + (rand() % 15000);
        }
    }
    
    Egg layEgg() {
        if (hasEgg) {
            hasEgg = false;
            // 30% chance of being fertilized
            bool fertilized = (rand() % 100) < 30;
            return Egg(x, y, fertilized);
        }
        return Egg(0, 0, false); // Return invalid egg
    }
    
    SDL_FRect getRect() const {
        return {x, y, CHICKEN_SIZE, CHICKEN_SIZE};
    }
};

// Character class
class Character {
public:
    float x, y;
    float speed;
    bool isMoving;
    Direction currentDirection;
    int eggsCollected;
    int fertilizedEggs;
    
    Character(float startX, float startY, float moveSpeed) 
        : x(startX), y(startY), speed(moveSpeed), isMoving(false), 
          currentDirection(Direction::RIGHT), eggsCollected(0), fertilizedEggs(0) {}
    
    void move(int dx, int dy, Direction dir) {
        // Calculate new position
        float newX = x + dx * speed;
        float newY = y + dy * speed;
        
        // Keep character within screen bounds
        if (newX >= 0 && newX <= SCREEN_WIDTH - CHARACTER_SIZE) {
            x = newX;
        }
        if (newY >= 0 && newY <= SCREEN_HEIGHT - CHARACTER_SIZE) {
            y = newY;
        }
        
        isMoving = (dx != 0 || dy != 0);
        if (isMoving) {
            currentDirection = dir;
        }
    }
    
    SDL_FRect getRect() const {
        return {x, y, CHARACTER_SIZE, CHARACTER_SIZE};
    }
    
    bool collectEgg(Egg& egg) {
        if (egg.collected || eggsCollected >= MAX_EGGS) return false;
        
        SDL_FRect charRect = getRect();
        SDL_FRect eggRect = egg.getRect();
        
        // Simple AABB collision detection
        if (charRect.x < eggRect.x + eggRect.w &&
            charRect.x + charRect.w > eggRect.x &&
            charRect.y < eggRect.y + eggRect.h &&
            charRect.y + charRect.h > eggRect.y) {
            
            eggsCollected++;
            if (egg.fertilized) {
                fertilizedEggs++;
            }
            egg.collected = true;
            return true;
        }
        return false;
    }
    
    void emptyBasket() {
        eggsCollected = 0;
        fertilizedEggs = 0;
    }
    
    bool hasFertilizedEgg() const {
        return fertilizedEggs > 0;
    }
    
    bool useFertilizedEgg() {
        if (fertilizedEggs > 0) {
            fertilizedEggs--;
            eggsCollected--;
            return true;
        }
        return false;
    }
};

// Dragon class
class Dragon {
public:
    float x, y;
    bool isHungry;
    Uint32 lastFeedTime;
    int currentFrame;
    Uint32 lastFrameTime;
    
    Dragon(float posX, float posY) 
        : x(posX), y(posY), isHungry(true), lastFeedTime(0), currentFrame(0), lastFrameTime(SDL_GetTicks()) {}
    
    SDL_FRect getRect() const {
        return {x, y, CHARACTER_SIZE * 1.5f, CHARACTER_SIZE * 1.5f};
    }
    
    bool feed() {
        if (isHungry) {
            isHungry = false;
            lastFeedTime = SDL_GetTicks();
            return true;
        }
        return false;
    }
    
    void update(Uint32 currentTime) {
        // Dragon gets hungry again after 30 seconds
        if (!isHungry && (currentTime - lastFeedTime > 30000)) {
            isHungry = true;
        }
        
        // Animate dragon (switch frame every 200ms)
        if (currentTime - lastFrameTime > 200) {
            currentFrame = (currentFrame + 1) % 3;
            lastFrameTime = currentTime;
        }
    }
};

// Hatching Egg class
class HatchingEgg {
public:
    float x, y;
    Uint32 startTime;
    bool isHatching;
    
    HatchingEgg(float posX, float posY) 
        : x(posX), y(posY), startTime(SDL_GetTicks()), isHatching(true) {}
    
    bool isReadyToHatch(Uint32 currentTime) {
        return (currentTime - startTime) >= HATCH_TIME;
    }
    
    SDL_FRect getRect() const {
        return {x, y, EGG_SIZE, EGG_SIZE};
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
        }
    }
    
    return frames;
}

vector<SDL_Texture*> loadBasketTextures(SDL_Renderer* renderer) {
    vector<SDL_Texture*> basketTextures;
    
    for (int i = 0; i <= MAX_EGGS; i++) {
        string basketPath = "src/assets/basketNeggs/basket_egg_" + to_string(i) + ".bmp";
        filesystem::path pBasket = basketPath;
        string absolutePath = filesystem::absolute(pBasket);
        
        SDL_Texture* texture = loadTexture(renderer, absolutePath);
        if (texture != nullptr) {
            basketTextures.push_back(texture);
        }
    }
    
    return basketTextures;
}

vector<SDL_Texture*> loadDragonFrames(SDL_Renderer* renderer) {
    vector<SDL_Texture*> dragonFrames;
    
    for (int i = 0; i < 3; i++) {
        string dragonPath = "src/assets/dragon_" + to_string(i) + ".bmp";
        filesystem::path pDragon = dragonPath;
        string absolutePath = filesystem::absolute(pDragon);
        
        SDL_Texture* frame = loadTexture(renderer, absolutePath);
        if (frame != nullptr) {
            dragonFrames.push_back(frame);
        }
    }
    
    return dragonFrames;
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
    
    const vector<SDL_Texture*>& frames = allFrames[static_cast<int>(direction)];
    if (frames.empty()) return;
    
    if (isMoving && currentTime - lastTime > 100) {
        currentFrame = (currentFrame + 1) % frames.size();
        currentTexture = frames[currentFrame];
        lastTime = currentTime;
    }
    else if (!isMoving) {
        currentFrame = 0;
        currentTexture = frames[0];
    }
}

// Input handling
void handleInput(const bool* keyboardState, Character& player, bool& quit, 
                 Dragon& dragon, vector<Chicken>& chickens, vector<HatchingEgg>& hatchingEggs) {
    int moveX = 0, moveY = 0;
    Direction dir = player.currentDirection;
    
    if (keyboardState[SDL_SCANCODE_A]) {
        moveX = -1;
        dir = Direction::LEFT;
    }
    if (keyboardState[SDL_SCANCODE_D]) {
        moveX = 1;
        dir = Direction::RIGHT;
    }
    if (keyboardState[SDL_SCANCODE_W]) {
        moveY = -1;
        dir = Direction::UP;
    }
    if (keyboardState[SDL_SCANCODE_S]) {
        moveY = 1;
        dir = Direction::DOWN;
    }
    if (keyboardState[SDL_SCANCODE_ESCAPE]) {
        quit = false;
    }
    
    player.move(moveX, moveY, dir);
    
    // Feed dragon with Space key
    static bool spacePressed = false;
    if (keyboardState[SDL_SCANCODE_SPACE] && !spacePressed) {
        spacePressed = true;
        SDL_FRect charRect = player.getRect();
        SDL_FRect dragonRect = dragon.getRect();
        
        // Check if player is near dragon
        if (charRect.x < dragonRect.x + dragonRect.w &&
            charRect.x + charRect.w > dragonRect.x &&
            charRect.y < dragonRect.y + dragonRect.h &&
            charRect.y + charRect.h > dragonRect.y) {
            
            if (player.eggsCollected > 0 && dragon.feed()) {
                player.emptyBasket();
                cout << "Dragon fed! Basket emptied.\n";
            }
        }
    }
    else if (!keyboardState[SDL_SCANCODE_SPACE]) {
        spacePressed = false;
    }
    
    // Automatically place fertilized egg in empty chicken spot
    if (player.hasFertilizedEgg() && chickens.size() < MAX_CHICKENS) {
        // Find first empty chicken spot
        int nextChickenIndex = chickens.size();
        float newX = CHICKEN_START_X;
        float newY = CHICKEN_START_Y + (nextChickenIndex * CHICKEN_SPACING);
        
        // Place hatching egg at the empty spot
        hatchingEggs.emplace_back(newX, newY);
        player.useFertilizedEgg();
        cout << "Fertilized egg placed at spot " << nextChickenIndex + 1 << ". Hatching in " << HATCH_TIME/1000 << " seconds.\n";
    }
}

int main() {
    cout << "Egg Collection Game Starting...\n";
    srand(time(nullptr));

    // 1. SDL Initialization
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        cout << "SDL Init Error: " << SDL_GetError() << "\n";
        return -1;
    }

    // 2. Create Window
    SDL_Window* window = SDL_CreateWindow("Egg Collector", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_VULKAN);
    if (window == nullptr) {
        cout << "Window Creation Error\n";
        SDL_Quit();
        return -1;
    }

    // 3. Create Renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    // 4. Load Textures
    SDL_Texture* bgTexture1 = loadTexture(renderer, filesystem::absolute("src/assets/villiage.bmp"));
    SDL_Texture* bgTexture2 = loadTexture(renderer, filesystem::absolute("src/assets/playG1.bmp"));
    SDL_Texture* currentBgTexture = bgTexture1;

    // Load character frames
    vector<vector<SDL_Texture*>> characterFrames(4);
    characterFrames[static_cast<int>(Direction::RIGHT)] = loadCharacterFrames(renderer, "right", 4);
    characterFrames[static_cast<int>(Direction::LEFT)] = loadCharacterFrames(renderer, "left", 4);
    characterFrames[static_cast<int>(Direction::UP)] = loadCharacterFrames(renderer, "up", 4);
    characterFrames[static_cast<int>(Direction::DOWN)] = loadCharacterFrames(renderer, "down", 4);
    
    SDL_Texture* currentCharacterTexture = characterFrames[static_cast<int>(Direction::RIGHT)][0];

    // Load basket textures
    vector<SDL_Texture*> basketTextures = loadBasketTextures(renderer);

    // Load egg and chicken textures
    SDL_Texture* eggTexture = loadTexture(renderer, filesystem::absolute("src/assets/basketNeggs/egg.bmp"));
    SDL_Texture* chickenTexture = loadTexture(renderer, filesystem::absolute("src/assets/chicken.bmp"));
    
    // Load dragon frames for animation
    vector<SDL_Texture*> dragonFrames = loadDragonFrames(renderer);

    // Create placeholders if textures missing
    auto createPlaceholder = [renderer](int width, int height, Uint32 color) {
        SDL_Surface* surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA32);
        Uint32* pixels = (Uint32*)surface->pixels;
        for (int i = 0; i < width * height; i++) {
            pixels[i] = color;
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);
        return texture;
    };

    if (chickenTexture == nullptr) {
        chickenTexture = createPlaceholder(CHICKEN_SIZE, CHICKEN_SIZE, 0xFFFFFF00); // Yellow
    }
    if (dragonFrames.empty()) {
        for (int i = 0; i < 3; i++) {
            Uint32 color = 0xFFFF0000; // Red
            if (i == 1) color = 0xFFAA0000;
            if (i == 2) color = 0xFF880000;
            SDL_Texture* texture = createPlaceholder(CHARACTER_SIZE * 1.5f, CHARACTER_SIZE * 1.5f, color);
            dragonFrames.push_back(texture);
        }
    }

    // 5. Initialize Game Objects
    Character player(CHARACTER_START_X, CHARACTER_START_Y, 5.0f);
    Dragon dragon(DRAGON_X, DRAGON_Y);
    
    // Start with one chicken in the first spot
    vector<Chicken> chickens;
    chickens.emplace_back(CHICKEN_START_X, CHICKEN_START_Y);
    
    vector<Egg> worldEggs;
    vector<HatchingEgg> hatchingEggs;

    // 6. Setup Rectangles
    SDL_FRect backgroundRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_FRect characterRect = {player.x, player.y, CHARACTER_SIZE, CHARACTER_SIZE};
    SDL_FRect basketRect = {0, 0, BASKET_SIZE, BASKET_SIZE};
    SDL_FRect dragonRect = dragon.getRect();

    // 7. Main Game Loop
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
        handleInput(keyboardState, player, running, dragon, chickens, hatchingEggs);
        
        // Update Character Position
        characterRect.x = player.x;
        characterRect.y = player.y;
        basketRect.x = player.x + (CHARACTER_SIZE - BASKET_SIZE) / 2;
        basketRect.y = player.y - BASKET_SIZE + 10;
        
        // Update Chickens
        for (auto& chicken : chickens) {
            chicken.update(currentTime);
            
            // If chicken has egg, lay it
            if (chicken.hasEgg) {
                Egg newEgg = chicken.layEgg();
                if (newEgg.x != 0 && newEgg.y != 0) { // Valid egg
                    worldEggs.push_back(newEgg);
                    cout << "Chicken laid an egg!\n";
                }
            }
        }
        
        // Update Dragon
        dragon.update(currentTime);
        
        // Update Hatching Eggs
        for (auto it = hatchingEggs.begin(); it != hatchingEggs.end(); ) {
            if (it->isReadyToHatch(currentTime)) {
                // Add new chicken at this position
                chickens.emplace_back(it->x, it->y);
                it = hatchingEggs.erase(it);
                cout << "New chicken hatched! Total chickens: " << chickens.size() << "\n";
            } else {
                ++it;
            }
        }
        
        // Check for egg collection from world
        for (auto it = worldEggs.begin(); it != worldEggs.end(); ) {
            if (player.collectEgg(*it)) {
                it = worldEggs.erase(it);
            } else {
                ++it;
            }
        }
        
        // Clean up collected eggs
        worldEggs.erase(
            std::remove_if(worldEggs.begin(), worldEggs.end(),
                [](const Egg& egg) { return egg.collected; }),
            worldEggs.end()
        );
        
        // Rendering
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);
        
        // Render Background
        // animateBackground(currentBgTexture, bgTexture1, bgTexture2, currentTime);
        SDL_RenderTexture(renderer, currentBgTexture, nullptr, &backgroundRect);
        
        // Render Dragon (with animation and hunger indicator)
        if (!dragonFrames.empty()) {
            SDL_Texture* currentDragonTexture = dragonFrames[dragon.currentFrame];
            SDL_RenderTexture(renderer, currentDragonTexture, nullptr, &dragonRect);
        }
        
      
        // Render Chicken Spots (5 fixed positions)
        for (int i = 0; i < MAX_CHICKENS; i++) {
            float spotX = CHICKEN_START_X;
            float spotY = CHICKEN_START_Y + (i * CHICKEN_SPACING);
            
            // Draw empty spot outline
            YAS_DrawRect(spotX - 5, spotY - 5, renderer, CHICKEN_SIZE + 10, CHICKEN_SIZE + 10, 100, 100, 100, 0);
            YAS_DrawRect(spotX - 5, spotY - 5, renderer, CHICKEN_SIZE + 10, CHICKEN_SIZE + 10, 200, 200, 200, 1);
        }
        
        // Render Chickens in their spots
        for (int i = 0; i < chickens.size(); i++) {
            SDL_FRect chickenRect = chickens[i].getRect();
            SDL_RenderTexture(renderer, chickenTexture, nullptr, &chickenRect);
            
            // Show egg indicator if chicken has egg
            if (chickens[i].hasEgg) {
                YAS_DrawCircle(chickenRect.x + CHICKEN_SIZE/2, chickenRect.y - 10, renderer, 5, 255, 255, 255, 1);
            }
        }
        
        // Render Hatching Eggs
        for (const auto& hatchingEgg : hatchingEggs) {
            SDL_FRect eggRect = hatchingEgg.getRect();
            SDL_RenderTexture(renderer, eggTexture, nullptr, &eggRect);
            
            // Show hatching progress
            float progress = min(1.0f, (float)(currentTime - hatchingEgg.startTime) / HATCH_TIME);
            YAS_DrawRect(eggRect.x, eggRect.y - 15, renderer, EGG_SIZE * progress, 5, 0, 255, 0, 1);
        }
        
        // Render World Eggs
        for (const auto& egg : worldEggs) {
            if (!egg.collected) {
                SDL_FRect eggRect = egg.getRect();
                SDL_RenderTexture(renderer, eggTexture, nullptr, &eggRect);
                
                // Show fertilization indicator
                if (egg.fertilized) {
                    YAS_DrawCircle(eggRect.x + EGG_SIZE/2, eggRect.y - 5, renderer, 3, 0, 255, 0, 1);
                }
            }
        }
        
        // Render Basket
        if (player.eggsCollected >= 0 && player.eggsCollected < basketTextures.size()) {
            SDL_RenderTexture(renderer, basketTextures[player.eggsCollected], nullptr, &basketRect);
        }

        // Render Character
        animateCharacter(currentCharacterTexture, characterFrames, currentTime, 
                        player.isMoving, player.currentDirection);
        SDL_RenderTexture(renderer, currentCharacterTexture, nullptr, &characterRect);

        // Render hunger indicator above dragon
        if (dragon.isHungry) {
            // YAS_DrawRect( const int kay, SDL_Renderer *renderer, int a, int b, int R, int G, int B, int A)
                YAS_DrawRect(dragon.x - 500, dragon.y - 350, renderer, 15, 150, 255, 0, 0, 1); // Red hunger bar
        } else {
            YAS_DrawRect(dragon.x - 10, dragon.y - 25, renderer, 60, 8, 0, 255, 0, 1); // Green fed bar
        }
        
        
        // Render UI - Egg Counter (Top Left)
        for (int i = 0; i < player.eggsCollected; i++) {
            YAS_DrawCircle(30 + i * 25, 30, renderer, 10, 255, 255, 0, 1); // Yellow for eggs
        }
        
        // Render UI - Fertilized Egg Counter
        for (int i = 0; i < player.fertilizedEggs; i++) {
            YAS_DrawCircle(30 + i * 25, 60, renderer, 10, 0, 255, 0, 1); // Green for fertilized
        }
        
        // Present Frame
        SDL_RenderPresent(renderer);
        
        // Frame Rate Control
        SDL_Delay(16);
    }

    // 8. Cleanup
    SDL_DestroyTexture(bgTexture1);
    SDL_DestroyTexture(bgTexture2);
    SDL_DestroyTexture(eggTexture);
    SDL_DestroyTexture(chickenTexture);
    
    for (auto texture : dragonFrames) {
        SDL_DestroyTexture(texture);
    }
    
    for (auto& frames : characterFrames) {
        for (auto texture : frames) {
            SDL_DestroyTexture(texture);
        }
    }
    
    for (auto texture : basketTextures) {
        SDL_DestroyTexture(texture);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    cout << "Game Ended. Final stats - Chickens: " << chickens.size() 
         << ", Eggs collected: " << player.eggsCollected << "\n";
    return 0;
}
