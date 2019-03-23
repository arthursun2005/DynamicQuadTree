//
//  AABB.h
//  DynamicQuadTree
//
//  Created by Arthur Sun on 3/17/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef AABB_h
#define AABB_h

#include <float.h>
#include "vec2.h"

struct AABB
{
    vec2 lowerBound;
    vec2 upperBound;
    
    AABB() {}
    
    AABB(const vec2& p) : lowerBound(p), upperBound(p) {}
    
    AABB(const vec2& a, const vec2& b) : lowerBound(a), upperBound(b) {}
    
    void set(const vec2& p) {
        lowerBound = p;
        upperBound = p;
    }
    
    void add(const vec2& p) {
        lowerBound = min(lowerBound, p);
        upperBound = max(upperBound, p);
    }
    
    void add(const AABB& aabb) {
        lowerBound = min(lowerBound, aabb.lowerBound);
        upperBound = max(upperBound, aabb.upperBound);
    }
    
    inline vec2 center () const
    {
        return (lowerBound + upperBound) * 0.5f;
    }
    
    void extend(float x) {
        lowerBound += -x;
        upperBound += +x;
    }
    
    bool covers(const vec2& p) const
    {
        return p.x >= lowerBound.x && p.y >= lowerBound.y && p.x <= upperBound.x && p.y <= upperBound.y;
    }
    
    bool covers(const vec2& p, float r) const
    {
        return p.x >= lowerBound.x - r && p.y >= lowerBound.y - r && p.x <= upperBound.x + r && p.y <= upperBound.y + r;
    }
    
};

inline bool touches(const AABB& a, const AABB& b)
{
    vec2 d1 = b.lowerBound - a.upperBound;
    vec2 d2 = a.lowerBound - b.upperBound;
    return d1.x <= 0.0f && d1.y <= 0.0f && d2.x <= 0.0f && d2.y <= 0.0f;
}

inline bool touches(const AABB& a, const AABB& b, float r)
{
    vec2 d1 = b.lowerBound - a.upperBound;
    vec2 d2 = a.lowerBound - b.upperBound;
    return d1.x <= r && d1.y <= r && d2.x <= r && d2.y <= r;
}

float distSq(const AABB& a, const AABB& b)
{
    vec2 ub = max(0.0f, a.lowerBound - b.upperBound);
    vec2 lb = max(0.0f, b.lowerBound - a.upperBound);
    return (ub - lb).magSq();
}


float distSq(const vec2& p, const AABB& b)
{
    vec2 ub = max(0.0f, p - b.upperBound);
    vec2 lb = max(0.0f, b.lowerBound - p);
    return (ub - lb).magSq();
}

#endif /* AABB_h */
