 #include <SDL3/SDL_main.h>
 #include <SDL3/SDL_render.h>
 #include <iostream>
 #include <filesystem>
 #include <string>
 #include <vector>
 #include <math.h>

 using namespace std;

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





// #include <SDL3/SDL_main.h>
// #include <SDL3/SDL_render.h>
// #include <iostream>
// #include <filesystem>
// #include <string>
// #include <vector>
// #include <math.h>

// using namespace std;

// // Forward declarations for shape functions
// int YAS_DrawRect(const int kax, const int kay, SDL_Renderer* renderer, int a, int b, int R, int G, int B, int A);
// int YAS_DrawCircle(const int kax, const int kay, SDL_Renderer* renderer, int radius, int R, int G, int B, int A);
// int YAS_DrawTri(const int kax, const int kay, float angle, SDL_Renderer* renderer, int size, int R, int G, int B, int A);

// // NOW ADD ALL YOUR LETTER FUNCTIONS HERE (A-Z and DrawText)
// // Copy all the letter function code from my previous response here...
// // Add this after your existing shape functions

// // Letter drawing functions
// int YAS_DrawA(const int kax, const int kay, SDL_Renderer* renderer, int size, int R, int G, int B, int A) {
//     int thickness = size / 5;
//     // Left diagonal
//     YAS_DrawTri(kax, kay, 30, renderer, size, R, G, B, A);
//     // Right diagonal
//     YAS_DrawTri(kax + size, kay, 150, renderer, size, R, G, B, A);
//     // Middle bar
//     YAS_DrawRect(kax + size/3, kay + size/2, renderer, size/3, thickness, R, G, B, A);
//     return 1;
// }

// int YAS_DrawB(const int kax, const int kay, SDL_Renderer* renderer, int size, int R, int G, int B, int A) {
//     int thickness = size / 5;
//     // Vertical line
//     YAS_DrawRect(kax, kay, renderer, thickness, size, R, G, B, A);
//     // Top curve
//     YAS_DrawCircle(kax + size/2, kay + size/3, renderer, size/3, R, G, B, A);
//     // Bottom curve
//     YAS_DrawCircle(kax + size/2, kay + 2*size/3, renderer, size/3, R, G, B, A);
//     // Fill cutout
//     YAS_DrawRect(kax + thickness, kay + size/2, renderer, size/2 - thickness, thickness, 0, 0, 0, 255);
//     return 1;
// }

// int YAS_DrawC(const int kax, const int kay, SDL_Renderer* renderer, int size, int R, int G, int B, int A) {
//     YAS_DrawCircle(kax + size/2, kay + size/2, renderer, size/2, R, G, B, A);
//     // Cut out inner part to make C shape
//     YAS_DrawRect(kax + size/2, kay, renderer, size/2, size, 0, 0, 0, 255);
//     YAS_DrawRect(kax, kay + size/3, renderer, size/3, size/3, 0, 0, 0, 255);
//     return 1;
// }

// int YAS_DrawD(const int kax, const int kay, SDL_Renderer* renderer, int size, int R, int G, int B, int A) {
//     int thickness = size / 5;
//     // Vertical line
//     YAS_DrawRect(kax, kay, renderer, thickness, size, R, G, B, A);
//     // Semi-circle
//     YAS_DrawCircle(kax + size/2, kay + size/2, renderer, size/2, R, G, B, A);
//     // Cut out left part
//     YAS_DrawRect(kax, kay, renderer, size/2, size, 0, 0, 0, 255);
//     return 1;
// }

// int YAS_DrawE(const int kax, const int kay, SDL_Renderer* renderer, int size, int R, int G, int B, int A) {
//     int thickness = size / 5;
//     // Vertical line
//     YAS_DrawRect(kax, kay, renderer, thickness, size, R, G, B, A);
//     // Top horizontal
//     YAS_DrawRect(kax, kay, renderer, size, thickness, R, G, B, A);
//     // Middle horizontal
//     YAS_DrawRect(kax, kay + size/2 - thickness/2, renderer, 2*size/3, thickness, R, G, B, A);
//     // Bottom horizontal
//     YAS_DrawRect(kax, kay + size - thickness, renderer, size, thickness, R, G, B, A);
//     return 1;
// }

