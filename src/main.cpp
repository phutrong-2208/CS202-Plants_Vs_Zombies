#include "raylib.h"
#include "Worlds/World.hpp"

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "Plants Vs Zombies");
    MaximizeWindow();
    SetTargetFPS(60);

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
        ClearBackground((Color){50, 60, 40, 255});
        world.draw();
        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
