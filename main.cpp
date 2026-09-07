#include <iostream>
#include <raylib.h>


int main(){
    int screenWidth{1280};
    int screenHeight{800};
    InitWindow(screenWidth, screenHeight, "Hello world");
    
    while(WindowShouldClose() == false){
        BeginDrawing();
        

        EndDrawing();
    }

    CloseWindow();
    return 0;
}