// int YAS_DrawF(const int kax, const int kay, SDL_Renderer* renderer, int size, int R, int G, int B, int A) {
//     int thickness = size / 5;
//     // Vertical line
//     YAS_DrawRect(kax, kay, renderer, thickness, size, R, G, B, A);
//     // Top horizontal
//     YAS_DrawRect(kax, kay, renderer, size, thickness, R, G, B, A);
//     // Middle horizontal
//     YAS_DrawRect(kax, kay + size/2 - thickness/2, renderer, 2*size/3, thickness, R, G, B, A);
//     return 1;
// }

// int YAS_DrawG(const int kax, const int kay, SDL_Renderer* renderer, int size, int R, int G, int B, int A) {
//     YAS_DrawCircle(kax + size/2, kay + size/2, renderer, size/2, R, G, B, A);
//     // Cut out to make G shape
//     YAS_DrawRect(kax + size/2, kay, renderer, size/2, size, 0, 0, 0, 255);
//     YAS_DrawRect(kax, kay + size/3, renderer, size/3, size/3, 0, 0, 0, 255);
//     // Add the horizontal bar for G
//     YAS_DrawRect(kax + size/2, kay + size/2, renderer, size/3, size/5, R, G, B, A);
//     return 1;
// }

// int YAS_DrawH(const int kax, const int kay, SDL_Renderer* renderer, int size, int R, int G, int B, int A) {
//     int thickness = size / 5;
//     // Left vertical
//     YAS_DrawRect(kax, kay, renderer, thickness, size, R, G, B, A);
//     // Right vertical
//     YAS_DrawRect(kax + size - thickness, kay, renderer, thickness, size, R, G, B, A);
//     // Middle horizontal
//     YAS_DrawRect(kax, kay + size/2 - thickness/2, renderer, size, thickness, R, G, B, A);
//     return 1;
// }

// int YAS_DrawI(const int kax, const int kay, SDL_Renderer* renderer, int size, int R, int G, int B, int A) {
//     int thickness = size / 5;
//     // Top horizontal
//     YAS_DrawRect(kax, kay, renderer, size, thickness, R, G, B, A);
//     // Bottom horizontal
//     YAS_DrawRect(kax, kay + size - thickness, renderer, size, thickness, R, G, B, A);
//     // Middle vertical
//     YAS_DrawRect(kax + size/2 - thickness/2, kay, renderer, thickness, size, R, G, B, A);
//     return 1;
// }

// int YAS_DrawJ(const int kax, const int kay, SDL_Renderer* renderer, int size, int R, int G, int B, int A) {
//     int thickness = size / 5;
//     // Top horizontal
//     YAS_DrawRect(kax, kay, renderer, size, thickness, R, G, B, A);
//     // Right vertical (shorter)
//     YAS_DrawRect(kax + size - thickness, kay, renderer, thickness, 3*size/4, R, G, B, A);
//     // Bottom curve
//     YAS_DrawCircle(kax + size/2, kay + 3*size/4, renderer, size/2, R, G, B, A);
//     // Cut out top part of circle
//     YAS_DrawRect(kax, kay, renderer, size, size/2, 0, 0, 0, 255);
//     return 1;
// }

// int YAS_DrawK(const int kax, const int kay, SDL_Renderer* renderer, int size, int R, int G, int B, int A) {
//     int thickness = size / 5;
//     // Vertical line
//     YAS_DrawRect(kax, kay, renderer, thickness, size, R, G, B, A);
//     // Top diagonal
//     YAS_DrawTri(kax + thickness, kay, 45, renderer, size/2, R, G, B, A);
//     // Bottom diagonal
//     YAS_DrawTri(kax + thickness, kay + size, -45, renderer, size/2, R, G, B, A);
//     return 1;
// }

