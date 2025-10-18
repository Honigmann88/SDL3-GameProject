#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>


// This draws us a rectangle
int YAS_DrawRect(int kax, int kay, SDL_Surface* surface, int ax, int ay, int a, int b, int R, int G, int B, int A){
    for (int i = 0; i < a; i++) {
        
        for (int j = 0; j<b; j++) {
        
            SDL_WriteSurfacePixel(surface, kax+ax+j, kay+ay+i, R, G, B, A);
        }
    }

    return 1;    
}
// This draws us a circle
int YAS_DrawCircle (int kax, int kay, int ax, int ay, int radius, SDL_Surface* surface, int R, int G, int B, int A){
    int centerX = kax + ax;
    int centerY = kay + ay;

    for (int i = -radius; i <= radius; i++) {

        for (int j = -radius; j <= radius; j++) {
            // Check if point (i,j) is inside the circle
            if (i*i + j*j <= radius*radius) {
                SDL_WriteSurfacePixel(surface, centerX + j, centerY + i, R, G, B, A);
            }
        }
    }
    return 1;
}
