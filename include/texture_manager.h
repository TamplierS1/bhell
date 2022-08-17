#pragma once

#include <SDL2/SDL.h>
#include <memory>
#include <unordered_map>
#include <optional>

#include "vec2.h"

class Texture
{
public:
    Texture(SDL_Texture* texture_, Vec2i size_)
        : texture(texture_)
        , size(size_)
    {
    }

    ~Texture()
    {
        SDL_DestroyTexture(texture);
    }

    SDL_Texture* texture;
    Vec2i size;
};

class TexMan
{
public:
    TexMan(const TexMan&) = delete;
    TexMan(TexMan&&) = delete;

    static TexMan& get()
    {
        static TexMan instance;
        return instance;
    }

    void load_textures(const std::string& path, SDL_Renderer* renderer);
    std::optional<Texture*> texture(const std::string& texture_name);

private:
    TexMan() = default;

    std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
};