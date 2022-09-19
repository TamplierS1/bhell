#pragma once

template <class T>
class Vec2
{
public:
    Vec2(T x_ = 0, T y_ = 0)
        : x(x_)
        , y(y_)
    {
    }

    template <class U>
    constexpr bool operator==(const Vec2<U>& vec) const
    {
        return x == vec.x && y == vec.y;
    }

    template <class U>
    constexpr Vec2 operator*(const U& scale) const
    {
        return Vec2{x * static_cast<T>(scale), y * static_cast<T>(scale)};
    }

    template <class U>
    constexpr Vec2 operator+=(const Vec2<U>& vec)
    {
        x += vec.x;
        y += vec.y;

        return *this;
    }

    T x;
    T y;
};

template class Vec2<int>;

using Vec2i = Vec2<int>;