// int YAS_DrawL(const int kax, const int kay, SDL_Renderer* renderer, int size, int R, int G, int B, int A) {
//     int thickness = size / 5;
//     // Vertical line
//     YAS_DrawRect(kax, kay, renderer, thickness, size, R, G, B, A);
//     // Bottom horizontal
//     YAS_DrawRect(kax, kay + size - thickness, renderer, size, thickness, R, G, B, A);
//     return 1;
// }

// int YAS_DrawM(const int kax, const int kay, SDL_Renderer* renderer, int size, int R, int G, int B, int A) {
//     int thickness = size / 5;
//     // Left vertical
//     YAS_DrawRect(kax, kay, renderer, thickness, size, R, G, B, A);
//     // Right vertical
//     YAS_DrawRect(kax + size - thickness, kay, renderer, thickness, size, R, G, B, A);
//     // Left diagonal
//     YAS_DrawTri(kax + thickness, kay, 60, renderer, size/2, R, G, B, A);
//     // Right diagonal
//     YAS_DrawTri(kax + size - thickness, kay, 120, renderer, size/2, R, G, B, A);
//     return 1;
// }

// int YAS_DrawN(const int kax, const int kay, SDL_Renderer* renderer, int size, int R, int G, int B, int A) {
//     int thickness = size / 5;
//     // Left vertical
//     YAS_DrawRect(kax, kay, renderer, thickness, size, R, G, B, A);
//     // Right vertical
//     YAS_DrawRect(kax + size - thickness, kay, renderer, thickness, size, R, G, B, A);
//     // Diagonal
//     for (int i = 0; i < thickness; i++) {
//         YAS_DrawTri(kax + i, kay, 45, renderer, size, R, G, B, A);
//     }
//     return 1;
// }

// int YAS_DrawO(const int kax, const int kay, SDL_Renderer* renderer, int size, int R, int G, int B, int A) {
//     return YAS_DrawCircle(kax + size/2, kay + size/2, renderer, size/2, R, G, B, A);
// }

// int YAS_DrawP(const int kax, const int kay, SDL_Renderer* renderer, int size, int R, int G, int B, int A) {
//     int thickness = size / 5;
//     // Vertical line
//     YAS_DrawRect(kax, kay, renderer, thickness, size, R, G, B, A);
//     // Top curve
//     YAS_DrawCircle(kax + size/2, kay + size/3, renderer, size/3, R, G, B, A);
//     // Cut out bottom part
//     YAS_DrawRect(kax + thickness, kay + size/2, renderer, size, size/2, 0, 0, 0, 255);
//     return 1;
// }

// int YAS_DrawQ(const int kax, const int kay, SDL_Renderer* renderer, int size, int R, int G, int B, int A) {
//     // Circle part
//     YAS_DrawCircle(kax + size/2, kay + size/2, renderer, size/2, R, G, B, A);
//     // Tail diagonal
//     YAS_DrawTri(kax + size/2, kay + size/2, 45, renderer, size/3, R, G, B, A);
//     return 1;
// }

// int YAS_DrawR(const int kax, const int kay, SDL_Renderer* renderer, int size, int R, int G, int B, int A) {
//     int thickness = size / 5;
//     // Vertical line
//     YAS_DrawRect(kax, kay, renderer, thickness, size, R, G, B, A);
//     // Top curve
//     YAS_DrawCircle(kax + size/2, kay + size/3, renderer, size/3, R, G, B, A);
//     // Bottom diagonal
//     YAS_DrawTri(kax + thickness, kay + size/2, 45, renderer, size/2, R, G, B, A);
//     // Cut out middle part
//     YAS_DrawRect(kax + thickness, kay + size/2, renderer, size/2 - thickness, thickness, 0, 0, 0, 255);
//     return 1;
// }

