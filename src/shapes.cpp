#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <math.h>


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
