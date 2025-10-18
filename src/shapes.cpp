#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <math.h>


// This draws us a rectangle
//          first: x  y,         draw surface,       input: x     y,    size; a b,    color: RGBA
int YAS_DrawRect(const int kax, const int kay, SDL_Surface* surface, int a, int b, int R, int G, int B, int A){
    for (int i = 0; i < a; i++) {
        
        for (int j = 0; j<b; j++) {
        
            SDL_WriteSurfacePixel(surface, kax+j, kay+i, R, G, B, A);
        }
    }

    return 1;    
}
// This draws us a circle
int YAS_DrawCircle (const int kax, const int kay, SDL_Surface* surface, int radius, int R, int G, int B, int A){
    int centerX = kax;
    int centerY = kay;

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
// // This draws a triangle
// int YAS_DrawTri(const int kax, const int kay, SDL_Surface* surface, int size, int R, int G, int B, int A){
   
//     int topX = kax ;
//     int topY = kay - size;
//     int leftX = kax - size;
//     int leftY = kay + size;
//     int rightX = kax + size;
//     int rightY = kay + size;
    
//     // Draw from top to bottom
//     for (int y = topY; y <= leftY; y++) {
//         float t = (float)(y - topY) / (leftY - topY);
//         int left = topX + (leftX - topX) * t;
//         int right = topX + (rightX - topX) * t;
        
//         for (int x = left; x <= right; x++) {
//             SDL_WriteSurfacePixel(surface, x, y, R, G, B, A);
//         }
//     }
//     return 1;
// }
// This draws a triangle with rotation
// int YAS_DrawTri(const int kax, const int kay, float angle, SDL_Surface* surface, int size, int R, int G, int B, int A) {
//     // Convert angle from degrees to radians
//     float angleRad = angle * (M_PI / 180.0);
    
//     // Pre-calculate sine and cosine for rotation
//     float cosAngle = cos(angleRad);
//     float sinAngle = sin(angleRad);
    
//     // Define the triangle vertices relative to center (before rotation)
//     int x1 = 0, y1 = -size;  // Top vertex
//     int x2 = -size, y2 = size;  // Bottom left
//     int x3 = size, y3 = size;   // Bottom right
    
//     // Rotate each vertex around the center
//     int topX = (int)(x1 * cosAngle - y1 * sinAngle) + kax;
//     int topY = (int)(x1 * sinAngle + y1 * cosAngle) + kay;
    
//     int leftX = (int)(x2 * cosAngle - y2 * sinAngle) + kax;
//     int leftY = (int)(x2 * sinAngle + y2 * cosAngle) + kay;
    
//     int rightX = (int)(x3 * cosAngle - y3 * sinAngle) + kax;
//     int rightY = (int)(x3 * sinAngle + y3 * cosAngle) + kay;
    
//     // Sort vertices by y-coordinate for drawing
//     if (topY > leftY) { 
//         int temp = topX; topX = leftX; leftX = temp; 
//         temp = topY; topY = leftY; leftY = temp; 
//     }
//     if (topY > rightY) { 
//         int temp = topX; topX = rightX; rightX = temp; 
//         temp = topY; topY = rightY; rightY = temp; 
//     }
//     if (leftY > rightY) { 
//         int temp = leftX; leftX = rightX; rightX = temp; 
//         temp = leftY; leftY = rightY; rightY = temp; 
//     }
    
//     // Draw from top to bottom
//     for (int y = topY; y <= leftY; y++) {
//         float t = (float)(y - topY) / (leftY - topY);
//         int left = topX + (leftX - topX) * t;
//         int right = topX + (rightX - topX) * t;
        
//         // Ensure left is actually left
//         if (left > right) {
//             int temp = left;
//             left = right;
//             right = temp;
//         }
        
//         for (int x = left; x <= right; x++) {
//             SDL_WriteSurfacePixel(surface, x, y, R, G, B, A);
//         }
//     }
    
//     // Draw the bottom part (from leftY to rightY)
//     for (int y = leftY; y <= rightY; y++) {
//         float t = (float)(y - leftY) / (rightY - leftY);
//         int left = leftX + (rightX - leftX) * t;
//         int right = topX + (rightX - topX) * t;
        
//         // Ensure left is actually left
//         if (left > right) {
//             int temp = left;
//             left = right;
//             right = temp;
//         }
        
//         for (int x = left; x <= right; x++) {
//             SDL_WriteSurfacePixel(surface, x, y, R, G, B, A);
//         }
//     }
    
//     return 1;
// }
// Simpler approach using barycentric coordinates
int YAS_DrawTri(const int kax, const int kay, float angle, SDL_Surface* surface, int size, int R, int G, int B, int A) {
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
                SDL_WriteSurfacePixel(surface, x, y, R, G, B, A);
            }
        }
    }
    
    return 1;
}
