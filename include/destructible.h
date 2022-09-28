#pragma once

#include "actor.h"

class Destructible : public Actor
{
public:
    Destructible(const std::string& texture_name = "spaceship", Vec2i pos = {0, 0},
                 float speed = 0.0f, int damage = 0, int health = 0)
        : damage(damage)
        , health(health)
        , Actor(texture_name, pos, speed)
    {
    }

    int damage;
    int health;
};
