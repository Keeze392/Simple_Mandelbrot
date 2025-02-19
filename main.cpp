#include <raylib.h>

#include <complex>
#include <algorithm>
#include <vector>

// window size
const int width{1000};
const int height{1000};

// x and y for mandelbrot, can used for zoom in or move around
double minX{-2.0}, maxX{1.0};
double minY{-1.5}, maxY{1.5};

// settings for mandelbrot quality and zoom etc
unsigned int maxIterations{250};
const double zoomfactor{0.5};
double move{};

//current pixel and draw mandelbrot
int current_px{0};
int current_py{0};
const int tile_size{50};
bool isDrawing{false};
bool first_text{true};

RenderTexture2D buffer;

// just convert into number i want get.
std::vector<float> converter(float X, float Y){
    std::vector<float> result;
    result.insert(result.begin(), minX + (X / width) * (maxX - minX));
    result.insert(result.end() , minY + (Y / height) * (maxY - minY));
    return result;
}

void draw_mandelbrot(){
    for(int step{0}; step < 5000; step++){
        if(current_py >= height){
            isDrawing = false;
            current_px = 0;
            current_py = 0;
            return;
        }

        double x0 = minX + (current_px/(double)width) * (maxX - minX);
        double y0 = minY + (current_py/(double)height) * (maxY - minY);

        std::complex<double> z = 0;
        std::complex<double> c(x0, y0);
        int iterations{0};

        while(std::abs(z) <= 2.0 && iterations < maxIterations){
            z = z*z + c;
            iterations++;
        }

        Color pixelColor;
        if(iterations == maxIterations){
            pixelColor = BLACK;
        } else {
            int colorValue = (iterations * 255) / maxIterations;
            pixelColor = (Color){(unsigned char)colorValue, (unsigned char)colorValue, (unsigned char)colorValue, 255};
        }
        int flippedY = height - current_py -1;

        DrawPixel(current_px, flippedY, pixelColor);

        current_px++;
        if(current_px >= width){
            current_px = 0;
            current_py++;
        }
    }
}

// WASD to move without zoom
void move_around(){
    current_px = 0;
    current_py = 0;
    isDrawing = true;
    float moveX = 0.25 * (maxX - minX);
    float moveY = 0.25 * (maxY - minY);
    if(IsKeyDown(KEY_D)){
        minX += moveX;
        maxX += moveX;
    } else if(IsKeyDown(KEY_A)){
        minX -= moveX;
        maxX -= moveX;
    } else if(IsKeyDown(KEY_S)){
        minY += moveY;
        maxY += moveY;
    } else if(IsKeyDown(KEY_W)){
        minY -= moveY;
        maxY -= moveY;
    }
}

// it follow where mouse is when zoom in.
void follow_mouse(float x, float y){
    std::vector<float> posXY{converter(x, y)};
    double moveX = posXY[0];
    double moveY = posXY[1];
    double XX{(maxX - minX) / 2};
    double YY{(maxY - minY) / 2};
    
    minX = moveX - XX * zoomfactor;
    maxX = moveX + XX * zoomfactor;
    minY = moveY - YY * zoomfactor;
    maxY = moveY + YY * zoomfactor;
}

int main(){
    InitWindow(width, height, "Mandelbrot");

    buffer = LoadRenderTexture(width, height);
    ClearBackground(BLACK);
    isDrawing = true;

    // starting draw screen
    while(!WindowShouldClose()){
        BeginDrawing();

        if(isDrawing){
            BeginTextureMode(buffer);
            draw_mandelbrot();
            EndTextureMode();
        }

        DrawTexture(buffer.texture, 0, 0, WHITE);

        // reset to default zoom
        if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && !isDrawing){
            isDrawing = true;
            minX = -2.0;
            maxX = 1.0;
            minY = -1.5;
            maxY = 1.5;
            maxIterations = 100;
            first_text = true;
        }
        // zoom in by 25% each time
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !isDrawing){
            isDrawing = true;
            Vector2 mouse_pos{GetMousePosition()};
            follow_mouse(mouse_pos.x, mouse_pos.y);
            maxIterations += 25;
            first_text = false;
        }

        // move around
        if(GetKeyPressed() && !isDrawing){
            move_around();
            first_text = false;
        }
        if(first_text){
            DrawText("WASD - move around", 20, 20, 24, RAYWHITE);
            DrawText("Left click - Zoom in + follow the mouse", 20, 44, 24, RAYWHITE);
            DrawText("Right click - Back to default", 20, 68, 24, RAYWHITE);
        }
        EndDrawing();
    }

    UnloadRenderTexture(buffer);
    CloseWindow();
    
    return 0;
}