// int YAS_DrawS(const int kax, const int kay, SDL_Renderer* renderer, int size, int R, int G, int B, int A) {
//     // Top curve
//     YAS_DrawCircle(kax + size/2, kay + size/3, renderer, size/3, R, G, B, A);
//     // Bottom curve
//     YAS_DrawCircle(kax + size/2, kay + 2*size/3, renderer, size/3, R, G, B, A);
//     // Cut out to make S shape
//     YAS_DrawRect(kax + size/2, kay, renderer, size/2, size/2, 0, 0, 0, 255);
//     YAS_DrawRect(kax, kay + size/2, renderer, size/2, size/2, 0, 0, 0, 255);
//     return 1;
// }

// int YAS_DrawT(const int kax, const int kay, SDL_Renderer* renderer, int size, int R, int G, int B, int A) {
//     int thickness = size / 5;
//     // Top horizontal
//     YAS_DrawRect(kax, kay, renderer, size, thickness, R, G, B, A);
//     // Middle vertical
//     YAS_DrawRect(kax + size/2 - thickness/2, kay, renderer, thickness, size, R, G, B, A);
//     return 1;
// }

// int YAS_DrawU(const int kax, const int kay, SDL_Renderer* renderer, int size, int R, int G, int B, int A) {
//     int thickness = size / 5;
//     // Left vertical
//     YAS_DrawRect(kax, kay, renderer, thickness, 3*size/4, R, G, B, A);
//     // Right vertical
//     YAS_DrawRect(kax + size - thickness, kay, renderer, thickness, 3*size/4, R, G, B, A);
//     // Bottom curve
//     YAS_DrawCircle(kax + size/2, kay + 3*size/4, renderer, size/2, R, G, B, A);
//     // Cut out top part of circle
//     YAS_DrawRect(kax, kay, renderer, size, size/2, 0, 0, 0, 255);
//     return 1;
// }

// int YAS_DrawV(const int kax, const int kay, SDL_Renderer* renderer, int size, int R, int G, int B, int A) {
//     // Left diagonal
//     YAS_DrawTri(kax, kay, 30, renderer, size, R, G, B, A);
//     // Right diagonal
//     YAS_DrawTri(kax + size, kay, 150, renderer, size, R, G, B, A);
//     return 1;
// }

// int YAS_DrawW(const int kax, const int kay, SDL_Renderer* renderer, int size, int R, int G, int B, int A) {
//     int thickness = size / 5;
//     // Left vertical
//     YAS_DrawRect(kax, kay, renderer, thickness, size, R, G, B, A);
//     // Middle left vertical (shorter)
//     YAS_DrawRect(kax + size/3, kay + size/3, renderer, thickness, 2*size/3, R, G, B, A);
//     // Middle right vertical (shorter)
//     YAS_DrawRect(kax + 2*size/3, kay + size/3, renderer, thickness, 2*size/3, R, G, B, A);
//     // Right vertical
//     YAS_DrawRect(kax + size - thickness, kay, renderer, thickness, size, R, G, B, A);
//     return 1;
// }

// int YAS_DrawX(const int kax, const int kay, SDL_Renderer* renderer, int size, int R, int G, int B, int A) {
//     // Forward diagonal
//     YAS_DrawTri(kax, kay, 45, renderer, size, R, G, B, A);
//     // Backward diagonal
//     YAS_DrawTri(kax + size, kay, 135, renderer, size, R, G, B, A);
//     return 1;
// }

// int YAS_DrawY(const int kax, const int kay, SDL_Renderer* renderer, int size, int R, int G, int B, int A) {
//     int thickness = size / 5;
//     // Top left diagonal
//     YAS_DrawTri(kax, kay, 30, renderer, size/2, R, G, B, A);
//     // Top right diagonal
//     YAS_DrawTri(kax + size, kay, 150, renderer, size/2, R, G, B, A);
//     // Bottom vertical
//     YAS_DrawRect(kax + size/2 - thickness/2, kay + size/2, renderer, thickness, size/2, R, G, B, A);
//     return 1;
// }

