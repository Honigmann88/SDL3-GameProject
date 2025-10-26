#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <math.h>

using namespace std;


// // Direction enum for character movement
// enum class Direction { RIGHT, LEFT, UP, DOWN };

// // Character class
// class Character {
// public:
//     float x, y;
//     float speed;
//     bool isMoving;
//     Direction currentDirection;
    
//     Character(float startX, float startY, float moveSpeed) 
//         : x(startX), y(startY), speed(moveSpeed), isMoving(false), currentDirection(Direction::RIGHT) {}
    
//     void move(int dx, int dy, Direction dir) {
//         x += dx * speed;
//         y += dy * speed;
//         isMoving = (dx != 0 || dy != 0);
//         if (isMoving) {
//             currentDirection = dir;
//         }
//     }
// };

// // Texture management functions
// SDL_Texture* loadTexture(SDL_Renderer* renderer, const string& path) {
//     SDL_Surface* surface = SDL_LoadBMP(path.c_str());
//     if (surface == nullptr) {
//         cout << "Failed to load: " << path << "\n";
//         return nullptr;
//     }
    
//     SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
//     SDL_DestroySurface(surface);
    
//     return texture;
// }

// vector<SDL_Texture*> loadCharacterFrames(SDL_Renderer* renderer, const string& direction, int frameCount) {
//     vector<SDL_Texture*> frames;
    
//     for (int i = 1; i <= frameCount; i++) {
//         string framePath = "src/assets/charFrames/char_" + direction + "_" + to_string(i) + ".bmp";
//         filesystem::path pFrame = framePath;
//         string absolutePath = filesystem::absolute(pFrame);
        
//         SDL_Texture* frame = loadTexture(renderer, absolutePath);
//         if (frame != nullptr) {
//             frames.push_back(frame);
//             cout << "Loaded: " << absolutePath << "\n";
//         }
//     }
    
//     return frames;
// }

// // Animation functions
// void animateBackground(SDL_Texture*& currentTexture, SDL_Texture* tex1, SDL_Texture* tex2, Uint32 currentTime) {
//     static Uint32 lastTime = 0;
//     static bool showFirst = true;
    
//     if (currentTime - lastTime > 500) {
//         currentTexture = showFirst ? tex1 : tex2;
//         showFirst = !showFirst;
//         lastTime = currentTime;
//     }
// }

// void animateCharacter(SDL_Texture*& currentTexture, 
//                      const vector<vector<SDL_Texture*>>& allFrames, 
//                      Uint32 currentTime, 
//                      bool isMoving, 
//                      Direction direction) {
//     static Uint32 lastTime = 0;
//     static int currentFrame = 0;
    
//     // Get frames for current direction
//     const vector<SDL_Texture*>& frames = allFrames[static_cast<int>(direction)];
    
//     if (frames.empty()) return;
    
//     // Only animate if character is moving
//     if (isMoving && currentTime - lastTime > 100) {
//         currentFrame = (currentFrame + 1) % frames.size();
//         currentTexture = frames[currentFrame];
//         lastTime = currentTime;
//     }
//     // If not moving, reset to first frame of current direction
//     else if (!isMoving) {
//         currentFrame = 0;
//         currentTexture = frames[0];
//     }
// }

// // Input handling
// void handleInput(const bool* keyboardState, Character& player, bool& quit) {
//     int moveX = 0, moveY = 0;
//     Direction dir = player.currentDirection; // Default to current direction
    
//     if (keyboardState[SDL_SCANCODE_A]) {  // Left
//         moveX = -1;
//         dir = Direction::LEFT;
//     }
//     if (keyboardState[SDL_SCANCODE_D]) {  // Right
//         moveX = 1;
//         dir = Direction::RIGHT;
//     }
//     if (keyboardState[SDL_SCANCODE_W]) {  // Up
//         moveY = -1;
//         dir = Direction::UP;
//     }
//     if (keyboardState[SDL_SCANCODE_S]) {  // Down
//         moveY = 1;
//         dir = Direction::DOWN;
//     }
//     if (keyboardState[SDL_SCANCODE_ESCAPE]) {
//         quit = false;
//     }
    
//     player.move(moveX, moveY, dir);
// }

// This draws us a rectangle
//          first: x  y,         draw surface,       input: x     y,    size; a b,    color: RGBA
int YAS_DrawRect(const int kax, const int kay, SDL_Renderer* renderer, int a, int b, int R, int G, int B, int A){
    for (int i = 0; i < a; i++) {
        
        for (int j = 0; j<b; j++) {
        
            SDL_SetRenderDrawColor(renderer, R, G, B, A);
            SDL_RenderPoint(renderer, kax + j, kay + i);
        }
    }

    return 1;    
}
// This draws us a circle
int YAS_DrawCircle (const int kax, const int kay, SDL_Renderer* renderer, int radius, int R, int G, int B, int A){
    int centerX = kax;
    int centerY = kay;

    for (int i = -radius; i <= radius; i++) {

        for (int j = -radius; j <= radius; j++) {
            if (i*i + j*j <= radius*radius) {
            SDL_SetRenderDrawColor(renderer, R, G, B, A);
            SDL_RenderPoint(renderer, kax + j, kay + i);
                
            }
        }
    }
    return 1;
}
// This draws a triangle 
int YAS_DrawTri(const int kax, const int kay, float angle, SDL_Renderer* renderer, int size, int R, int G, int B, int A) {
    float angleRad = angle * (3.14159 / 180.0);
    float cosAngle = cos(angleRad);
    float sinAngle = sin(angleRad);
    
    // Define and rotate vertices
    int x1 = (int)(0 * cosAngle - (-size) * sinAngle) + kax;
    int y1 = (int)(0 * sinAngle + (-size) * cosAngle) + kay;
    
    int x2 = (int)((-size) * cosAngle - size * sinAngle) + kax;
    int y2 = (int)((-size) * sinAngle + size * cosAngle) + kay;
    
    int x3 = (int)(size * cosAngle - size * sinAngle) + kax;
    int y3 = (int)(size * sinAngle + size * cosAngle) + kay;
    
    // Find bounding box
    int minX = (x1 < x2) ? ((x1 < x3) ? x1 : x3) : ((x2 < x3) ? x2 : x3);
    int maxX = (x1 > x2) ? ((x1 > x3) ? x1 : x3) : ((x2 > x3) ? x2 : x3);
    int minY = (y1 < y2) ? ((y1 < y3) ? y1 : y3) : ((y2 < y3) ? y2 : y3);
    int maxY = (y1 > y2) ? ((y1 > y3) ? y1 : y3) : ((y2 > y3) ? y2 : y3);
    
    // Draw pixels inside triangle using barycentric coordinates
    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            // Calculate barycentric coordinates
            float denominator = ((y2 - y3)*(x1 - x3) + (x3 - x2)*(y1 - y3));
            float a = ((y2 - y3)*(x - x3) + (x3 - x2)*(y - y3)) / denominator;
            float b = ((y3 - y1)*(x - x3) + (x1 - x3)*(y - y3)) / denominator;
            float c = 1 - a - b;
            
            // If point is inside triangle
            if (a >= 0 && a <= 1 && b >= 0 && b <= 1 && c >= 0 && c <= 1) {
                // SDL_WriteSurfacePixel(surface, x, y, R, G, B, A);
                SDL_SetRenderDrawColor(renderer, R, G, B, A);
                SDL_RenderPoint(renderer, x, y);
            }
        }
    }
    
    return 1;
}
