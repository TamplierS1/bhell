#pragma once

#include <SDL2/SDL.h>
#include <vector>

#include "actor.h"
#include "clock.h"

class World
{
public:
    World();

    void run(SDL_Renderer* renderer, SDL_Rect viewport);

private:
    void render_world(SDL_Renderer* renderer, SDL_Rect viewport);
    void render(SDL_Renderer* renderer, const Actor& actor);

    void handle_events();
    void handle_input(SDL_Rect viewport);

    std::vector<Actor> actors;
    Actor player;

    Clock clock;
    bool running = true;
};