#pragma once

#include <SDL.h>
#include <vector>
#include <memory>

#include "destructible.h"
#include "clock.h"
#include "font_renderer.h"

enum class GameState
{
    StartMenu,
    Playing,
    GameOver
};

class World
{
public:
    World();

    void run(SDL_Renderer* renderer, SDL_Rect viewport);

private:
    void render_world(SDL_Renderer* renderer);
    void render(SDL_Renderer* renderer, const Actor& actor);
    void render_gameover_menu(SDL_Renderer* renderer);

    void handle_events();
    void handle_input();

    void spawn_bullets();
    void despawn_bullets();
    void move_bullets();

    void spawn_enemies();
    void despawn_enemies();
    void move_enemies();

    void check_collisions();

    void restart_game();

    std::vector<std::shared_ptr<Actor>> actors;
    std::vector<std::shared_ptr<Destructible>> bullets;
    Destructible player;

    Clock clock;
    GameState state;
    bool running = true;
    SDL_Rect viewport;

    std::unique_ptr<FontRenderer> font_renderer;
};