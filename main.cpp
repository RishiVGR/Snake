#include <iostream>
#include <raylib.h>
#include <deque> 
#include <raymath.h>

namespace Config{ // using a namespace to not pollute global namespace and prevent duplication
    constexpr int height{750}; // using constexpr rather than const as values are predefined
    constexpr int width{750}; // meaning that i know the hard-coded values i wish to keep constant at compile time
    constexpr Color lightGreen = {173,204,96,255};
    constexpr Color darkGreen = {43,51,24,255};
    constexpr Color yellow = {220, 240, 38,255};
    constexpr Color snakeBlue = {66,135,245,255};
    constexpr int cellSize{30};
    constexpr int cellCount{25};
    constexpr int offset{75};

}

using namespace Config;

class gameTime{ // this class is used to manage snake frames
    gameTime() = delete; // got rid of constructor as no need for instances of this class
    public:
        static inline float lastUpdateTime = 0.0f; // used static inline - as previously all static compiler assigned variables in class
        // had to be const. without inline - compiler throws error that c++ didnt know where to create variable in memory unless constant
        // inline tells compiler to create variable inside of the class (even if it changes during runtime(as it is in our case))
        // variable has to be static as since constructor is deleted, member initialized variables (which belong to instances) dont exist
        public:
        static bool snakeTimeElapsed(float interval){ // static so that class can access it, furthermore so it can read static lastUpdateTime
            float currentTime = GetTime();
            if(currentTime-lastUpdateTime >= interval ){
                lastUpdateTime = currentTime;
                return 1;
            }
            else{
                return 0;
            }
        }

};

class snake{
    public:
        bool running = true;
        std::deque<Vector2> body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}}; // using deque type to easily add and remove vec2s in array from both ends easier
        Vector2 direction = {1,0};
        Sound eat;
        Sound defeat;
    public:
        snake(){
            InitAudioDevice();
            eat = LoadSound("/Users/rishi/Downloads/cartoon_music-arcade-game-achievement-bling-489759.mp3");
            defeat = LoadSound("/Users/rishi/Downloads/freesound_community-game-over-arcade-6435.mp3");
        }
        ~snake(){
            UnloadSound(eat);
            UnloadSound(defeat);
            CloseAudioDevice();
            std::cout << "[LOG] Successfully unloaded audio" << std::endl;
        }
        void draw(){
            for (unsigned int i = 0; i < body.size(); i++){ // set i to unsigned, as body.size returns unsigned int (optional)
                if(i == 0){
                    DrawRectangle(offset+ body[i].x*cellSize,offset + body[i].y*cellSize, cellSize, cellSize, snakeBlue); // check if we are drawing head, if so make color distinction
                }
                else{
                    DrawRectangle(offset+body[i].x*cellSize,offset+ body[i].y*cellSize, cellSize, cellSize, darkGreen); // if not head, draw dark green
                }
            }
        }
        void update(){ // if i want to put input logic in class, would need to seperate as input would only be checked every 0.2s
            body.push_front(direction + body[0]);
            body.pop_back(); // remove last square from deque (creates ilusion of moving forwards)
        }
        void inputLogic(){
            if(IsKeyPressed(KEY_UP) && direction != Vector2{0,1}){ // one way to counteract 180 turns 
            // (make sure to outline that {0,1} is vector2 type)
                direction = {0,-1};
                running = true;
            }
            else if(IsKeyPressed(KEY_DOWN) && direction.y != -1){ // another way
                direction = {0,1};
                running = true;
            }
            else if(IsKeyPressed(KEY_LEFT) && direction != Vector2{1,0}){
                direction = {-1, 0};
                running = true;
            }
            else if(IsKeyPressed(KEY_RIGHT) && direction.x != -1){
                direction = {1,0};
                running = true;
            }
        }
        void gameOver(){
            std::cout << "Game Over" << std::endl;
            running = false;
            body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
            
            
        }
        void collisionWithWall(){
            if(body[0].x >= cellCount|| body[0].x <0){
                direction = {0,0};
                PlaySound(defeat);
                gameOver();
            }
            if(body[0].y <= 0 || body[0].y >= cellCount){
                direction = {0,0};
                PlaySound(defeat);
                gameOver();
            }

        }
        void collisionWithTail(){
            for(int i = 1; i < body.size(); i++){
                if(body[0].x == body[i].x && body[0].y == body[i].y){
                    direction = {0,0};
                    PlaySound(defeat);
                    gameOver();
                }
            }
        }

};

class Food{
    private:
        Texture2D foodTexture{};
        bool foodLoad;
    public: 
        Vector2 position = foodRandomPosition();
    public:
        void drawFood(){
                if(foodLoad==false){
                    DrawRectangle(offset+ position.x*cellSize,offset + position.y*cellSize, cellSize, cellSize, darkGreen);
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
    InitWindow(Config::height +2*offset, Config::width+ 2*offset, title);
    SetTargetFPS(30);
    
    //objects
    Food foodObject(path);
    snake snakeObject;


    while(WindowShouldClose() == false){ // game loop should handle events and updating positions, alongside drawing objects
        snakeObject.inputLogic();
        if(snakeObject.body[0] == foodObject.position){
                foodObject.position = foodObject.foodRandomPosition();
                snakeObject.body.push_front(snakeObject.direction + snakeObject.body[0]);
                PlaySound(snakeObject.eat);
            }
        snakeObject.collisionWithWall();
        snakeObject.collisionWithTail();  
        if(snakeObject.running){
            if(gameTime::snakeTimeElapsed(0.2)){
                snakeObject.update();
            }
                      
        }    

        ClearBackground(lightGreen);
        DrawRectangleLinesEx(Rectangle{offset-5, offset - 5, cellSize*cellCount+10, cellSize*cellCount+10}, 5, darkGreen);
        foodObject.drawFood();
        snakeObject.draw();
        DrawText("Snake", offset -5, 20, 40, darkGreen);
        DrawText(TextFormat("%i", snakeObject.body.size()-3), offset-5, offset+cellSize*cellCount+10, 40, darkGreen);    
        EndDrawing();
        
    }
    CloseWindow();
    return 0;
}
