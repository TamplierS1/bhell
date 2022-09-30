#include "font_renderer.h"

void FontRenderer::render_text(SDL_Renderer* renderer, const std::string& text,
                                       Vec2i pos, SDL_Color color)
{
    SDL_Rect dest = {pos.x, pos.y, size.x * text.size(), size.y};

    if (prev_text_rendered != text)
    {
        font_surface = TTF_RenderText_Solid(font.lock()->ttf_font, text.c_str(), color);
        font_texture = SDL_CreateTextureFromSurface(renderer, font_surface);
        prev_text_rendered = text;
    }

    if (SDL_RenderCopy(renderer, font_texture, nullptr, &dest) != 0)
    {
        printf("Failed to render a texture! SDL Error: %s\n", SDL_GetError());
    }
}

Vec2i FontRenderer::font_size() const
{
    return size;
}
