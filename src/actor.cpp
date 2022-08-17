#include "actor.h"
#include "vec2.h"

void Actor::move(Vec2i direction, float delta)
{
    pos += direction * speed * delta;
}