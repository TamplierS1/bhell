#pragma once

#include <SDL.h>
#include <vector>

#include "actor.h"
#include "clock.h"

class World
{
public:
    World();

    void run(SDL_Renderer* renderer, SDL_Rect viewport);

private:
    void render_world(SDL_Renderer* renderer);
    void render(SDL_Renderer* renderer, const Actor& actor);

    void handle_events();
    void handle_input();

    void spawn_bullets();
    void spawn_bullet(const Actor& shooter);
    void despawn_bullets();
    void move_bullets();

    bool is_out_of_bounds(const Actor& actor);

    std::vector<Actor> actors;
    std::vector<Actor> bullets;
    Actor player;

    Clock clock;
    bool running = true;
    SDL_Rect viewport;
};