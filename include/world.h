#pragma once

#include <SDL.h>
#include <vector>
#include <memory>

#include "destructible.h"
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
    void despawn_bullets();
    void move_bullets();

    void spawn_enemies();
    void despawn_enemies();
    void move_enemies();

    void check_collisions();

    std::vector<std::shared_ptr<Actor>> actors;
    std::vector<std::shared_ptr<Destructible>> bullets;
    Destructible player;

    Clock clock;
    bool running = true;
    SDL_Rect viewport;
};