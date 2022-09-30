#include <random>

#include "world.h"
#include "texture_manager.h"

World::World()
    : player(Destructible{"player_spaceship", Vec2i{10, 10}, 5.0f, 30, 100})
    , state(GameState::Playing)
    , viewport(SDL_Rect{})
{
    srand(time(NULL));
}

void World::run(SDL_Renderer* renderer, SDL_Rect game_viewport)
{
    font_renderer = std::make_unique<FontRenderer>(TexMan::get().font(), Vec2i{25, 25});

    viewport = game_viewport;
    while (running)
    {
        clock.tick();

        handle_events();
        handle_input();

        switch (state)
        {
            case GameState::Playing:

                spawn_enemies();
                spawn_bullets();

                move_enemies();
                move_bullets();

                check_collisions();

                if (player.health <= 0)
                    state = GameState::GameOver;

                despawn_bullets();
                despawn_enemies();

                render_world(renderer);
                break;
            case GameState::GameOver:
                render_gameover_menu(renderer);
                break;
            default:
                break;
        }
    }
}

void World::render_world(SDL_Renderer* renderer)
{
    SDL_RenderClear(renderer);
    SDL_RenderSetViewport(renderer, &viewport);
    for (const auto& actor : actors)
    {
        render(renderer, *actor);
    }
    render(renderer, player);

    for (const auto& bullet : bullets)
    {
        render(renderer, *bullet);
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

void World::render_gameover_menu(SDL_Renderer* renderer)
{
    SDL_RenderClear(renderer);
    SDL_RenderSetViewport(renderer, &viewport);

    std::string msg = "GAME OVER";
    Vec2i font_size = font_renderer->font_size();
    Vec2i pos = Vec2i{static_cast<int>((viewport.w - (font_size.x * msg.size()))) / 2,
                      (viewport.h - font_size.y) / 2};
    font_renderer->render_text(renderer, msg, pos);

    msg = "PRESS R TO RESTART";
    pos = Vec2i{static_cast<int>((viewport.w - (font_size.x * msg.size()))) / 2,
                (viewport.h - font_size.y) / 2 + font_size.y * 2};
    font_renderer->render_text(renderer, msg, pos);

    SDL_RenderPresent(renderer);
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

    if (state == GameState::GameOver)
    {
        if (key_states[SDL_SCANCODE_R])
        {
            restart_game();
        }
    }
    else if (state == GameState::Playing)
    {
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

        static Vec2i texture_size =
            TexMan::get().texture("player_spaceship").value()->size;
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
}

void World::spawn_bullets()
{
    // Player
    {
        static float time_threshold = 0.f;
        time_threshold += clock.get_delta();

        if (time_threshold >= 50.f)
        {
            time_threshold = 0.f;

            // Player' bullets
            Vec2i spawn_pos = {player.pos.x, player.pos.y - 2};
            bullets.emplace_back(std::make_shared<Destructible>(
                "player_bullet", spawn_pos, 5.f, player.damage, 10));
        }
    }

    // Enemies
    {
        static float time_threshold = 0.f;
        time_threshold += clock.get_delta();

        if (time_threshold >= 150.f)
        {
            time_threshold = 0.f;

            Vec2i texture_size = TexMan::get().texture("bullet").value()->size;
            // Enemies' bullets
            for (auto& enemy : actors)
            {
                Destructible* enemy_ptr = dynamic_cast<Destructible*>(enemy.get());
                if (enemy_ptr == nullptr)
                    continue;

                Vec2i spawn_pos = {enemy_ptr->pos.x,
                                   enemy_ptr->pos.y + texture_size.y + 2};
                bullets.emplace_back(std::make_shared<Destructible>(
                    "bullet", spawn_pos, -5.f, enemy_ptr->damage, 10));
            }
        }
    }
}

void World::despawn_bullets()
{
    std::vector<int> dead_bullets_idx;

    static Vec2i texture_size = TexMan::get().texture("bullet").value()->size;
    for (int i = 0; i < bullets.size(); i++)
    {
        if (bullets[i]->pos.x + texture_size.x < 0 || bullets[i]->pos.x > viewport.w ||
            bullets[i]->pos.y + texture_size.y < 0 || bullets[i]->pos.y > viewport.h ||
            bullets[i]->health <= 0)
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
        bullet->move(Vec2i{0, -1}, clock.get_delta());
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
        actors.emplace_back(
            std::make_shared<Destructible>("enemy_spaceship", pos, 3.f, 20, 10));
    }
}

void World::despawn_enemies()
{
    std::vector<int> dead_enemies_idx;

    static Vec2i texture_size = TexMan::get().texture("enemy_spaceship").value()->size;
    for (int i = 0; i < actors.size(); i++)
    {
        if (actors[i]->pos.y > viewport.h)
        {
            dead_enemies_idx.emplace_back(i);
        }

        auto enemy_ptr = dynamic_cast<Destructible*>(actors[i].get());
        if (enemy_ptr != nullptr && enemy_ptr->health <= 0)
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
        enemy->move(Vec2i{0, 1}, clock.get_delta());
    }
}

bool rect_rect_collision(SDL_Rect rect1, SDL_Rect rect2)
{
    return rect1.x < rect2.x + rect2.w && rect1.x + rect1.w > rect2.x &&
           rect1.y < rect2.y + rect2.h && rect1.h + rect1.y > rect2.y;
}

void World::check_collisions()
{
    static Vec2i bullet_tex_size = TexMan::get().texture("bullet").value()->size;
    static Vec2i enemy_tex_size = TexMan::get().texture("enemy_spaceship").value()->size;
    static Vec2i player_tex_size =
        TexMan::get().texture("player_spaceship").value()->size;
    for (auto& bullet : bullets)
    {
        // Player bullets with enemies
        if (bullet->texture_name == "player_bullet")
        {
            for (auto& enemy : actors)
            {
                auto enemy_ptr = dynamic_cast<Destructible*>(enemy.get());
                if (enemy_ptr == nullptr)
                    continue;

                if (rect_rect_collision(SDL_Rect{bullet->pos.x, bullet->pos.y,
                                                 bullet_tex_size.x, bullet_tex_size.y},
                                        SDL_Rect{enemy_ptr->pos.x, enemy_ptr->pos.y,
                                                 enemy_tex_size.x, enemy_tex_size.y}))
                {
                    enemy_ptr->health -= bullet->damage;
                    bullet->health = 0;
                }
            }
        }
        // Enemy bullets with player
        else if (bullet->texture_name == "bullet")
        {
            if (rect_rect_collision(SDL_Rect{bullet->pos.x, bullet->pos.y,
                                             bullet_tex_size.x, bullet_tex_size.y},
                                    SDL_Rect{player.pos.x, player.pos.y,
                                             player_tex_size.x, player_tex_size.y}))
            {
                player.health -= bullet->damage;
                bullet->health = 0;
            }
        }
    }
}

void World::restart_game()
{
    bullets.clear();
    actors.clear();
    player.health = 100;

    state = GameState::Playing;
}
