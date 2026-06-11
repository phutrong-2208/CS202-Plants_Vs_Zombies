#include "raylib.h"

#include "Core/TextureManager.hpp"
#include "Worlds/World.hpp"

TextureManager file;

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    
    InitWindow(1200, 900, "Plants Vs Zombies");
    SetTargetFPS(60);

    file.LoadResource();

    int w = GetScreenWidth();
    int h = GetScreenHeight();
    World world(w, h);

    while (!WindowShouldClose()) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 m = GetMousePosition();
            world.onMouseClick((int)m.x, (int)m.y);
        }

        world.update(GetFrameTime());

        BeginDrawing();
        world.draw();
        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    file.UnloadResource();
    return 0;
}
