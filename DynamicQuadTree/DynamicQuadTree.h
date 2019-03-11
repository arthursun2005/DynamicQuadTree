//
//  DynamicQuadTree.h
//  DynamicQuadTree
//
//  Created by Arthur Sun on 3/9/19.
//  Copyright © 2019 Arthur Sun. All rights reserved.
//

#ifndef DynamicQuadTree_h
#define DynamicQuadTree_h

#include <cstring>
#include <vector>
#include "vec2.h"

typedef int32_t qt_int;

struct AABB
{
    vec2 lowerBound;
    vec2 upperBound;
    
    AABB() {}
    
    AABB(const vec2& p) : lowerBound(p), upperBound(p) {}
    
    AABB(const vec2& a, const vec2& b) : lowerBound(a), upperBound(b) {}
    
    inline void set(const vec2& p) {
        lowerBound = p;
        upperBound = p;
    }
    
    inline void add(const vec2& p) {
        lowerBound = min(lowerBound, p);
        upperBound = max(upperBound, p);
    }
    
    inline void add(const AABB& aabb) {
        lowerBound = min(lowerBound, aabb.lowerBound);
        upperBound = max(upperBound, aabb.upperBound);
    }
};

template <class T>
class DynamicQuadTree
{
    
protected:
    
    struct Node
    {
        T** data;
        
        qt_int children[4];
        
        inline const qt_int& operator [] (int i) const { return children[i]; }
        inline qt_int& operator [] (int i) { return children[i]; }
        
        qt_int count;
        qt_int capacity;
        
        Node() {}
        
        Node(const Node& x) = delete;
        
        inline Node& operator = (const Node& x) = delete;
        
        inline void init() {
            capacity = 0;
            count = 0;
        }
        
        inline void free() {
            if(capacity > 0)
                ::free(data);
        }
        
        inline void grow() {
            if(capacity == 0) {
                capacity = 2;
                data = (T**)malloc(sizeof(T*) * capacity);
            }else{
                capacity <<= 1;
                data = (T**)realloc(data, sizeof(T*) * capacity);
            }
        }
        
        inline void add(T* ptr)
        {
            if(count >= capacity)
                grow();
            data[count++] = ptr;
        }
        
        inline T** begin()
        {
            return data;
        }
        
        inline T** end()
        {
            return data + count;
        }
        
        inline const T** begin() const
        {
            return data;
        }
        
        inline const T** end() const
        {
            return data + count;
        }
    };
    
    float h;
    float rootSize;
    
    Node* nodes;
    
    qt_int size;
    qt_int capacity;
    
    qt_int root;
    
    qt_int level;
    
    static const int initial_node_capacity = 1;
    
public:
    
    
    DynamicQuadTree(float h) : h(h) , capacity(256), root(0), size(1), rootSize(h), level(0) {
        nodes = (Node*)malloc(sizeof(Node) * capacity);
        for (qt_int i = 0; i < capacity; ++i) {
            nodes[i].init();
            nodes[i].children[0] = -1;
            nodes[i].children[1] = -1;
            nodes[i].children[2] = -1;
            nodes[i].children[3] = -1;
        }
        nodes[0].size = rootSize;
    }
    
    
    
    ~DynamicQuadTree() {
        for (qt_int i = 0; i < capacity; ++i)
            nodes[i].free();
        
        free(nodes);
    }
    
    
    
    qt_int alloc_node() {
        if(size >= capacity) {
            qt_int cap = capacity << 1;
            nodes = (Node*)realloc(nodes, sizeof(Node) * cap);
            for (qt_int i = capacity; i < cap; ++i) {
                nodes[i].init();
                nodes[i].children[0] = -1;
                nodes[i].children[1] = -1;
                nodes[i].children[2] = -1;
                nodes[i].children[3] = -1;
            }
            capacity = cap;
        }
        return size++;
    }
    
    
    
