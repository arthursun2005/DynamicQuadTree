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
    };
    
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
    
    void delete_last(qt_int n) {
        if(n <= size) {
            for (qt_int i = 1; i <= n; ++i) {
                nodes[size - i].clear();
                nodes[size - i].children[0] = -1;
                nodes[size - i].children[1] = -1;
                nodes[size - i].children[2] = -1;
                nodes[size - i].children[3] = -1;
            }
            size -= n;
        }else{
            printf("Unexpected\n");
        }
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
        
        nodes[root][0] = n0;
        nodes[root][1] = n1;
        nodes[root][2] = n2;
        nodes[root][3] = n3;
        
        rootSize *= 2.0f;
        ++level;
    }
    
    void grow_to(const vec2& p)
    {
        float pSize = std::max(abs(p.x), abs(p.y)) * 4.0f;
        while(rootSize < pSize)
            expand_once();
    }
    
    inline qt_int& get(qt_int i, uint8_t n)
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
    void solve_cells(qt_int n0, const std::initializer_list<qt_int>& l, _Solver& solver) {
        if(n0 != -1)
            for(T*& p0 : nodes[n0])
                for(const qt_int& n1 : l)
                    if(n1 != -1)
                        for(T*& p1 : nodes[n1])
                            solver(p0, p1);
    }
    
    void assign_c(qt_int n, qt_int n0, qt_int n1, qt_int n2, qt_int n3) {
        nodes[n][0] = at(n0, 3);
        nodes[n][1] = at(n1, 2);
        nodes[n][2] = at(n2, 1);
        nodes[n][3] = at(n3, 0);
    }
    
    void assign_h2(qt_int n, qt_int n0, qt_int n1) {
        nodes[n][0] = at(n0, 1, 3);
        nodes[n][1] = at(n1, 0, 2);
        nodes[n][2] = at(n0, 3, 1);
        nodes[n][3] = at(n1, 2, 0);
    }
    
    void assign_v2(qt_int n, qt_int n0, qt_int n2) {
        nodes[n][0] = at(n0, 2, 3);
        nodes[n][1] = at(n0, 3, 2);
        nodes[n][2] = at(n2, 0, 1);
        nodes[n][3] = at(n2, 1, 0);
    }
    
    void assign_c2(qt_int n, qt_int n0, qt_int n1, qt_int n2, qt_int n3) {
        nodes[n][0] = at(n0, 3, 3);
        nodes[n][1] = at(n1, 2, 2);
        nodes[n][2] = at(n2, 1, 1);
        nodes[n][3] = at(n3, 0, 0);
    }
    
    template <class _Solver>
    void solve_level2(qt_int n0, qt_int n1, qt_int n2, qt_int n3, _Solver& solver) {
        solve_cells(at(n0, 3), {at(n0, 0), at(n0, 1), at(n0, 2), at(n1, 0)}, solver);
        solve_cells(at(n1, 2), {at(n0, 1), at(n1, 0), at(n0, 3), at(n1, 1)}, solver);
        solve_cells(at(n2, 1), {at(n0, 2), at(n0, 3), at(n2, 0), at(n1, 2)}, solver);
        solve_cells(at(n3, 0), {at(n0, 3), at(n1, 2), at(n2, 1), at(n1, 3)}, solver);
        
        if(at(n0, 3) != -1) solve_single(get(n0, 3), solver);
        
        if(at(n1, 2) != -1) solve_single(get(n1, 2), solver);
        
        if(at(n2, 1) != -1) solve_single(get(n2, 1), solver);
        
        if(at(n3, 0) != -1) solve_single(get(n3, 0), solver);
    }
    
    inline bool empty_center(qt_int n0, qt_int n1, qt_int n2, qt_int n3) {
        if(at(n0, 3) == -1 && at(n1, 2) == -1 && at(n2, 1) == -1 && at(n3, 0) == -1) {
            return true;
        }
        return false;
    }
    
    template <class _Solver>
    void solve_center(qt_int n0, qt_int n1, qt_int n2, qt_int n3, qt_int l, _Solver& solver) {
        if(l == 1) {
            solve_level1(n0, n1, n2, n3, solver);
            return;
        }
        
        if(l == 2) {
            solve_level2(n0, n1, n2, n3, solver);
            return;
        }
        
        qt_int n00 = alloc_node();
        qt_int n01 = alloc_node();
        qt_int n02 = alloc_node();
        qt_int n10 = alloc_node();
        qt_int n11 = alloc_node();
        qt_int n12 = alloc_node();
        qt_int n20 = alloc_node();
        qt_int n21 = alloc_node();
        qt_int n22 = alloc_node();
        
        if(n0 != -1) {
            assign_c(n00, get(n0, 0), get(n0, 1), get(n0, 2), get(n0, 3));
        }
        
        if(n1 != -1) {
            assign_c(n02, get(n1, 0), get(n1, 1), get(n1, 2), get(n1, 3));
        }
        
        if(n2 != -1) {
            assign_c(n20, get(n2, 0), get(n2, 1), get(n2, 2), get(n2, 3));
        }
        
        if(n3 != -1) {
            assign_c(n22, get(n3, 0), get(n3, 1), get(n3, 2), get(n3, 3));
        }
        
        if(n0 != -1 && n1 != -1) {
            assign_h2(n01, n0, n1);
        }
        
        if(n2 != -1 && n3 != -1) {
            assign_h2(n21, n2, n3);
        }
        
        if(n0 != -1 && n2 != -1) {
            assign_v2(n10, n0, n2);
        }
        
        if(n1 != -1 && n3 != -1) {
            assign_v2(n12, n1, n3);
        }
        
        if(n0 != -1 && n2 != -1 && n1 != -1 && n3 != -1) {
            assign_c2(n11, n0, n1, n2, n3);
        }
        
        if(!empty_center(n00, n01, n10, n11))
            solve_center(n00, n01, n10, n11, l - 1, solver);
        
        if(!empty_center(n01, n02, n11, n12))
            solve_center(n01, n02, n11, n12, l - 1, solver);
        
        if(!empty_center(n10, n11, n20, n21))
            solve_center(n10, n11, n20, n21, l - 1, solver);
        
        if(!empty_center(n11, n12, n21, n22))
            solve_center(n11, n12, n21, n22, l - 1, solver);
        
        delete_last(9);
    }
    
    template <class _Solver>
    inline void solve(_Solver solver) {
        solve_center(at(root, 0), at(root, 1), at(root, 2), at(root, 3), level, solver);
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
