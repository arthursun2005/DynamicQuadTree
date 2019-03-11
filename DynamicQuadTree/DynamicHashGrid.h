//
//  DynamicHashGrid.h
//  DynamicQuadTree
//
//  Created by Arthur Sun on 3/10/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef DynamicHashGrid_h
#define DynamicHashGrid_h

#include <vector>
#include "vec2.h"

struct _gridHasher
{
    inline size_t operator () (int x, int y) const
    {
        return (x << 8) ^ (y << 0);
    }
};

template <class T, int N = 1048576, class _Hasher = _gridHasher>
class DynamicHashGrid
{
    
protected:
    
    struct Node
    {
        Node(T* data, const vec2& p) : data(data), p(p) {}
        T* data;
        vec2 p;
    };
    
    std::vector<Node> data;
    std::vector<int>* grid[N];
    _Hasher hasher;
    float h;
    
public:
    
    bool null;
    
    DynamicHashGrid(float h) : h(h), null(true) {
        memset(grid, 0, N * sizeof(std::vector<T*>*));
        data.reserve(1024);
    }
    
    ~DynamicHashGrid() {
        for(int i = 0; i < N; ++i) {
            if(grid[i]) delete grid[i];
        }
    }
    
    inline void clear() {
        data.clear();
    }
    
    template <class _Solver>
    void solve(_Solver solver) {
        static const int nk = 16;
        static const int k[nk] = {
            0, -1,
            1, -1,
            -1, 0,
            0, 0,
            1, 0,
            -1, 1,
            0, 1,
            1, 1
        };
        
        size_t hash;
        
        for(Node& x : data) {
            int ix = x.p.x/h;
            int iy = x.p.y/h;
            for(int i = 0; i < nk; i += 2) {
                hash = hasher(ix + k[i], iy + k[i + 1]);
                std::vector<int>* cell = grid[hash%N];
                if(cell == 0) continue;
                for(int& j : *cell) {
                    vec2& e = data[j].p;
                    if((e.x <= x.p.x && e.y <= x.p.y)) continue;
                    if((e - x.p).magSq() <= h * h)
                        solver(x.data, data[j].data);
                }
            }
        }
        
    }
    
    void insert_pointer(T* ptr, const vec2& p)
    {
        size_t hash = hasher(p.x/h, p.y/h);
        std::vector<int>*& k = grid[hash%N];
        if(k == 0)
            k = new std::vector<int>;
        
        k->push_back((int)data.size());
        data.push_back(Node(ptr, p));
    }
};

#endif /* DynamicHashGrid_h */
