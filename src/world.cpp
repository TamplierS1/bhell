#include "world.h"
#include "texture_manager.h"

World::World()
    : player(Actor{"spaceship", Vec2i{10, 10}, 3.0f})
{
}

void World::run(SDL_Renderer* renderer, SDL_Rect viewport)
{
    while (running)
    {
        clock.tick();

        handle_events();
        handle_input(viewport);

        render_world(renderer, viewport);
    }
}

void World::render_world(SDL_Renderer* renderer, SDL_Rect viewport)
{
    SDL_RenderClear(renderer);

    SDL_RenderSetViewport(renderer, &viewport);

    for (const auto& actor : actors)
    {
        render(renderer, actor);
    }

    render(renderer, player);

    SDL_RenderPresent(renderer);
}

void World::render(SDL_Renderer* renderer, const Actor& actor)
{
    auto texture = TexMan::get().texture(actor.texture_name);
    if (!texture.has_value())
    {
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

void World::handle_input(SDL_Rect viewport)
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