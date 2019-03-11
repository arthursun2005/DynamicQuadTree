//
//  vec2.h
//  DynamicQuadTree
//
//  Created by Arthur Sun on 3/10/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef vec2_h
#define vec2_h

struct vec2
{
    float x;
    float y;
    
    vec2() {}
    
    vec2(float x, float y) : x(x), y(y) {}
    
    vec2(float xy) : x(xy), y(xy) {}
    
    inline void set(float _x, float _y) {
        x = _x;
        y = _y;
    }
    
    inline void operator *= (float a)
    {
        x *= a;
        y *= a;
    }
    
    inline void operator += (const vec2& a)
    {
        x += a.x;
        y += a.y;
    }
    
    inline vec2 operator - () const
    {
        return vec2(-x, -y);
    }
    
    inline float magSq() const
    {
        return x * x + y * y;
    }
    
    inline void print() const
    {
        printf("%6.3f, %6.3f \n", x, y);
    }
};

inline float dot(const vec2& a, const vec2& b)
{
    return a.x * b.x + a.y * b.y;
}

inline vec2 min (const vec2& a, const vec2& b)
{
    return vec2(std::min(a.x, b.x), std::min(a.y, b.y));
}

inline vec2 max (const vec2& a, const vec2& b)
{
    return vec2(std::max(a.x, b.x), std::max(a.y, b.y));
}

inline vec2 operator + (const vec2& a, const vec2& b)
{
    return vec2(a.x + b.x, a.y + b.y);
}

inline vec2 operator * (float a, const vec2& b)
{
    return vec2(a * b.x, a * b.y);
}

inline vec2 operator / (const vec2& a, const vec2& b)
{
    return vec2(a.x / b.x, a.y / b.y);
}

inline vec2 operator / (float a, const vec2& b)
{
    return vec2(a / b.x, a / b.y);
}

inline vec2 operator - (const vec2& a, const vec2& b)
{
    return vec2(a.x - b.x, a.y - b.y);
}

inline vec2 operator * (const vec2& a, const vec2& b)
{
    return vec2(a.x * b.x, a.y * b.y);
}

inline vec2 rot(const vec2& a, const vec2& b)
{
    return vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

inline float invSqrt(float f) {
    long i = *(long*)&f;
    i = 0x5f3759df - (i >> 1);
    float f2 = f * 0.5f;
    f = *(float*)&i;
    f *= (1.5f - (f2 * f * f));
    return f;
}

inline float randFlt() {
    return rand() / (float) RAND_MAX;
}

inline float randFlt(float a, float b) {
    return (randFlt() * (b - a)) + a;
}

#endif /* vec2_h */
