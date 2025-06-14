#include <raylib.h>

#include <complex>
#include <vector>
#include <thread>

// window size
const int width{1000};
const int height{1000};

// x and y for mandelbrot, can used for zoom in or move around
double minX{-2.0}, maxX{1.0};
double minY{-1.5}, maxY{1.5};

// settings for mandelbrot quality and zoom etc
unsigned int maxIterations{250};
const double zoomfactor{0.5};

//current pixel and draw mandelbrot
int current_px{0};
int current_py{0};
const int tile_size{50};
bool isDrawing{true};
bool first_text{true};
bool needs_update{true};

Texture2D mandelbrot_texture;
std::vector<Color> pixelBuffer(width * height);
RenderTexture2D buffer;

std::complex<double> pixel_to_complex(int x, int y){
  double real = minX + (x / (double)width) * (maxX - minX);
  double imag = minY + (y / (double)height) * (maxY - minY);
  return std::complex<double>(real, imag);
}

Color mandelbrot_color(int x, int y){
  std::complex<double> c = pixel_to_complex(x, y);
  std::complex<double> z = 0;

  int iterations = 0;

  while(std::abs(z) <= 2.0 && iterations < maxIterations){
    z = z * z + c;
    ++iterations;
  }

  if(iterations == maxIterations) return BLACK;

  int color_val = (iterations * 255) / maxIterations;
  return Color{(unsigned char)color_val, (unsigned char)color_val, (unsigned char)color_val, 255};
}

void compute_mandelbrot(){
  const int thread_count = std::thread::hardware_concurrency();
  std::vector<std::thread> threads(thread_count);

  auto worker = [&](int thread_ID){
    for(int y{thread_ID}; y < height; y += thread_count){
      for(int x{0}; x < width; x++){
        int index = (height - y - 1) * width + x;
        pixelBuffer[index] = mandelbrot_color(x, y);
      }
    }
  };

  for(int i{0}; i < thread_count; i++){
    threads[i] = std::thread(worker, i);
  }

  for(auto& t : threads){
    t.join();
  }

  needs_update = true;
  isDrawing = false;
}

// WASD to move without zoom
void move_around(){
    float moveX = 0.25 * (maxX - minX);
    float moveY = 0.25 * (maxY - minY);

    if(IsKeyDown(KEY_D)){
        minX += moveX;
        maxX += moveX;
    } else if(IsKeyDown(KEY_A)){
        minX -= moveX;
        maxX -= moveX;
    } else if(IsKeyDown(KEY_S)){
        minY -= moveY;
        maxY -= moveY;
    } else if(IsKeyDown(KEY_W)){
        minY += moveY;
        maxY += moveY;
    }

    isDrawing = true;
}

// it follow where mouse is when zoom in.
void follow_mouse(Vector2 mouse_pos){
    double moveX = minX + (mouse_pos.x / width) * (maxX - minX);
    double moveY = minY + ((height - mouse_pos.y) / height) * (maxY - minY);
    double XX{(maxX - minX) * zoomfactor * 0.5};
    double YY{(maxY - minY) * zoomfactor * 0.5};
    
    minX = moveX - XX;
    maxX = moveX + XX;
    minY = moveY - YY;
    maxY = moveY + YY;
}

void reset_view(){
  minX = -2.0; maxX = 1.0;
  minY = -1.5; maxY = 1.5;
  maxIterations = 250;
  isDrawing = true;
  first_text = true;
}

int main(){
    InitWindow(width, height, "Mandelbrot");
    SetTargetFPS(60);

    Image image = GenImageColor(width, height, BLACK);
    mandelbrot_texture = LoadTextureFromImage(image);
    UnloadImage(image);

    compute_mandelbrot();
    
    while(!WindowShouldClose()){
      if(!isDrawing){
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
          follow_mouse(GetMousePosition());
          maxIterations = 100 + static_cast<int>(log2(3.0 / (maxX - minX)) * 50);
          isDrawing = true;
          first_text = false;
        }

        if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
          reset_view();
        }

        if(IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D)){
          move_around();
          first_text = false;
        }
      }

      if(isDrawing){
        compute_mandelbrot();
      }

      if(needs_update){
        UpdateTexture(mandelbrot_texture, pixelBuffer.data());
        needs_update = false;
      }

      BeginDrawing();
      ClearBackground(BLACK);
      DrawTexture(mandelbrot_texture, 0, 0, WHITE);

      if(first_text){
        DrawText("WASD - move", 20, 20, 24, RAYWHITE);
        DrawText("Left click - Zoom in at mouse", 20, 48, 24, RAYWHITE);
        DrawText("Right click - Reset view", 20, 76, 24, RAYWHITE);
      }

      EndDrawing();
    }

    UnloadTexture(mandelbrot_texture);
    CloseWindow();
    return 0;
}
