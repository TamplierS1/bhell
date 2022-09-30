#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>
#include <stdexcept>

#include "game.h"
#include "texture_manager.h"

Game::Game()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        throw std::runtime_error("SDL failed to initialize! SDL_Error: %s\n" +
                                 std::string(SDL_GetError()));
    }

    window = SDL_CreateWindow("bhell", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              screen_size.x, screen_size.y,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == nullptr)
    {
        throw std::runtime_error("SDL window failed to initialize! SDL_Error: %s\n" +
                                 std::string(SDL_GetError()));
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        throw std::runtime_error("SDL renderer failed to initialize! SDL_Error: %s\n" +
                                 std::string(SDL_GetError()));
    }
    SDL_RenderSetVSync(renderer, 1);

    int img_init_flags = IMG_INIT_PNG;
    if (!(IMG_Init(img_init_flags) & img_init_flags))
    {
        throw std::runtime_error("SDL_image failed to initialize! SDL_image error: %s\n" +
                                 std::string(IMG_GetError()));
    }

    if (TTF_Init() < 0)
    {
        throw std::runtime_error("SDL_ttf failed to initialize! SDL_ttf error: %s\n" +
                                 std::string(TTF_GetError()));
    }
    TexMan::get().load_font("assets/font.ttf", 18);

    TexMan::get().load_textures("assets", renderer);
}

Game::~Game()
{
    TexMan::get().free_assets();

    IMG_Quit();
    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::run()
{
    game_world.run(renderer, gameplay_viewport);
}