#pragma once

#include <SDL.h>

#include "vec2.h"
#include "world.h"

class Game
{
public:
    Game();
    ~Game();

    void run();

private:
    const Vec2i screen_size = {1920, 1080};
    const SDL_Rect gameplay_viewport = {screen_size.x / 2 - 600 / 2, 0, 600, 1080};

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    World game_world;
};