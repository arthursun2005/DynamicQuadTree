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

/*
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
        
        AABB aabb;
        
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
        
        inline void add(const vec2& p)
        {
            if(count == 0)
                aabb.set(p);
            else
                aabb.add(p);
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
    
    static float distSq(const Node& a, const Node& b)
    {
        if(a.empty() || b.empty())
            return FLT_MAX;
        return ::distSq(a.aabb, b.aabb);
    }
    
    float h;
    float rootSize;
    
    Node* nodes;
    
    qt_int size;
    qt_int capacity;
    
    qt_int root;
    
    qt_int level;
    
public:
    
    
    DynamicQuadTree(float h) : h(h) , capacity(256), root(0), size(1), level(0), rootSize(h) {
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
            printf("Unexpected delete_last \n");
        }
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
        float pSize = std::max(abs(p.x), abs(p.y)) * 4.0f;
        while(rootSize < pSize)
            expand_once();
    }
    
    inline const qt_int& get(qt_int i, uint8_t n) const
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
    
    void assign_cr(qt_int n, qt_int n0, qt_int n1, qt_int n2, qt_int n3) {
        nodes[n0][3] = at(n, 0);
        nodes[n1][2] = at(n, 1);
        nodes[n2][1] = at(n, 2);
        nodes[n3][0] = at(n, 3);
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
    void solve_level2(qt_int n0, qt_int n1, qt_int n2, qt_int n3, _Solver& solver)
    {
        solve_cells(at(n0, 3), {at(n0, 0), at(n0, 1), at(n0, 2), at(n1, 0)}, solver);
        
        solve_cells(at(n1, 2), {at(n0, 1), at(n1, 0), at(n0, 3), at(n1, 1)}, solver);
        
        solve_cells(at(n2, 1), {at(n0, 2), at(n0, 3), at(n2, 0), at(n1, 2)}, solver);
        
        solve_cells(at(n3, 0), {at(n0, 3), at(n1, 2), at(n2, 1), at(n1, 3)}, solver);
        
        if(at(n0, 3) != -1) solve_single(get(n0, 3), solver);
        
        if(at(n1, 2) != -1) solve_single(get(n1, 2), solver);
        
        if(at(n2, 1) != -1) solve_single(get(n2, 1), solver);
        
        if(at(n3, 0) != -1) solve_single(get(n3, 0), solver);
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
        
        /*
        if(l < level) {
            if(!nodes[n0].empty() && nodes[n1].empty() && nodes[n2].empty() && nodes[n3].empty()) {
                qt_int t0 = alloc_node();
                qt_int t1 = alloc_node();
                qt_int t2 = alloc_node();
                qt_int t3 = alloc_node();
                
                nodes[t0][3] = nodes[n0][0];
                nodes[t1][2] = nodes[n0][1];
                nodes[t2][1] = nodes[n0][2];
                nodes[t3][0] = nodes[n0][3];
                
                solve_center(t0, t1, t2, t3, l - 1, solver);
                
                delete_last(4);
                
                return;
            }
        }
        */
        
        /*
        if(l < level) {
            Node& b0 = nodes[n0];
            Node& b1 = nodes[n1];
            Node& b2 = nodes[n2];
            Node& b3 = nodes[n3];
            
            float d0 = distSq(b0, b1);
            float d1 = distSq(b0, b2);
            float d2 = distSq(b0, b3);
            float d3 = distSq(b1, b2);
            float d4 = distSq(b1, b3);
            float d5 = distSq(b2, b3);
            
            //printf("%f %f %f %f %f %f \n", d0, d1, d2, sd3, d4, d5);
            
            if(d0 > h * h && d1 > h * h && d2 > h * h && d3 > h * h && d4 > h * h && d5 > h * h) {
                
                qt_int t00 = alloc_node();
                qt_int t01 = alloc_node();
                qt_int t02 = alloc_node();
                qt_int t03 = alloc_node();
                
                qt_int t10 = alloc_node();
                qt_int t11 = alloc_node();
                qt_int t12 = alloc_node();
                qt_int t13 = alloc_node();
                
                qt_int t20 = alloc_node();
                qt_int t21 = alloc_node();
                qt_int t22 = alloc_node();
                qt_int t23 = alloc_node();
                
                qt_int t30 = alloc_node();
                qt_int t31 = alloc_node();
                qt_int t32 = alloc_node();
                qt_int t33 = alloc_node();
                
                assign_cr(get(n0, 3), t00, t01, t02, t03);
                
                assign_cr(get(n1, 2), t10, t11, t12, t13);
                
                assign_cr(get(n2, 1), t20, t21, t22, t23);
                
                assign_cr(get(n3, 0), t30, t31, t32, t33);
                
                solve_center(t00, t01, t02, t03, l - 1, solver);
                
                solve_center(t10, t11, t12, t13, l - 1, solver);
                
                solve_center(t20, t21, t22, t23, l - 1, solver);
                
                solve_center(t30, t31, t32, t33, l - 1, solver);
                
                delete_last(16);
                
                return;
            }
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
        
        
        assign_c(n00, at(n0, 0), at(n0, 1), at(n0, 2), at(n0, 3));
        
        assign_c(n02, at(n1, 0), at(n1, 1), at(n1, 2), at(n1, 3));
        
        assign_c(n20, at(n2, 0), at(n2, 1), at(n2, 2), at(n2, 3));
        
        assign_c(n22, at(n3, 0), at(n3, 1), at(n3, 2), at(n3, 3));
        
        
        assign_h2(n21, n2, n3);
        
        assign_h2(n01, n0, n1);
        
        assign_v2(n12, n1, n3);
        
        assign_v2(n10, n0, n2);
        
        
        assign_c2(n11, n0, n1, n2, n3);
        
        
        if(at(n0, 3) != -1)
            solve_center(n00, n01, n10, n11, l - 1, solver);
        
        if(at(n1, 2) != -1)
            solve_center(n01, n02, n11, n12, l - 1, solver);
        
        if(at(n2, 1) != -1)
            solve_center(n10, n11, n20, n21, l - 1, solver);
        
        if(at(n3, 0) != -1)
            solve_center(n11, n12, n21, n22, l - 1, solver);
        
        delete_last(9);
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
        float d = rootSize;
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
            
            nodes[i].add(p);
        }
        
        nodes[i].add(ptr);
        nodes[i].add(p);
    }
};
*/

