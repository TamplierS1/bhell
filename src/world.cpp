#include <random>

#include "world.h"
#include "texture_manager.h"

World::World()
    : player(Actor{"player_spaceship", Vec2i{10, 10}, 5.0f})
    , viewport(SDL_Rect{})
{
    srand(time(NULL));
}

void World::run(SDL_Renderer* renderer, SDL_Rect game_viewport)
{
    viewport = game_viewport;
    while (running)
    {
        clock.tick();

        handle_events();
        handle_input();

        spawn_enemies();
        spawn_bullets();

        move_enemies();
        move_bullets();

        despawn_bullets();
        despawn_enemies();

        render_world(renderer);
    }
}

void World::render_world(SDL_Renderer* renderer)
{
    SDL_RenderClear(renderer);
    SDL_RenderSetViewport(renderer, &viewport);

    for (const auto& actor : actors)
    {
        render(renderer, actor);
    }
    render(renderer, player);

    for (const auto& bullet : bullets)
    {
        render(renderer, bullet);
    }

    SDL_RenderPresent(renderer);
}

void World::render(SDL_Renderer* renderer, const Actor& actor)
{
    auto texture = TexMan::get().texture(actor.texture_name);
    if (!texture.has_value())
    {
        printf("Failed to render an actor! Texture %s does not exist.\n",
               actor.texture_name.c_str());
        return;
    }

    SDL_Rect dest = {actor.pos.x, actor.pos.y, texture.value()->size.x,
                     texture.value()->size.y};

    if (SDL_RenderCopy(renderer, texture.value()->texture, nullptr, &dest) != 0)
    {
        printf("Failed to render a texture! SDL Error: %s\n", SDL_GetError());
    }
}

void World::handle_events()
{
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0)
    {
        if (e.type == SDL_QUIT)
        {
            running = false;
        }
    }
}

void World::handle_input()
{
    const Uint8* key_states = SDL_GetKeyboardState(NULL);
    float delta = clock.get_delta();
    if (key_states[SDL_SCANCODE_W])
    {
        player.move(Vec2i{0, -1}, delta);
    }
    if (key_states[SDL_SCANCODE_A])
    {
        player.move(Vec2i{-1, 0}, delta);
    }
    if (key_states[SDL_SCANCODE_D])
    {
        player.move(Vec2i{1, 0}, delta);
    }
    if (key_states[SDL_SCANCODE_S])
    {
        player.move(Vec2i{0, 1}, delta);
    }

    if (key_states[SDL_SCANCODE_ESCAPE])
    {
        running = false;
    }

    static Vec2i texture_size = TexMan::get().texture("player_spaceship").value()->size;
    if (player.pos.x < 0)
    {
        player.pos.x = 0;
    }
    else if (player.pos.x > viewport.w - texture_size.x)
    {
        player.pos.x = viewport.w - texture_size.x;
    }

    if (player.pos.y < 0)
    {
        player.pos.y = 0;
    }
    else if (player.pos.y > viewport.h - texture_size.y)
    {
        player.pos.y = viewport.h - texture_size.y;
    }
}

void World::spawn_bullets()
{
    static float time_threshold = 0.f;
    time_threshold += clock.get_delta();

    if (time_threshold >= 50.f)
    {
        time_threshold = 0.f;
        Vec2i spawn_pos = {player.pos.x, player.pos.y - 2};
        bullets.emplace_back("bullet", spawn_pos, 5.f);
    }
}

void World::despawn_bullets()
{
    std::vector<int> dead_bullets_idx;

    static Vec2i texture_size = TexMan::get().texture("bullet").value()->size;
    for (int i = 0; i < bullets.size(); i++)
    {
        if (bullets[i].pos.x + texture_size.x < 0 || bullets[i].pos.x > viewport.w ||
            bullets[i].pos.y + texture_size.y < 0 || bullets[i].pos.y > viewport.h)
        {
            dead_bullets_idx.emplace_back(i);
        }
    }

    for (const auto idx : dead_bullets_idx)
    {
        bullets.erase(bullets.begin() + idx);
    }
}

void World::move_bullets()
{
    for (auto& bullet : bullets)
    {
        bullet.move(Vec2i{0, -1}, clock.get_delta());
    }
}

void World::spawn_enemies()
{
    static float time_threshold = 0.f;
    time_threshold += clock.get_delta();

    static Vec2i texture_size = TexMan::get().texture("enemy_spaceship").value()->size;
    if (time_threshold >= 300.f)
    {
        time_threshold = 0.f;
        Vec2i pos{rand() % (viewport.w - texture_size.x - 10) + texture_size.x,
                  0 - texture_size.y};
        actors.emplace_back("enemy_spaceship", pos, 3.f);
    }
}

void World::despawn_enemies()
{
    std::vector<int> dead_enemies_idx;

    static Vec2i texture_size = TexMan::get().texture("enemy_spaceship").value()->size;
    for (int i = 0; i < actors.size(); i++)
    {
        if (actors[i].pos.y > viewport.h)
        {
            dead_enemies_idx.emplace_back(i);
        }
    }

    for (const auto idx : dead_enemies_idx)
    {
        actors.erase(actors.begin() + idx);
    }
}

void World::move_enemies()
{
    for (auto& enemy : actors)
    {
        enemy.move(Vec2i{0, 1}, clock.get_delta());
    }
}
