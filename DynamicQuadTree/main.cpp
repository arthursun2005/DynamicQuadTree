//
//  main.cpp
//  DynamicQuadTree
//
//  Created by Arthur Sun on 3/9/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#include <iostream>
#include <cmath>
#include "DynamicQuadTree.h"
#include "DynamicHashGrid.h"

std::vector<unsigned long> clocks;

struct particle
{
    particle() : w(0.0f) {}
    
    particle(const vec2& p) : p(p), w(0.0f) {}
    
    particle(float x, float y) : p(x, y), w(0.0f) {}
    
    vec2 p;
    float w;
    
    inline void print() const
    {
        printf("w: %6.3f, p: %6.3f, %6.3f \n", w, p.x, p.y);
    }
    
};

float k = 100.0f;
float h = 3.0f;
float h2 = h * h;

DynamicQuadTree<particle> qt(h);
DynamicHashGrid<particle> hg(h);

inline float weight(const vec2& d) {
    float u = 1.0f - d.magSq() / h2;
    return u * u * u;
}

inline void solve_weight(particle* a, particle* b)
{
    vec2 d = b->p - a->p;
    if(d.magSq() <= h2) {
        float w = weight(d);
        a->w += w;
        b->w += w;
    }
}

void simple_solve(particle* parts, int n) {
    for(int i = 0; i < n; ++i) {
        for(int j = i + 1; j < n; ++j) {
            particle* a = parts + i;
            particle* b = parts + j;
            solve_weight(a, b);
        }
    }
}

void solve_part(particle* a, particle* b) {
    if(a == b) return;
    solve_weight(a, b);
}

const int n = 1400;

inline float calc_ms(int i)
{
    return 1000.0f * (clocks[i + 1] - clocks[i]) / (float) CLOCKS_PER_SEC;
}

int main(int argc, const char * argv[]) {
    srand((unsigned int)time(0));
    particle dots[n];

    for(int i = 0; i < n; ++i) {
        dots[i].p = vec2(randFlt(-k * 0.5f, k * 0.5f), randFlt(-k * 0.5f, k * 0.5f));
    }
    
    particle dots1[n];
    particle dots2[n];
    memcpy(dots1, dots, sizeof(particle) * n);
    memcpy(dots2, dots, sizeof(particle) * n);
    
    for(int i = 0; i < n; ++i) {
        qt.insert_pointer(dots + i, dots[i].p);
        hg.insert_pointer(dots2 + i, dots2[i].p);
    }
    
    clocks.push_back(clock());
    
    qt.solve(solve_part);
    
    clocks.push_back(clock());
    
    hg.solve(solve_part);
    
    clocks.push_back(clock());
    
    simple_solve(dots1, n);
    
    clocks.push_back(clock());
    
    for(int i = 0; i < n; ++i) {
        float f = dots1[i].w - dots[i].w;
        printf("%.3f\n", f);
    }
    
    printf("\n");
    
    printf("DynamicQuadTree: %.5f ms\n", calc_ms(0));
    printf("DynamicHashGrid: %.5f ms\n", calc_ms(1));
    printf("O(n^2): %.5f ms\n", calc_ms(2));

    return 0;
}
