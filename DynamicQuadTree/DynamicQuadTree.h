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
#include "AABB.h"

typedef int32_t qt_int;

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
        
        inline void clear() {
            count = 0;
        }
        
        inline void grow() {
            if(capacity == 0) {
                capacity = 4;
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
        
        inline bool empty() const
        {
            return children[0] == -1 && children[1] == -1 && children[2] == -1 && children[3] == -1 && count == 0;
        }
        
    };
    
    inline bool should_solve(qt_int a, qt_int b) const
    {
        //return touches(nodes[a].aabb, nodes[b].aabb, h);
        return true;
    }
    
    float h;
    float rootSize;
    
    Node* nodes;
    
    qt_int size;
    qt_int capacity;
    
    qt_int root;
    
    qt_int level;
    
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
        expand_once();
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
        qt_int n[4] = {-1, -1, -1, -1};
        
        for(qt_int i = 0; i < 4; ++i) {
            
            if(get(root, i) != -1) {
                n[i] = alloc_node();
                nodes[n[i]][3 - i] = get(root, i);
            }
            
        }
        
        for(qt_int i = 0; i < 4; ++i) {
            nodes[root][i] = n[i];
        }
        
        rootSize *= 2.0f;
        ++level;
    }
    
    void grow_to(const vec2& p)
    {
        float pSize = std::max(abs(p.x), abs(p.y)) * 2.0f;
        while(rootSize < pSize)
            expand_once();
    }
    
    inline qt_int& get(qt_int i, uint8_t n) const
    {
        return nodes[i][n];
    }
    
    inline qt_int at(qt_int i, uint8_t n) const
    {
        if(i == -1) return -1;
        return nodes[i][n];
    }
    
    inline qt_int at(qt_int i, uint8_t n, uint8_t k) const
    {
        return at(at(i, n), k);
    }
    
    template <class _Solver>
    void solve_single(qt_int n, _Solver& solver) {
        qt_int& ct = nodes[n].count;
        for(qt_int i = 0; i < ct; ++i) {
            for(qt_int j = i; j < ct; ++j) {
                solver(nodes[n].data[i], nodes[n].data[j]);
            }
        }
    }
    
    template <class _Solver>
    void solve_cells(qt_int n0, qt_int n1, _Solver& solver) {
        for(T*& p0 : nodes[n0])
            for(T*& p1 : nodes[n1])
                solver(p0, p1);
    }
    
    template <class _Solver>
    void solve_level1(qt_int n0, qt_int n1, qt_int n2, qt_int n3, _Solver& solver) {
        if(n0 != -1) {
            solve_single(n0, solver);
            if(n1 != -1) solve_cells(n0, n1, solver);
            if(n2 != -1) solve_cells(n0, n2, solver);
            if(n3 != -1) solve_cells(n0, n3, solver);
        }
        
        if(n1 != -1) {
            solve_single(n1, solver);
            if(n2 != -1) solve_cells(n1, n2, solver);
            if(n3 != -1) solve_cells(n1, n3, solver);
        }
        
        if(n2 != -1) {
            solve_single(n2, solver);
            if(n3 != -1) solve_cells(n2, n3, solver);
        }
        
        if(n3 != -1) {
            solve_single(n3, solver);
        }
    }
    
    template <class _Solver>
    void solve_level1_v(qt_int n0, qt_int n1, qt_int n2, qt_int n3, _Solver& solver) {
        if(n0 != -1 && n2 != -1)
            solve_cells(n0, n2, solver);
        
        if(n1 != -1 && n3 != -1)
            solve_cells(n1, n3, solver);
    }
    
    template <class _Solver>
    void solve_level1_h(qt_int n0, qt_int n1, qt_int n2, qt_int n3, _Solver& solver) {
        if(n0 != -1 && n1 != -1)
            solve_cells(n0, n1, solver);
        
        if(n2 != -1 && n3 != -1)
            solve_cells(n2, n3, solver);
    }
    
    template <class _Solver>
    void solve_level1_c(qt_int n0, qt_int n1, qt_int n2, qt_int n3, _Solver& solver) {
        if(n0 != -1 && n3 != -1)
            solve_cells(n0, n3, solver);
        
        if(n1 != -1 && n2 != -1)
            solve_cells(n1, n2, solver);
    }
    
    template <class _Solver>
    void solve_node_v(qt_int n0, qt_int n1, qt_int n2, qt_int n3, qt_int l, _Solver& solver) {
        if(l == 0) {
            printf("Unexpected result. \n");
            return;
        }
        
        if(l == 1) {
            solve_level1_v(n0, n1, n2, n3, solver);
            solve_level1_c(n0, n1, n2, n3, solver);
            return;
        }
        
        if(l >= 2) {
            solve_node_c(at(n0, 3), at(n1, 2), at(n2, 1), at(n3, 0), l - 1, solver);
        }
        
        if(n0 != -1 && n2 != -1 && should_solve(n0, n2))
            solve_node_v(get(n0, 2), get(n0, 3), get(n2, 0), get(n2, 1), l - 1, solver);
        
        if(n1 != -1 && n3 != -1 && should_solve(n1, n3))
            solve_node_v(get(n1, 2), get(n1, 3), get(n3, 0), get(n3, 1), l - 1, solver);
    }
    
    template <class _Solver>
    void solve_node_h(qt_int n0, qt_int n1, qt_int n2, qt_int n3, qt_int l, _Solver& solver) {
        if(l == 0) {
            printf("Unexpected result. \n");
            return;
        }
        
        if(l == 1) {
            solve_level1_h(n0, n1, n2, n3, solver);
            solve_level1_c(n0, n1, n2, n3, solver);
            return;
        }
        
        if(l >= 2) {
            solve_node_c(at(n0, 3), at(n1, 2), at(n2, 1), at(n3, 0), l - 1, solver);
        }
        
        if(n0 != -1 && n1 != -1 && should_solve(n0, n1))
            solve_node_h(get(n0, 1), get(n1, 0), get(n0, 3), get(n1, 2), l - 1, solver);
        
        if(n2 != -1 && n3 != -1 && should_solve(n2, n3))
            solve_node_h(get(n2, 1), get(n3, 0), get(n2, 3), get(n3, 2), l - 1, solver);
    }
    
    template <class _Solver>
    void solve_node_c(qt_int n0, qt_int n1, qt_int n2, qt_int n3, qt_int l, _Solver& solver) {
        if(l == 0) {
            printf("Unexpected result. \n");
            return;
        }
        
        if(l == 1) {
            solve_level1_c(n0, n1, n2, n3, solver);
            return;
        }
        
        if((n0 != -1 && n3 != -1 && should_solve(n0, n3)) || (n1 != -1 && n2 != -1 && should_solve(n1, n2)))
            solve_node_c(at(n0, 3), at(n1, 2), at(n2, 1), at(n3, 0), l - 1, solver);
    }
    
    template <class _Solver>
    void solve_node(qt_int n0, qt_int n1, qt_int n2, qt_int n3, qt_int l, _Solver& solver) {
        if(l == 0) {
            solve_cells(root, root, solver);
            return;
        }
        
        if(l == 1) {
            solve_level1(n0, n1, n2, n3, solver);
            return;
        }
        
        if(n0 != -1) {
            solve_node(nodes[n0][0], nodes[n0][1], nodes[n0][2], nodes[n0][3], l - 1, solver);
            
            if(n1 != -1 && should_solve(n0, n1))
                solve_node_h(nodes[n0][1], nodes[n1][0], nodes[n0][3], nodes[n1][2], l - 1, solver);
            
            if(n2 != -1 && should_solve(n0, n2))
                solve_node_v(nodes[n0][2], nodes[n0][3], nodes[n2][0], nodes[n2][1], l - 1, solver);
        }
        
        if(n1 != -1)
            solve_node(nodes[n1][0], nodes[n1][1], nodes[n1][2], nodes[n1][3], l - 1, solver);
        
        if(n2 != -1)
            solve_node(nodes[n2][0], nodes[n2][1], nodes[n2][2], nodes[n2][3], l - 1, solver);
        
        if(n3 != -1) {
            solve_node(nodes[n3][0], nodes[n3][1], nodes[n3][2], nodes[n3][3], l - 1, solver);
            
            if(n1 != -1 && should_solve(n1, n3))
                solve_node_v(nodes[n1][2], nodes[n1][3], nodes[n3][0], nodes[n3][1], l - 1, solver);
            
            if(n2 != -1 && should_solve(n2, n3))
                solve_node_h(nodes[n2][1], nodes[n3][0], nodes[n2][3], nodes[n3][2], l - 1, solver);
        }
        
        if((n0 != -1 && n3 != -1 && should_solve(n0, n3)) || (n1 != -1 && n2 != -1 && should_solve(n1, n2)))
            solve_node_c(at(n0, 3), at(n1, 2), at(n2, 1), at(n3, 0), l - 1, solver);
    }
    
    template <class _Solver>
    void solve(_Solver solver) {
        solve_node(at(root, 0), at(root, 1), at(root, 2), at(root, 3), level, solver);
    }
    
    
    inline void alloc_child(qt_int i, qt_int c) {
        if(nodes[i][c] == -1)
            nodes[i][c] = alloc_node();
    }
    
    
    void insert_pointer(T* ptr, const vec2& p) {
        grow_to(p);
        qt_int i = root;
        float d = rootSize * 0.5f;
        vec2 n = p;
        for(int _n = 0; _n < level; ++_n) {
            d *= 0.5f;
            
            if(n.x >= 0.0f) {
                if(n.y >= 0.0f) {
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
                if(n.y >= 0.0f) {
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
    }
};

#endif /* DynamicQuadTree_h */

