#include <GameLoop.hpp>

int main() {
    GameLoop app;
    app.initGame();

    while (app.isRunning()) {
        app.runGame();
    }

    app.closeGame();
    return 0;
}