// int YAS_DrawZ(const int kax, const int kay, SDL_Renderer* renderer, int size, int R, int G, int B, int A) {
//     int thickness = size / 5;
//     // Top horizontal
//     YAS_DrawRect(kax, kay, renderer, size, thickness, R, G, B, A);
//     // Diagonal
//     YAS_DrawTri(kax, kay + size, 45, renderer, size, R, G, B, A);
//     // Bottom horizontal
//     YAS_DrawRect(kax, kay + size - thickness, renderer, size, thickness, R, G, B, A);
//     return 1;
// }

// // Text drawing function that uses the individual letter functions
// int YAS_DrawText(const char* text, const int kax, const int kay, SDL_Renderer* renderer, 
//                  int size, int spacing, int R, int G, int B, int A) {
//     int x = kax;
//     int y = kay;
    
//     for (int i = 0; text[i] != '\0'; i++) {
//         char c = text[i];
        
//         switch (c) {
//             case 'A': case 'a': YAS_DrawA(x, y, renderer, size, R, G, B, A); break;
//             case 'B': case 'b': YAS_DrawB(x, y, renderer, size, R, G, B, A); break;
//             case 'C': case 'c': YAS_DrawC(x, y, renderer, size, R, G, B, A); break;
//             case 'D': case 'd': YAS_DrawD(x, y, renderer, size, R, G, B, A); break;
//             case 'E': case 'e': YAS_DrawE(x, y, renderer, size, R, G, B, A); break;
//             case 'F': case 'f': YAS_DrawF(x, y, renderer, size, R, G, B, A); break;
//             case 'G': case 'g': YAS_DrawG(x, y, renderer, size, R, G, B, A); break;
//             case 'H': case 'h': YAS_DrawH(x, y, renderer, size, R, G, B, A); break;
//             case 'I': case 'i': YAS_DrawI(x, y, renderer, size, R, G, B, A); break;
//             case 'J': case 'j': YAS_DrawJ(x, y, renderer, size, R, G, B, A); break;
//             case 'K': case 'k': YAS_DrawK(x, y, renderer, size, R, G, B, A); break;
//             case 'L': case 'l': YAS_DrawL(x, y, renderer, size, R, G, B, A); break;
//             case 'M': case 'm': YAS_DrawM(x, y, renderer, size, R, G, B, A); break;
//             case 'N': case 'n': YAS_DrawN(x, y, renderer, size, R, G, B, A); break;
//             case 'O': case 'o': YAS_DrawO(x, y, renderer, size, R, G, B, A); break;
//             case 'P': case 'p': YAS_DrawP(x, y, renderer, size, R, G, B, A); break;
//             case 'Q': case 'q': YAS_DrawQ(x, y, renderer, size, R, G, B, A); break;
//             case 'R': case 'r': YAS_DrawR(x, y, renderer, size, R, G, B, A); break;
//             case 'S': case 's': YAS_DrawS(x, y, renderer, size, R, G, B, A); break;
//             case 'T': case 't': YAS_DrawT(x, y, renderer, size, R, G, B, A); break;
//             case 'U': case 'u': YAS_DrawU(x, y, renderer, size, R, G, B, A); break;
//             case 'V': case 'v': YAS_DrawV(x, y, renderer, size, R, G, B, A); break;
//             case 'W': case 'w': YAS_DrawW(x, y, renderer, size, R, G, B, A); break;
//             case 'X': case 'x': YAS_DrawX(x, y, renderer, size, R, G, B, A); break;
//             case 'Y': case 'y': YAS_DrawY(x, y, renderer, size, R, G, B, A); break;
//             case 'Z': case 'z': YAS_DrawZ(x, y, renderer, size, R, G, B, A); break;
//             case ' ': // Space
//                 break;
//             default:
//                 // Draw a rectangle for unknown characters
//                 YAS_DrawRect(x, y, renderer, size, size, R, G, B, A);
//                 break;
//         }
        