    void expand_once() {
        qt_int n0 = alloc_node();
        qt_int n1 = alloc_node();
        qt_int n2 = alloc_node();
        qt_int n3 = alloc_node();
        
        nodes[n0][3] = nodes[root][0];
        nodes[n1][2] = nodes[root][1];
        nodes[n2][1] = nodes[root][2];
        nodes[n3][0] = nodes[root][3];
        
        nodes[n0].size = rootSize;
        nodes[n1].size = rootSize;
        nodes[n2].size = rootSize;
        nodes[n3].size = rootSize;
        
        nodes[root][0] = n0;
        nodes[root][1] = n1;
        nodes[root][2] = n2;
        nodes[root][3] = n3;
        
        rootSize *= 2.0f;
        ++level;
    }
    
    
    
    void grow_to(const vec2& p)
    {
        float pSize = std::max(abs(p.x), abs(p.y)) * 2.0f;
        while(rootSize < pSize)
            expand_once();
    }
    
    
    
    template <class _Solver>
    void solve_node_complete(qt_int i, qt_int n, _Solver& solver, const vec2& k) {
        if(nodes[i].size >= nodes[n].size) {
            if(!nodes[n].m() && !nodes[i].m() && (nodes[i].world - nodes[n].world).magSq() <= h * h) {
                solver(*nodes[i].data, *nodes[n].data);
            }
            
            if(nodes[n].m() || nodes[i].m()) {
                for(T*& p0 : nodes[i]) {
                    for(T*& p1 : nodes[n]) {
                        solver(p0, p1);
                    }
                }
            }
            
        }
        
        float d = nodes[n].size * 0.25f;
        
        if(k.x <= h) {
            if(k.y <= h && nodes[n][0] != -1)
                solve_node_complete(i, nodes[n][0], solver, k + vec2(d, d));
            
            
            if(k.y >= -h && nodes[n][2] != -1)
                solve_node_complete(i, nodes[n][2], solver, k + vec2(d, -d));
        }
        
        if(k.x >= -h) {
            if(k.y <= h && nodes[n][1] != -1)
                solve_node_complete(i, nodes[n][1], solver, k + vec2(-d, d));
            
            
            if(k.y >= -h && nodes[n][3] != -1)
                solve_node_complete(i, nodes[n][3], solver, k + vec2(-d, -d));
        }
    }
    
    
    
    template <class _Solver>
    inline void solve_node(qt_int i, _Solver& solver) {
        if(nodes[i].data)
            solve_node_complete(i, nodes[i].p, solver, nodes[nodes[i].p].node_position);
        
        if(nodes[i][0] != -1)
            solve_node(nodes[i][0], solver);
        
        if(nodes[i][1] != -1)
            solve_node(nodes[i][1], solver);
        
        if(nodes[i][2] != -1)
            solve_node(nodes[i][2], solver);
        
        if(nodes[i][3] != -1)
            solve_node(nodes[i][3], solver);
        
    }
    
    
    template <class _Solver>
    void solve(_Solver solver) {
        if(nodes[root].data == 0) return;
        solve_node(root, solver);
    }
    
    
    
    inline void alloc_child(qt_int i, qt_int c) {
        if(nodes[i][c] == -1)
            nodes[i][c] = alloc_node();
    }
    
    
    
    void insert_pointer(T* ptr, const vec2& p) {
        grow_to(p);
        qt_int i = root;
        float d = rootSize * 0.5f;
        vec2 n, b;
        qt_int w = i;
        for(int _n = 0; _n < level; ++_n) {
            d *= 0.5f;
            
            b = p + n;
            
            if(b.x >= 0.0) {
                if(b.y >= 0.0) {
                    alloc_child(i, 3);
                    i = nodes[i][3];
                    
                    n.x -= d;
                    n.y -= d;
                }else{
                    alloc_child(i, 1);
                    i = nodes[i][1];
                    
                    n.x -= d;
                    n.y += d;
                }
            }else{
                if(b.y >= 0.0) {
                    alloc_child(i, 2);
                    i = nodes[i][2];
                    
                    n.x += d;
                    n.y -= d;
                }else{
                    alloc_child(i, 0);
                    i = nodes[i][0];
                    
                    n.x += d;
                    n.y += d;
                }
            }
        }
        
        nodes[i].add(ptr);
        if(nodes[i].count > 1) {
            nodes[i].node.add(b);
        }else{
            nodes[i].node.set(b);
        }
    }
};

#endif /* DynamicQuadTree_h */
