#pragma once

#include <string>

#include "vec2.h"

class Actor
{
public:
    Actor(const std::string& texture_name = "spaceship", Vec2i pos = {0, 0},
          float speed = 0.0f)
        : pos(pos)
        , speed(speed)
        , texture_name(texture_name)
    {
    }

    void move(Vec2i direction, float delta);

    Vec2i pos;
    float speed;
    std::string texture_name;
};