//         x += size + spacing; // Move to next character position
//     }
    
//     return 1;
// }
// // THEN KEEP YOUR EXISTING SHAPE FUNCTION IMPLEMENTATIONS
// // This draws us a rectangle
// int YAS_DrawRect(const int kax, const int kay, SDL_Renderer* renderer, int a, int b, int R, int G, int B, int A){
//     for (int i = 0; i < a; i++) {
//         for (int j = 0; j<b; j++) {
//             SDL_SetRenderDrawColor(renderer, R, G, B, A);
//             SDL_RenderPoint(renderer, kax + j, kay + i);
//         }
//     }
//     return 1;    
// }

// // This draws us a circle
// int YAS_DrawCircle (const int kax, const int kay, SDL_Renderer* renderer, int radius, int R, int G, int B, int A){
//     int centerX = kax;
//     int centerY = kay;

//     for (int i = -radius; i <= radius; i++) {
//         for (int j = -radius; j <= radius; j++) {
//             if (i*i + j*j <= radius*radius) {
//                 SDL_SetRenderDrawColor(renderer, R, G, B, A);
//                 SDL_RenderPoint(renderer, kax + j, kay + i);
//             }
//         }
//     }
//     return 1;
// }

// // This draws a triangle 
// int YAS_DrawTri(const int kax, const int kay, float angle, SDL_Renderer* renderer, int size, int R, int G, int B, int A) {
//     float angleRad = angle * (3.14159 / 180.0);
//     float cosAngle = cos(angleRad);
//     float sinAngle = sin(angleRad);
    
//     // Define and rotate vertices
//     int x1 = (int)(0 * cosAngle - (-size) * sinAngle) + kax;
//     int y1 = (int)(0 * sinAngle + (-size) * cosAngle) + kay;
    
//     int x2 = (int)((-size) * cosAngle - size * sinAngle) + kax;
//     int y2 = (int)((-size) * sinAngle + size * cosAngle) + kay;
    
//     int x3 = (int)(size * cosAngle - size * sinAngle) + kax;
//     int y3 = (int)(size * sinAngle + size * cosAngle) + kay;
    
//     // Find bounding box
//     int minX = (x1 < x2) ? ((x1 < x3) ? x1 : x3) : ((x2 < x3) ? x2 : x3);
//     int maxX = (x1 > x2) ? ((x1 > x3) ? x1 : x3) : ((x2 > x3) ? x2 : x3);
//     int minY = (y1 < y2) ? ((y1 < y3) ? y1 : y3) : ((y2 < y3) ? y2 : y3);
//     int maxY = (y1 > y2) ? ((y1 > y3) ? y1 : y3) : ((y2 > y3) ? y2 : y3);
    
//     // Draw pixels inside triangle using barycentric coordinates
//     for (int y = minY; y <= maxY; y++) {
//         for (int x = minX; x <= maxX; x++) {
//             // Calculate barycentric coordinates
//             float denominator = ((y2 - y3)*(x1 - x3) + (x3 - x2)*(y1 - y3));
//             float a = ((y2 - y3)*(x - x3) + (x3 - x2)*(y - y3)) / denominator;
//             float b = ((y3 - y1)*(x - x3) + (x1 - x3)*(y - y3)) / denominator;
//             float c = 1 - a - b;
            
//             // If point is inside triangle
//             if (a >= 0 && a <= 1 && b >= 0 && b <= 1 && c >= 0 && c <= 1) {
//                 SDL_SetRenderDrawColor(renderer, R, G, B, A);
//                 SDL_RenderPoint(renderer, x, y);
//             }
//         }
//     }
    
//     return 1;
// }
