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
#include "AABB.h"

struct _gridHasher
{
    inline size_t operator () (int x, int y) const
    {
        return ((x << 8) * 31) ^ y;
    }
};

template <class T, int N = 16777216, class _Hasher = _gridHasher>
class DynamicHashGrid
{
    
protected:
    
    struct Pptr
    {
        T* ptr;
        vec2 p;
    };
    
    struct Node
    {
        std::vector<Pptr> data;
        AABB aabb;
        
        inline void add(T* ptr, const vec2& _p) {
            data.push_back(Pptr{ptr, _p});
        }
        
        inline int count() const
        {
            return (int)data.size();
        }
    };
    
    std::vector<Node> data;
    int grid[N];
    _Hasher hasher;
    float h;
    
public:
    
    bool null;
    
    DynamicHashGrid(float h) : h(h), null(true) {
        for(int i = 0; i < N; ++i) {
            grid[i] = -1;
        }
        data.reserve(1024);
    }
    
    ~DynamicHashGrid() {
        for(int i = 0; i < N; ++i) {
        }
    }
    
    inline void clear() {
        data.clear();
        for(int i = 0; i < N; ++i) {
            grid[i] = -1;
        }
    }
    
    template <class _Solver>
    void solve(_Solver solver) {
        static const int nk = 12;
        static const int k[nk] = {
            -1, 0,
            0, 0,
            1, 0,
            -1, 1,
            0, 1,
            1, 1
        };
        
        size_t hash;
        
        for(Node& x : data) {
            int xs = x.count();
            for(int w = 0; w < xs; ++w) {
                vec2& p = x.data[w].p;
                int ix = p.x/h;
                int iy = p.y/h;
                for(int i = 0; i < nk; i += 2) {
                    hash = hasher(ix + k[i], iy + k[i + 1]);
                    int& cell = grid[hash%N];
                    if(cell == -1) continue;
                    if(!data[cell].aabb.covers(p, h)) continue;
                    int cs = data[cell].count();
                    for(int q = 0; q < cs; ++q) {
                        vec2& e = data[cell].data[q].p;
                        if(e.y > p.y || (e.y >= p.y && e.x < p.x))
                            solver(x.data[w].ptr, data[cell].data[q].ptr);
                    }
                }
            }
        }
    }
    
    void insert_pointer(T* ptr, const vec2& p)
    {
        size_t hash = hasher(p.x/h, p.y/h);
        
        int& k = grid[hash%N];
        
        if(k == -1) {
            Node n;
            k = (int)data.size();
            n.aabb.set(p);
            n.add(ptr, p);
            data.push_back(n);
        }else{
            Node& n = data[k];
            n.aabb.add(p);
            n.add(ptr, p);
        }
    }
};

#endif /* DynamicHashGrid_h */