template <class T>
class DynamicQuadTree
{
    
protected:
    
    struct Node
    {
        T** data;
        
        qt_int children[4];
        
        bool null;
        
        inline const qt_int& operator [] (int i) const { return children[i]; }
        inline qt_int& operator [] (int i) { return children[i]; }
        
        qt_int count;
        qt_int capacity;
        
        AABB aabb;
        
        Node() {}
        
        Node(const Node& x) = delete;
        
        inline Node& operator = (const Node& x) = delete;
        
        inline void init() {
            capacity = 0;
            count = 0;
            null = true;
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
        
        inline void add(const vec2& p)
        {
            if(null) {
                aabb.set(p);
                null = false;
            }else{
                aabb.add(p);
            }
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
    
    float should_solve(qt_int a, qt_int b) const
    {
        return nodes[a].aabb.touches(nodes[b].aabb, h);
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
            
            if(n1 != -1)
                solve_node_h(nodes[n0][1], nodes[n1][0], nodes[n0][3], nodes[n1][2], l - 1, solver);
            
            if(n2 != -1)
                solve_node_v(nodes[n0][2], nodes[n0][3], nodes[n2][0], nodes[n2][1], l - 1, solver);
        }
        
        if(n1 != -1)
            solve_node(nodes[n1][0], nodes[n1][1], nodes[n1][2], nodes[n1][3], l - 1, solver);
        
        if(n2 != -1)
            solve_node(nodes[n2][0], nodes[n2][1], nodes[n2][2], nodes[n2][3], l - 1, solver);
        
        if(n3 != -1) {
            solve_node(nodes[n3][0], nodes[n3][1], nodes[n3][2], nodes[n3][3], l - 1, solver);
            
            if(n1 != -1)
                solve_node_v(nodes[n1][2], nodes[n1][3], nodes[n3][0], nodes[n3][1], l - 1, solver);
            
            if(n2 != -1)
                solve_node_h(nodes[n2][1], nodes[n3][0], nodes[n2][3], nodes[n3][2], l - 1, solver);
        }
        
        if((n0 != -1 && n3 != -1) || (n1 != -1 && n2 != -1))
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
            
            nodes[i].add(p);
            
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
        nodes[i].add(p);
    }
};

#endif /* DynamicQuadTree_h */

