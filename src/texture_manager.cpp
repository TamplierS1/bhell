#include <filesystem>
#include <SDL_image.h>

#include "texture_manager.h"

namespace fs = std::filesystem;

void TexMan::load_textures(const std::string& path, SDL_Renderer* renderer)
{
    if (fs::exists(path) && fs::is_directory(path))
    {
        for (const auto& entry : fs::directory_iterator(path))
        {
            if (entry.path().extension() == ".png")
            {
                SDL_Texture* new_texture = nullptr;
                SDL_Surface* loaded_surface =
                    IMG_Load(entry.path().generic_string().c_str());

                if (loaded_surface == nullptr)
                {
                    printf("Failed to load image at %s! SDL_image error: %s\n",
                           path.c_str(), IMG_GetError());
                    continue;
                }

                new_texture = SDL_CreateTextureFromSurface(renderer, loaded_surface);
                if (new_texture == nullptr)
                {
                    printf("Failed to create texture from %s! SDL Error: %s\n",
                           path.c_str(), SDL_GetError());
                    SDL_FreeSurface(loaded_surface);
                    continue;
                }

                textures[entry.path().stem().generic_string()] =
                    std::make_shared<Texture>(
                        new_texture, Vec2i{loaded_surface->w, loaded_surface->h});

                SDL_FreeSurface(loaded_surface);
            }
        }
    }
}

void TexMan::load_font(const std::string& path, int ptsize)
{
    ttf_font = std::make_shared<Font>(TTF_OpenFont(path.c_str(), ptsize));
    if (ttf_font.get() == nullptr)
    {
        printf("Failed to load a font from %s! SDL_TTF error: %s\n", path.c_str(),
               TTF_GetError());
    }
}

std::optional<Texture*> TexMan::texture(const std::string& texture_name)
{
    if (!textures.contains(texture_name))
    {
        return std::nullopt;
    }

    return textures[texture_name].get();
}

std::shared_ptr<Font> TexMan::font()
{
    return ttf_font;
}

void TexMan::free_assets()
{
    // I manually reset them because they need to be freed
    // before `TTF_Quit()` is called.
    for (auto& texture : textures)
    {
        texture.second.reset();
    }
    ttf_font.reset();
}
