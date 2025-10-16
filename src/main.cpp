#include <SDL3/SDL.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <iostream>

class SDLException : public std::runtime_error {
public:
    SDLException(const std::string& message) 
        : std::runtime_error(message + ": " + SDL_GetError()) {}
};

class Texture {
public:
    // Create texture from BMP file
    Texture(SDL_Renderer* renderer, const std::string& filePath) 
        : texture_(nullptr, SDL_DestroyTexture) {
        
        // Load BMP file
        SDL_Surface* surface = SDL_LoadBMP(filePath.c_str());
        if (!surface) {
            throw SDLException("Failed to load BMP: " + filePath);
        }
        
        // Create texture from surface
        texture_.reset(SDL_CreateTextureFromSurface(renderer, surface));
        SDL_DestroySurface(surface);
        
        if (!texture_) {
            throw SDLException("Failed to create texture from BMP");
        }
        
        // Get texture dimensions
        SDL_GetTextureSize(texture_.get(), &width_, &height_);
        std::cout << "Loaded texture: " << filePath << " (" << width_ << "x" << height_ << ")" << std::endl;
    }
    
    // Create a colored texture (fallback)
    Texture(SDL_Renderer* renderer, int width, int height, SDL_Color color) 
        : texture_(nullptr, SDL_DestroyTexture),
          width_(width), height_(height) {
        
        // Create a blank texture
        texture_.reset(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, 
                                        SDL_TEXTUREACCESS_TARGET, width, height));
        if (!texture_) {
            throw SDLException("Failed to create texture");
        }
        
        // Set as render target
        SDL_Texture* previousTarget = SDL_GetRenderTarget(renderer);
        SDL_SetRenderTarget(renderer, texture_.get());
        
        // Fill with color
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderClear(renderer);
        
        // Restore previous target
        SDL_SetRenderTarget(renderer, previousTarget);
    }
    
    SDL_Texture* get() const { return texture_.get(); }
    float getWidth() const { return width_; }
    float getHeight() const { return height_; }

private:
    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> texture_;
    float width_;
    float height_;
};

class Scene {
public:
    virtual ~Scene() = default;
    virtual void handleEvents(const SDL_Event& event) = 0;
    virtual void update() = 0;
    virtual void render(SDL_Renderer* renderer) = 0;
};

class Scene2 : public Scene {
public:
    Scene2(SDL_Renderer* renderer) {
        try {
            // Load background texture from BMP file
            backgroundTexture_ = std::make_unique<Texture>(renderer, "assets/gameBg.bmp");
        } catch (const std::exception& e) {
            std::cout << "Failed to load background BMP: " << e.what() << std::endl;
            std::cout << "Using fallback colored background..." << std::endl;
            
            // Fallback: create a solid color background
            SDL_Color fallbackColor = {40, 40, 80, 255};
            backgroundTexture_ = std::make_unique<Texture>(renderer, 900, 900, fallbackColor);
        }
    }
    
    void handleEvents(const SDL_Event& event) override {
        switch (event.type) {
            case SDL_EVENT_KEY_DOWN:
                std::cout << "Key pressed in Scene 2: " << event.key.key << std::endl;
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                std::cout << "Mouse clicked at: " << event.button.x << ", " << event.button.y << std::endl;
                break;
        }
    }
    
    void update() override {
        // Scene 2 update logic will go here
    }
    
    void render(SDL_Renderer* renderer) override {
        // Render background
        if (backgroundTexture_) {
            SDL_FRect destRect = {0, 0, 900, 900};
            SDL_RenderTexture(renderer, backgroundTexture_->get(), nullptr, &destRect);
        }
        
        // You can add more rendering here for game objects
        // Example: Draw a simple rectangle in the center
        SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255);
        SDL_FRect playerRect = {400, 400, 100, 100};
        SDL_RenderFillRect(renderer, &playerRect);
    }

private:
    std::unique_ptr<Texture> backgroundTexture_;
};

class Game {
public:
    Game();
    ~Game();
    
    void run();
    void switchScene(std::unique_ptr<Scene> newScene);
    
private:
    void handleEvents();
    void update();
    void render();
    
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window_;
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer_;
    std::unique_ptr<Scene> currentScene_;
    bool running_{true};
};

Game::Game() 
    : window_(nullptr, SDL_DestroyWindow)
    , renderer_(nullptr, SDL_DestroyRenderer) {
    
    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        throw SDLException("Failed to initialize SDL");
    }
    
    // Create window
    window_.reset(SDL_CreateWindow("2D Game - Scene 2", 900, 900, SDL_WINDOW_RESIZABLE));
    if (!window_) {
        throw SDLException("Failed to create window");
    }
    
    // Create renderer
    renderer_.reset(SDL_CreateRenderer(window_.get(), nullptr));
    if (!renderer_) {
        throw SDLException("Failed to create renderer");
    }
    
    // Start with Scene 2
    switchScene(std::make_unique<Scene2>(renderer_.get()));
}

Game::~Game() {
    SDL_Quit();
}

void Game::run() {
    while (running_) {
        handleEvents();
        update();
        render();
        
        // Cap at 60 FPS
        SDL_Delay(16);
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                running_ = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_ESCAPE) {
                    running_ = false;
                }
                break;
        }
        
        if (currentScene_) {
            currentScene_->handleEvents(event);
        }
    }
}

void Game::update() {
    if (currentScene_) {
        currentScene_->update();
    }
}

void Game::render() {
    // Clear screen (backup in case scene doesn't cover everything)
    SDL_SetRenderDrawColor(renderer_.get(), 0, 0, 0, 255);
    SDL_RenderClear(renderer_.get());
    
    // Render current scene
    if (currentScene_) {
        currentScene_->render(renderer_.get());
    }
    
    // Present the renderer
    SDL_RenderPresent(renderer_.get());
}

void Game::switchScene(std::unique_ptr<Scene> newScene) {
    currentScene_ = std::move(newScene);
}

int main(int argc, char* argv[]) {
    try {
        Game game;
        game.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Game failed: " << e.what() << std::endl;
        return 1;
    }
}
