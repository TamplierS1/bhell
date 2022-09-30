#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

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

class Font
{
public:
    Font(TTF_Font* font)
        : ttf_font(font)
    {
    }

    ~Font()
    {
        TTF_CloseFont(ttf_font);
    }

    TTF_Font* ttf_font;
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
    void load_font(const std::string& path, int ptsize = 18);

    std::optional<Texture*> texture(const std::string& texture_name);
    std::shared_ptr<Font> font();

    void free_assets();

private:
    TexMan() = default;

    std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
    std::shared_ptr<Font> ttf_font;
};