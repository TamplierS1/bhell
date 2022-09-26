#include "world.h"
#include "texture_manager.h"

World::World()
    : player(Actor{"spaceship", Vec2i{10, 10}, 3.0f})
    , viewport(SDL_Rect{})
{
}

void World::run(SDL_Renderer* renderer, SDL_Rect game_viewport)
{
    viewport = game_viewport;
    while (running)
    {
        clock.tick();

        handle_events();
        handle_input();

        spawn_bullets();
        move_bullets();
        despawn_bullets();

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

    Vec2i player_texture_size = TexMan::get().texture("spaceship").value()->size;
    if (player.pos.x < 0)
    {
        player.pos.x = 0;
    }
    else if (player.pos.x > viewport.w - player_texture_size.x)
    {
        player.pos.x = viewport.w - player_texture_size.x;
    }

    if (player.pos.y < 0)
    {
        player.pos.y = 0;
    }
    else if (player.pos.y > viewport.h - player_texture_size.y)
    {
        player.pos.y = viewport.h - player_texture_size.y;
    }
}

void World::spawn_bullets()
{
    static float time_threshold = 0.f;
    time_threshold += clock.get_delta();
    
    if (time_threshold >= 50.f)
    {
        time_threshold = 0.f;
        spawn_bullet(player);
    }
}

void World::spawn_bullet(const Actor& shooter)
{
    Vec2i spawn_pos = {shooter.pos.x, shooter.pos.y - 2};
    bullets.emplace_back("bullet", spawn_pos, 5.f);
}

void World::despawn_bullets()
{
    std::vector<int> dead_bullets_idx;

    for (int i = 0; i < bullets.size();i++)
    {
        if (is_out_of_bounds(bullets[i]))
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

bool World::is_out_of_bounds(const Actor& actor)
{
    Vec2i texture_size = TexMan::get().texture(actor.texture_name).value()->size;

    // We'll wait until the whole texture is out_of_bounds
    return actor.pos.x + texture_size.x < 0 ||
        actor.pos.x > viewport.w ||
        actor.pos.y + texture_size.y < 0 ||
        actor.pos.y > viewport.h;
}
