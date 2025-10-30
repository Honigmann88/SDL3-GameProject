#include <SDL3/SDL.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <ctime>
#include <algorithm>
#include "shapes.cpp"

using namespace std;

// Constants
const int SCREEN_HEIGHT = 128 * 6;
const int SCREEN_WIDTH = 1280;
const int CHARACTER_SIZE = 100;
const int EGG_SIZE = 22;
const int BASKET_SIZE = 64;
const int CHICKEN_SIZE = 90;
const int MAX_EGGS = 3;
const int MAX_CHICKENS = 5;
int HATCH_TIME = 20000; // 10 seconds to hatch a chicken
const int HATCH_MIN_TIME = 1000; // 10 seconds to hatch a chicken
const int INITIAL_HUNGER_TIME = 45000; // 45 seconds at start
const int MIN_HUNGER_TIME = 15000;     // 15 seconds minimum
const int HUNGER_DECREMENT = 3000;     // Reduce by 2 seconds each feed
int CHICKEN_DEATH_INTERVAL = 60000 ; // Check for chicken death every 60 seconds
const int CHICKEN_DEATH_MIN_INTERVAL = 10000 ; // Check for chicken death every 60 seconds
const int EGG_REQUIREMENT_INCREMENT = 3; // Dragon requires 1 more egg every 5 days
int intervalDeathChicken = 1;
int intervalHatchChicken = 1;

// Grid positions (based on your layout)
const int CHICKEN_START_X = 200;
const int CHICKEN_START_Y = 720;
const int CHICKEN_SPACING = 30;
const int DRAGON_X = SCREEN_WIDTH - 250;
const int DRAGON_Y = 400;
const int CHARACTER_START_X = SCREEN_WIDTH / 2;
const int CHARACTER_START_Y = 400;

