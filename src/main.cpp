#include <iostream>

#include <glad/glad.h>
#include <game.h>

int main() {
    Game game(Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT);
    game.Run();
}
