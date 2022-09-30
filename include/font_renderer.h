#pragma once

#include "texture_manager.h"

class FontRenderer
{
public:
    FontRenderer(const std::shared_ptr<Font>& font, Vec2i size = {100, 100})
        : font{font}
        , size{size}
    {
    }
    ~FontRenderer()
    {
        SDL_FreeSurface(font_surface);
        SDL_DestroyTexture(font_texture);
    }

    void render_text(SDL_Renderer* renderer, const std::string& text, Vec2i pos,
                             SDL_Color color = {255, 255, 255, 255});

    // TODO: add 'get_' to these getters.
    Vec2i font_size() const;

private:
    std::weak_ptr<Font> font;
    Vec2i size;

    SDL_Surface* font_surface = nullptr;
    SDL_Texture* font_texture = nullptr;

    std::string prev_text_rendered;
};