// Chicken states
enum class ChickenState { WALKING, STOPPED, FEEDING };
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
    
    // New variables for movement and animation
    ChickenState state;
    Direction direction;
    float speed;
    Uint32 stateStartTime;
    Uint32 stateDuration;
    int currentFrame;
    Uint32 lastFrameTime;
    
    Chicken(float posX, float posY) 
        : x(posX), y(posY), hasEgg(false), lastEggTime(SDL_GetTicks()), 
          eggLayInterval(15000 + (rand() % 15000)), 
          state(ChickenState::WALKING), direction(rand() % 2 == 0 ? Direction::RIGHT : Direction::LEFT),
          speed(1.0f), stateStartTime(SDL_GetTicks()), stateDuration(2000 + (rand() % 3000)),
          currentFrame(0), lastFrameTime(SDL_GetTicks()) {}
    
    void update(Uint32 currentTime) {
        // Update egg laying
        if (!hasEgg && (currentTime - lastEggTime > eggLayInterval)) {
            hasEgg = true;
            lastEggTime = currentTime;
            eggLayInterval = 15000 + (rand() % 15000);
        }
        
        // Update state and movement
        if (currentTime - stateStartTime > stateDuration) {
            changeState(currentTime);
        }
        
        // Handle current state
        switch(state) {
            case ChickenState::WALKING:
                updateWalking(currentTime);
                break;
            case ChickenState::STOPPED:
                // Just wait
                break;
            case ChickenState::FEEDING:
                updateFeeding(currentTime);
                break;
        }
        
        // Keep chickens on left side of screen
        if (x < 50) {
            x = 50;
            direction = Direction::RIGHT;
        } else if (x > SCREEN_WIDTH / 2 - CHICKEN_SIZE) {
            x = SCREEN_WIDTH / 2 - CHICKEN_SIZE;
            direction = Direction::LEFT;
        }
        
        // Keep chickens within vertical bounds
        if (y < 100) {
            y = 100;
        } else if (y > SCREEN_HEIGHT - CHICKEN_SIZE - 50) {
            y = SCREEN_HEIGHT - CHICKEN_SIZE - 50;
        }
    }
    
    void changeState(Uint32 currentTime) {
        stateStartTime = currentTime;
        
        switch(state) {
            case ChickenState::WALKING:
                // After walking, either stop or feed
                state = (rand() % 100 < 70) ? ChickenState::STOPPED : ChickenState::FEEDING;
                stateDuration = 1000 + (rand() % 2000);
                break;
                
            case ChickenState::STOPPED:
                // After stopping, either walk or feed
                if (rand() % 100 < 50) {
                    state = ChickenState::WALKING;
                    direction = (rand() % 2 == 0) ? Direction::RIGHT : Direction::LEFT;
                    stateDuration = 2000 + (rand() % 3000);
                } else {
                    state = ChickenState::FEEDING;
                    stateDuration = 2000 + (rand() % 2000);
                }
                break;
                
            case ChickenState::FEEDING:
                // After feeding, start walking
                state = ChickenState::WALKING;
                direction = (rand() % 2 == 0) ? Direction::RIGHT : Direction::LEFT;
                stateDuration = 2000 + (rand() % 3000);
                break;
        }
        
        currentFrame = 0; // Reset animation frame when state changes
    }
    
    void updateWalking(Uint32 currentTime) {
        // Move chicken based on direction
        if (direction == Direction::RIGHT) {
            x += speed;
        } else {
            x -= speed;
        }
        
        // Occasionally change direction randomly
        if (rand() % 200 == 0) {
            direction = (direction == Direction::RIGHT) ? Direction::LEFT : Direction::RIGHT;
        }
        
        // Animate walking (change frame every 200ms)
        if (currentTime - lastFrameTime > 200) {
            currentFrame = (currentFrame + 1) % 3;
            lastFrameTime = currentTime;
        }
    }
    
    void updateFeeding(Uint32 currentTime) {
        // Animate feeding (change frame every 300ms)
        if (currentTime - lastFrameTime > 300) {
            currentFrame = (currentFrame + 1) % 4;
            lastFrameTime = currentTime;
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
    int& hungerTime; // Reference to the current hunger time
    int eggsRequired; // How many eggs dragon needs to be fed
    float angerLevel; // 0-100, when reaches 100, dragon eats a chicken
    Uint32 lastAngerIncrease;
    
    Dragon(float posX, float posY, int& hungerTimeRef) 
        : x(posX), y(posY), isHungry(true), lastFeedTime(0), currentFrame(0), 
          lastFrameTime(SDL_GetTicks()), hungerTime(hungerTimeRef), eggsRequired(1),
          angerLevel(0.0f), lastAngerIncrease(SDL_GetTicks()) {}
    
    SDL_FRect getRect() const {
        return {x, y, CHARACTER_SIZE * 2.5f, CHARACTER_SIZE * 2.5f};
    }
    
    bool feed(int eggsGiven) {
        if (isHungry && eggsGiven >= eggsRequired) {
            isHungry = false;
            lastFeedTime = SDL_GetTicks();
            angerLevel = 0; // Reset anger when fed
            return true;
        }
        return false;
    }
    
    void update(Uint32 currentTime, vector<Chicken>& chickens, bool& gameOver) {
        // Use dynamic hunger time
        if (!isHungry && (currentTime - lastFeedTime > hungerTime)) {
            isHungry = true;
        }
        
        // Increase anger when hungry
        if (isHungry && currentTime - lastAngerIncrease > 1000) { // Increase anger every second when hungry
            angerLevel = min(100.0f, angerLevel + 1.0f);
            lastAngerIncrease = currentTime;
            
            // If anger reaches 100, eat a chicken
            if (angerLevel >= 100.0f && !chickens.empty()) {
                // Eat a random chicken
                int chickenToEat = rand() % chickens.size();
                chickens.erase(chickens.begin() + chickenToEat);
                angerLevel = 0; // Reset anger after eating
                cout << "Dragon ate a chicken out of anger! Remaining chickens: " << chickens.size() << "\n";
                
                // Check for game over
                if (chickens.empty()) {
                    gameOver = true;
                    cout << "GAME OVER! Dragon ate all chickens!\n";
                }
            }
        }
        
        // Animate dragon (switch frame every 200ms)
        if (currentTime - lastFrameTime > 200) {
            currentFrame = (currentFrame + 1) % 3; // Still using 3 frames for dragon
            lastFrameTime = currentTime;
        }
    }
    
    void increaseEggRequirement() {
        eggsRequired++;
        cout << "Dragon now requires " << eggsRequired << " eggs to be fed!\n";
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

vector<SDL_Texture*> loadDragonFrames(SDL_Renderer* renderer, int frameCount = 3) {
    vector<SDL_Texture*> dragonFrames;
    
    for (int i = 0; i < frameCount; i++) {
        string dragonPath = "src/assets/drag/dragon_" + to_string(i) + ".bmp";
        filesystem::path pDragon = dragonPath;
        string absolutePath = filesystem::absolute(pDragon);
        
        SDL_Texture* frame = loadTexture(renderer, absolutePath);
        if (frame != nullptr) {
            dragonFrames.push_back(frame);
        }
    }
    
    return dragonFrames;
}

// Load chicken animation frames
vector<SDL_Texture*> loadChickenWalkRightFrames(SDL_Renderer* renderer) {
    vector<SDL_Texture*> frames;
    for (int i = 0; i <= 2; i++) {
        string path = "src/assets/chickenFrames/chicken_walk_right_" + to_string(i) + ".bmp";
        filesystem::path p = path;
        string absolutePath = filesystem::absolute(p);
        SDL_Texture* frame = loadTexture(renderer, absolutePath);
        if (frame != nullptr) {
            frames.push_back(frame);
        }
    }
    return frames;
}

vector<SDL_Texture*> loadChickenWalkLeftFrames(SDL_Renderer* renderer) {
    vector<SDL_Texture*> frames;
    for (int i = 0; i <= 2; i++) {
        string path = "src/assets/chickenFrames/chicken_walk_left_" + to_string(i) + ".bmp";
        filesystem::path p = path;
        string absolutePath = filesystem::absolute(p);
        SDL_Texture* frame = loadTexture(renderer, absolutePath);
        if (frame != nullptr) {
            frames.push_back(frame);
        }
    }
    return frames;
}

vector<SDL_Texture*> loadChickenFeedFrames(SDL_Renderer* renderer) {
    vector<SDL_Texture*> frames;
    for (int i = 0; i <= 3; i++) {
        string path = "src/assets/chickenFrames/chicken_feed_" + to_string(i) + ".bmp";
        filesystem::path p = path;
        string absolutePath = filesystem::absolute(p);
        SDL_Texture* frame = loadTexture(renderer, absolutePath);
        if (frame != nullptr) {
            frames.push_back(frame);
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
                 Dragon& dragon, vector<Chicken>& chickens, vector<HatchingEgg>& hatchingEggs, 
                 int& currentHungerTime, bool& gameOver) {
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
    
    // Don't allow movement if game over
    if (!gameOver) {
        player.move(moveX, moveY, dir);
    }
    
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
            
            if (player.eggsCollected >= dragon.eggsRequired && dragon.feed(dragon.eggsRequired)) {
                // Remove the required number of eggs
                for (int i = 0; i < dragon.eggsRequired; i++) {
                    if (player.eggsCollected > 0) {
                        player.eggsCollected--;
                        // Remove fertilized eggs first if we have them
                        if (player.fertilizedEggs > 0) {
                            player.fertilizedEggs--;
                        }
                    }
                }
                
                // Make dragon hungrier for next time
                currentHungerTime = max(MIN_HUNGER_TIME, currentHungerTime - HUNGER_DECREMENT);
                cout << "Dragon fed with " << dragon.eggsRequired << " eggs! Next hunger in: " << currentHungerTime/1000 << " seconds\n";
            } else if (dragon.isHungry) {
                cout << "Dragon requires " << dragon.eggsRequired << " eggs, but you only have " << player.eggsCollected << "!\n";
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
    SDL_Texture* bgTexture1 = loadTexture(renderer, filesystem::absolute("src/assets/playG1.bmp"));
    SDL_Texture* bgTexture2 = loadTexture(renderer, filesystem::absolute("src/assets/playG2.bmp"));
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
    SDL_Texture* chickenTexture = loadTexture(renderer, filesystem::absolute("src/assets/drag/chicken.bmp"));
    
    // Load dragon frames for animation
    vector<SDL_Texture*> dragonFrames = loadDragonFrames(renderer, 3); // Now adjustable frame count
    
    // Load chicken animation frames
    vector<SDL_Texture*> chickenWalkRightFrames = loadChickenWalkRightFrames(renderer);
    vector<SDL_Texture*> chickenWalkLeftFrames = loadChickenWalkLeftFrames(renderer);
    vector<SDL_Texture*> chickenFeedFrames = loadChickenFeedFrames(renderer);

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

    // Create placeholders for chicken animations if needed
    if (chickenWalkRightFrames.empty()) {
        for (int i = 0; i < 3; i++) {
            Uint32 color = 0xFFFFFF00; // Yellow
            if (i == 1) color = 0xFFDDDD00;
            if (i == 2) color = 0xFFBBBB00;
            chickenWalkRightFrames.push_back(createPlaceholder(CHICKEN_SIZE, CHICKEN_SIZE, color));
        }
    }
    
    if (chickenWalkLeftFrames.empty()) {
        for (int i = 0; i < 3; i++) {
            Uint32 color = 0xFFFFFF00; // Yellow
            if (i == 1) color = 0xFFDDDD00;
            if (i == 2) color = 0xFFBBBB00;
            chickenWalkLeftFrames.push_back(createPlaceholder(CHICKEN_SIZE, CHICKEN_SIZE, color));
        }
    }
    
    if (chickenFeedFrames.empty()) {
        for (int i = 0; i < 4; i++) {
            Uint32 color = 0xFFFFFF00; // Yellow
            if (i == 1) color = 0xFFDDDD00;
            if (i == 2) color = 0xFFBBBB00;
            if (i == 3) color = 0xFF999900;
            chickenFeedFrames.push_back(createPlaceholder(CHICKEN_SIZE, CHICKEN_SIZE, color));
        }
    }

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
    int currentHungerTime = INITIAL_HUNGER_TIME;
    Character player(CHARACTER_START_X, CHARACTER_START_Y, 5.0f);
    Dragon dragon(DRAGON_X, DRAGON_Y, currentHungerTime);
    
    // Game state variables
    int dayCount = 1;
    Uint32 lastDayTime = SDL_GetTicks();
    Uint32 lastChickenDeathCheck = SDL_GetTicks();
    bool gameOver = false;
    int daysSinceLastEggRequirementIncrease = 0;
    
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
        handleInput(keyboardState, player, running, dragon, chickens, hatchingEggs, currentHungerTime, gameOver);
        
        // Update Character Position
        characterRect.x = player.x;
        characterRect.y = player.y;
        basketRect.x = player.x + int((CHARACTER_SIZE - BASKET_SIZE) / 2);
        basketRect.y = player.y - BASKET_SIZE + 15;

        
        // Update Chickens
        for (auto& chicken : chickens) {
            chicken.update(currentTime);
            
            // If chicken has egg, lay it (but only if there's no egg already at this position)
            if (chicken.hasEgg) {
                bool eggExistsAtPosition = false;
                for (const auto& egg : worldEggs) {
                    if (abs(egg.x - chicken.x) < EGG_SIZE && abs(egg.y - chicken.y) < EGG_SIZE) {
                        eggExistsAtPosition = true;
                        break;
                    }
                }
                
                if (!eggExistsAtPosition) {
                    Egg newEgg = chicken.layEgg();
                    if (newEgg.x != 0 && newEgg.y != 0) { // Valid egg
                        worldEggs.push_back(newEgg);
                        cout << "Chicken laid an egg!\n";
                    }
                }
            }
        }
        
        // Update Dragon (pass chickens reference for anger mechanic)
        dragon.update(currentTime, chickens, gameOver);
        
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
        
        // Update day counter (increment every 30 seconds)
        if (currentTime - lastDayTime > 30000) { // 30 seconds = 1 day
            dayCount++;
            daysSinceLastEggRequirementIncrease++;
            lastDayTime = currentTime;
            cout << "Day " << dayCount << " survived!\n";
            
            // Increase dragon egg requirement every 5 days
            if (daysSinceLastEggRequirementIncrease >= 5) {
                dragon.increaseEggRequirement();
                daysSinceLastEggRequirementIncrease = 0;
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

        if (CHICKEN_DEATH_INTERVAL > CHICKEN_DEATH_MIN_INTERVAL && dayCount != intervalDeathChicken) {
            CHICKEN_DEATH_INTERVAL -= 2000;
            intervalDeathChicken++;
            cout << "New Death time: "<< CHICKEN_DEATH_INTERVAL << endl;
        }

        if (HATCH_TIME > HATCH_MIN_TIME && dayCount != intervalHatchChicken) {
            HATCH_TIME -= 2000;
            intervalHatchChicken++;
            cout << "New Hatch time: "<< HATCH_TIME << endl;
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
        
        
        // Render Chicken Spots (5 fixed positions)
        for (int i = 0; i < MAX_CHICKENS; i++) {
            float spotX = CHICKEN_START_X;
            float spotY = CHICKEN_START_Y + (i * CHICKEN_SPACING);
        }
        
        // Render Chickens with animations
        for (int i = 0; i < chickens.size(); i++) {
            SDL_FRect chickenRect = chickens[i].getRect();
            SDL_Texture* currentChickenTexture = chickenTexture;
            
            // Select appropriate texture based on chicken state and direction
            switch(chickens[i].state) {
                case ChickenState::WALKING:
                    if (chickens[i].direction == Direction::RIGHT) {
                        if (!chickenWalkRightFrames.empty()) {
                            currentChickenTexture = chickenWalkRightFrames[chickens[i].currentFrame];
                        }
                    } else {
                        if (!chickenWalkLeftFrames.empty()) {
                            currentChickenTexture = chickenWalkLeftFrames[chickens[i].currentFrame];
                        }
                    }
                    break;
                    
                case ChickenState::FEEDING:
                    if (!chickenFeedFrames.empty()) {
                        currentChickenTexture = chickenFeedFrames[chickens[i].currentFrame];
                    }
                    break;
                    
                case ChickenState::STOPPED:
                    // Use default texture or first frame of walking
                    if (!chickenWalkRightFrames.empty()) {
                        currentChickenTexture = chickenWalkRightFrames[0];
                    }
                    break;
            }
            
            SDL_RenderTexture(renderer, currentChickenTexture, nullptr, &chickenRect);
            
            // Show egg indicator if chicken has egg
            if (chickens[i].hasEgg) {
                YAS_DrawCircle(chickenRect.x + int(CHICKEN_SIZE/2), chickenRect.y - 10, renderer, 5, 255, 255, 255, 1);
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
                    YAS_DrawCircle(eggRect.x + int(EGG_SIZE/2), eggRect.y - 5, renderer, 3, 0, 255, 0, 1);
                }
            }
        }
        

        // Render Character
        if (!gameOver) {
            animateCharacter(currentCharacterTexture, characterFrames, currentTime, 
                            player.isMoving, player.currentDirection);
            SDL_RenderTexture(renderer, currentCharacterTexture, nullptr, &characterRect);
        }

        // Render Basket
        if (player.eggsCollected >= 0 && player.eggsCollected < basketTextures.size()) {
            SDL_RenderTexture(renderer, basketTextures[player.eggsCollected], nullptr, &basketRect);
        }

        // Render Dragon (with animation and hunger indicator)
        if (!dragonFrames.empty()) {
            SDL_Texture* currentDragonTexture = dragonFrames[dragon.currentFrame];
            SDL_RenderTexture(renderer, currentDragonTexture, nullptr, &dragonRect);
        }

        // Render hunger indicator above dragon
        if (dragon.isHungry) {
            YAS_DrawRect(dragon.x - 500, dragon.y - 350, renderer, 15, 150, 255, 0, 0, 1); // Red hunger bar
            
            // Also render anger meter
            YAS_DrawRect(dragon.x - 500, dragon.y - 370, renderer, 15, dragon.angerLevel * 1.5f, 255, 165, 0, 1); // Orange anger bar
        } else {
            YAS_DrawRect(dragon.x - 500, dragon.y - 350, renderer, 15, 150, 0, 255, 0, 1); // Green fed bar
        }
        
        // Render UI - Egg Counter (Top Left)
        for (int i = 0; i < player.eggsCollected; i++) {
            YAS_DrawCircle(30 + i * 25, 30, renderer, 10, 255, 255, 0, 1); // Yellow for eggs
        }
        
        // Render UI - Fertilized Egg Counter
        for (int i = 0; i < player.fertilizedEggs; i++) {
            YAS_DrawCircle(30 + i * 25, 60, renderer, 10, 0, 255, 0, 1); // Green for fertilized
        }
        
        // Render Text UI using SDL_RenderDebugText (Top Right)
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White text
        string dayText = "Survived Day: " + to_string(dayCount);
        SDL_RenderDebugText(renderer, SCREEN_WIDTH - 150.0f, 30.0f, dayText.c_str());
        
        string reqText = "Dragon needs " + to_string(dragon.eggsRequired) + " eggs";
        SDL_RenderDebugText(renderer, SCREEN_WIDTH - 150.0f, 50.0f, reqText.c_str());
        
        // Render chicken count
        string chickenText = "Current Chickens: " + to_string(chickens.size());
        SDL_RenderDebugText(renderer, SCREEN_WIDTH - 150.0f, 70.0f, chickenText.c_str());

        // If game over, display message
        if (gameOver) {
            // Draw semi-transparent overlay
            YAS_DrawRect(SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 - 50, renderer, 300, 300, 130, 0, 0, 1);
            
            // Render game over text
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red text
            SDL_RenderDebugText(renderer, SCREEN_WIDTH/2 - 100.0f, SCREEN_HEIGHT/2 - 20.0f, "GAME OVER");
            
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White text
            string finalDaysText = "Days: " + to_string(dayCount);
            SDL_RenderDebugText(renderer, SCREEN_WIDTH/2 - 80.0f, SCREEN_HEIGHT/2 + 10.0f, finalDaysText.c_str());
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
    
    // Clean up chicken animation textures
    for (auto texture : chickenWalkRightFrames) {
        SDL_DestroyTexture(texture);
    }
    for (auto texture : chickenWalkLeftFrames) {
        SDL_DestroyTexture(texture);
    }
    for (auto texture : chickenFeedFrames) {
        SDL_DestroyTexture(texture);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    cout << "Game Ended. Final stats - Chickens: " << chickens.size() 
         << ", Eggs collected: " << player.eggsCollected 
         << ", Days survived: " << dayCount << "\n";
    return 0;
}
