#pragma once

#include <SDL.h>
#include <cstdint>

class Clock
{
public:
    Clock()
        : now(SDL_GetPerformanceCounter())
        , last(0)
    {
    }

    void tick()
    {
        last = now;
        now = SDL_GetPerformanceCounter();
    }

    float get_delta() const
    {
        return ((now - last) * 1000 /
                static_cast<double>(SDL_GetPerformanceFrequency())) /
               10;
    }

private:
    uint64_t now;
    uint64_t last;
};