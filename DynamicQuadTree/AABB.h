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
    
    bool covers(const vec2& p, const vec2& t) const
    {
        return p.x >= lowerBound.x - t.x && p.y >= lowerBound.y - t.y && p.x <= upperBound.x + t.x && p.y <= upperBound.y + t.y;
    }
    
    bool touches(const AABB& k) const
    {
        vec2 d1 = k.lowerBound - upperBound;
        vec2 d2 = lowerBound - k.upperBound;
        
        return d1.x <= 0.0f && d1.y <= 0.0f && d2.x <= 0.0f && d2.y <= 0.0f;
    }
    
    bool touches(const AABB& k, float r) const
    {
        vec2 d1 = k.lowerBound - upperBound - r;
        vec2 d2 = lowerBound - k.upperBound - r;
        
        return d1.x <= 0.0f && d1.y <= 0.0f && d2.x <= 0.0f && d2.y <= 0.0f;
    }
    
};

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
