#include <iostream>
#include <raylib.h>


int main(){
    const char* title{"Snake"};
    const int screenWidth{900};
    const int screenHeight{900};
    InitWindow(screenWidth, screenHeight, title);

    SetTargetFPS(60);
    
    while(WindowShouldClose() == false){
        
        BeginDrawing();
        ClearBackground(WHITE);
        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}