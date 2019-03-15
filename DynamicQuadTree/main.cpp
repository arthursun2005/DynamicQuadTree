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

float k = 1000.0f;
float h = 3.0f;
float h2 = h * h;

DynamicQuadTree<particle> qt(h);
DynamicHashGrid<particle> hg(h);

inline float weight(const vec2& d) {
    float u = 1.0f - d.magSq() / h2;
    return u * u * u;
}

int u1 = 0;
int u2 = 0;

void simple_solve(particle* parts, int n) {
    for(int i = 0; i < n; ++i) {
        for(int j = i + 1; j < n; ++j) {
            particle* a = parts + i;
            particle* b = parts + j;
            vec2 d = b->p - a->p;
            if(d.magSq() <= h2) {
                float w = weight(d);
                a->w += w;
                b->w += w;
            }
        }
    }
}



void solve_part1(particle* a, particle* b) {
    if(a == b) return;
    vec2 d = b->p - a->p;
    if(d.magSq() <= h2) {
        float w = weight(d);
        a->w += w;
        b->w += w;
        ++u1;
    }
}

void solve_part2(particle* a, particle* b) {
    if(a == b) return;
    vec2 d = b->p - a->p;
    if(d.magSq() <= h2) {
        float w = weight(d);
        a->w += w;
        b->w += w;
        ++u2;
    }
}

const int n = 100000;

inline float calc_ms(int i)
{
    return 1000.0f * (clocks[i + 1] - clocks[i]) / (float) CLOCKS_PER_SEC;
}

int main(int argc, const char * argv[]) {
    srand((unsigned int)time(0));
    particle *dots;
    particle *dots1;
    particle *dots2;
    
    dots = (particle*)malloc(sizeof(particle) * n);
    dots1 = (particle*)malloc(sizeof(particle) * n);
    dots2 = (particle*)malloc(sizeof(particle) * n);
    
    dots[0].p.set(0.0f, 0.0f);
    dots[1].p.set(0.0f, 0.5f);
    
    vec2 sum;
    for(int i = 0; i < n; ++i) {
        dots[i].p = vec2(randFlt(-k * 0.5f, k * 0.5f), randFlt(-k * 0.5f, k * 0.5f));
        sum += dots[i].p;
    }
    
    sum *= 1.0f/(float)n;
    sum.print();
    
    memcpy(dots1, dots, sizeof(particle) * n);
    memcpy(dots2, dots, sizeof(particle) * n);
    
    for(int i = 0; i < n; ++i) {
        qt.insert_pointer(dots + i, dots[i].p);
        hg.insert_pointer(dots2 + i, dots2[i].p);
    }
    
    clocks.push_back(clock());
    
    qt.solve(solve_part1);
    
    clocks.push_back(clock());
    
    hg.solve(solve_part2);
    
    clocks.push_back(clock());
    
    if(n < 3000)
        simple_solve(dots1, n);
    
    clocks.push_back(clock());
    
    if(n < 2000) {
        for(int i = 0; i < n; ++i) {
            float f = dots1[i].w - dots[i].w;
            if(f != 0.0f)
                printf("%.5f\n", f);
        }
        printf("\n");
    }
    
    printf("DynamicQuadTree: %.5f ms\n", calc_ms(0));
    printf("DynamicHashGrid: %.5f ms\n", calc_ms(1));
    printf("%d, %d collisions \n", u1, u2);
    
    free(dots);
    free(dots1);
    free(dots2);
    
    return 0;
}
