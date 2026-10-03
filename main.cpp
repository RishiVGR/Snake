#include <iostream>
#include <raylib.h>

namespace Config{ // using a namespace to not pollute global namespace and prevent duplication
    constexpr int height{750}; // using constexpr rather than const as values are predefined
    constexpr int width{750}; // meaning that i know the hard-coded values i wish to keep constant at compile time
    constexpr Color lightGreen = {173,204,96,255};
    constexpr Color darkGreen = {43,51,24,255};
    constexpr Color yellow = {220, 240, 38,255};
    constexpr int cellSize{30};
    constexpr int cellCount{25};

}

using namespace Config;

class Food{
    private:
        Texture2D foodTexture{};
        bool foodLoad;
    public: 
        Vector2 position = foodRandomPosition();
    public:
        void drawFood(){
                if(foodLoad==false){
                    DrawRectangle(position.x*cellSize, position.y*cellSize, cellSize, cellSize, darkGreen);
                }
                else{
                    DrawTexture(foodTexture, position.x*cellSize, position.y*cellSize, WHITE); // last value is tint coloru - white means no tint
                }
            }
        Vector2 foodRandomPosition(){ // return type vector 2
            float x = GetRandomValue(0, cellCount - 1); // produce x value between 0 and 24 (25 values)
            float y = GetRandomValue(0, cellCount - 1); // produce x value between 0 and 24 (25 values)
            return Vector2{x,y};
        }
        Food(const std::string& imagePath){ // pass const reference path into constructor
            foodLoad = true;
            Image foodImage = LoadImage(imagePath.c_str()); // transforms string into c-style string (const char*)
            if(foodImage.data == nullptr){
                std::cout << "[WARN] Failed to load image - check path: " << imagePath << std::endl;
                foodLoad = false;
            }
            else{
                std::cout << "[LOG] Image texture succesfully loaded" << std::endl;
                ImageResize(&foodImage, 30, 30); // modify image at memory adress of FoodImage
                foodTexture = LoadTextureFromImage(foodImage); // transform image to texture - making it more efficient
                
                UnloadImage(foodImage);
            }
        }
        ~Food(){
            UnloadTexture(foodTexture);
            std::cout << "[LOG] Texture successfully unloaded from memory" << std::endl;
        }
};


int main(){
    // window init
    const char* title = "Snake Game";
    std::string path;
    std::cout << "Enter path for food image: ";
    std::cin >> path;
    InitWindow(Config::height, Config::width, title);
    SetTargetFPS(60);
    
    //objects
    Food foodObject(path);


    while(WindowShouldClose() == false){ // game loop should handle events and updating positions, alongside drawing objects
        
        ClearBackground(lightGreen);
        foodObject.drawFood();    
        EndDrawing();
        
    }
    CloseWindow();
    return 0;
}