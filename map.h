#ifndef _MAP_H_
#define _MAP_H_

#include <iostream>
#include <stdexcept>
#include "utilities.h"

namespace my {
    enum Color {RED, BLACK};
    template <class K, class V> struct mapNode;
    template <class K, class V, class L = my::less<const K>, class E = my::equals<const K> > class map;
    template <class K, class V>
    struct print {
        void operator()(const std::pair<const K, V>& p) const {
            std::cout << p.first << '\t' << p.second << '\n';
        }
    };
}
template <class K, class V>
struct my::mapNode {
    std::pair<const K, V> data;
    Color color;
    mapNode *parent, *right, *left;
    mapNode(): parent(NULL), right(NULL), left(NULL), color(RED) {}
    mapNode(const std::pair<const K, V>& val): parent(NULL), right(NULL), left(NULL), color(RED), data(val) {}
    mapNode* grandparent() {
        if (this->parent == NULL) return NULL;
        return this->parent->parent;
    }
    mapNode* sibling() {
        if (this->parent == NULL) return NULL;
        if (this == this->parent->left)
            return this->parent->right;
        return this->parent->left;
    }
    mapNode* uncle() {
        mapNode* g = this->grandparent();
        if (g == NULL) return NULL;
        return this->parent->sibling();
    }
    bool has_red_child() { 
        return (left != NULL && left->color == RED) || (right != NULL && right->color == RED); 
    } 
    mapNode* next() {
        mapNode* node = this;
        if(node->right != NULL) {
            node = node->right;
            while (node->left != NULL)
                node = node->left;
        }
        else {
            mapNode* p = node->parent;
            while(p != NULL && node == p->right) {
                node = p; p = p->parent;
            } 
            node = p;
        }
        return node;
    }
    mapNode* previous() {
        mapNode* node = this;
        if(node->left != NULL) {
            node = node->left;
            while (node->right != NULL)
                node = node->right;
        }
        else {
            mapNode* p = node->parent;
            while(p != NULL && node == p->left) {
                node = p; p = p->parent;
            } 
            node = p;
        }
        return node;
    }
};

template <class K, class V, class L, class E>
class my::map {
    public: typedef std::pair<const K, V> value_type;
    private: typedef mapNode<K, V> node;
    public:
    template <class U>
    class map_iterator {
        friend class map;
        public:
            typedef U value_type;
            typedef U& reference;
            typedef U* pointer;
            typedef std::bidirectional_iterator_tag iterator_category;
            typedef void difference_type;
            map_iterator(node* p): hd(p) {}
            map_iterator& operator=(const map_iterator& it) {hd = it.hd; return *this;} 
            bool operator==(const map_iterator& it) {return hd == it.hd;}
            bool operator!=(const map_iterator& it) {return hd != it.hd;}
            reference operator*() {return hd->data;}
            pointer operator->() { return &(hd->data);}
        private:
            node* hd;
    };
    class iterator : public map_iterator<value_type> {
        public:
        iterator(node* p = NULL): map_iterator<value_type>(p) {}
        iterator& operator++() {this->hd = this->hd->next(); return *this;}
        iterator operator++(int) {iterator it(this->hd); ++*this; return it;}
        iterator& operator--() {this->hd = this->hd->previous(); return *this;}
        iterator operator--(int) {iterator it(this->hd); --*this; return it;}
    };
    class const_iterator : public map_iterator<const value_type>{
        public:
        const_iterator(node* p = NULL): map_iterator<const value_type>(p) {}
        const_iterator(const iterator& it): map_iterator<const value_type>(it.hd) {}
        const_iterator& operator++() {this->hd = this->hd->next(); return *this;}
        const_iterator operator++(int) {const_iterator it(this->hd); ++*this; return it;}
        const_iterator& operator--() {this->hd = this->hd->previous(); return *this;}
        const_iterator operator--(int) {const_iterator it(this->hd); --*this; return it;}
    };
    class reverse_iterator : public map_iterator<value_type> {
        public:
        reverse_iterator(node* p = NULL): map_iterator<value_type>(p) {}
        reverse_iterator& operator++() {this->hd = this->hd->previous(); return *this;}
        reverse_iterator operator++(int) {reverse_iterator it(this->hd); ++*this; return it;}
        reverse_iterator& operator--() {this->hd = this->hd->next(); return *this;}
        reverse_iterator operator--(int) {reverse_iterator it(this->hd); --*this; return it;}
    };
    class const_reverse_iterator : public map_iterator<const value_type> {
        public:
        const_reverse_iterator(node* p = NULL): map_iterator<const value_type>(p) {}
        const_reverse_iterator& operator++() {this->hd = this->hd->previous(); return *this;}
        const_reverse_iterator operator++(int) {const_reverse_iterator it(this->hd); ++*this; return it;}
        const_reverse_iterator& operator--() {this->hd = this->hd->next(); return *this;}
        const_reverse_iterator operator--(int) {const_reverse_iterator it(this->hd); --*this; return it;}
    };
    map(const L& less_comp = L(), const E& equal_comp = E());
    map(const map& t);
    template <class InputIterator>
    map(InputIterator first, InputIterator last);
    #if __cplusplus >= 201103L
    map(std::initializer_list<value_type> l);
    void insert (std::initializer_list<value_type> il);
    map& operator=(std::initializer_list<value_type> il);
    template <class... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {return insert(T(args...));}
    #endif
    ~map() {delete_tree(root);}
    map& operator=(const map& t);
    V& at(const K& key);
    V& operator[] (const K& key);
    std::pair<iterator, bool> insert(const value_type& val);
    template <class InputIterator>
    void insert (InputIterator first, InputIterator last);
    void erase(const K& val);
    void erase(const_iterator it);
    void erase(const_iterator first, const_iterator last);
    void clear() {delete_tree(root); root = NULL;}
    size_t size() {return count;}
    bool empty() {return count == 0;}
    template <class OutputIterator>
    void copy(OutputIterator first) {inorder_copy(root, first);}
    iterator find(const K& val) {return iterator(search(root, val));}
    template <class P>
    void print(P func) {inorder_print(root, func);}
    void print() {inorder_print(root, my::print<K, V>());}
    void swap(map& t);
    iterator begin() const {return iterator(first());}
    iterator end() const { return iterator(NULL);}
    const_iterator cbegin() const {return const_iterator(first());}
    const_iterator cend() const { return const_iterator(NULL);}
    reverse_iterator rbegin() const {return reverse_iterator(NULL);}
    reverse_iterator rend() const { return reverse_iterator(last());}
    const_reverse_iterator crbegin() const {return const_reverse_iterator(NULL);}
    const_reverse_iterator crend() const { return const_reverse_iterator(last());}

    protected:
    void rotate_left(node*);
    void rotate_right(node*);
    void insert_repair(node*);
    node* search(node* root, const K& key) const;
    node* BSTInsert(node* root, node* pt, node*& s);
    node* find_replacement(node* x) const;
    void replace_node(node* v, node* u);
    void delete_node(node* v);
    void delete_case6(node* n);
    void delete_case5(node* n);
    void delete_case4(node* n);
    void delete_case3(node* n);
    void delete_case2(node* n);
    void delete_case1(node* n);
    node* first() const;
    node* last() const;
    void delete_tree(node* n);
    void copy_tree(node* &n, node* p, node* np = NULL);
    template <class P>
    void inorder_print(node* root, const P& func) const;
    template <class OutputIterator>
    void inorder_copy(node* n, OutputIterator& first) const;

    private:
    node* root;
    size_t count;
    L less;
    E equals;
};
template <class K, class V, class L, class E>
void my::map<K, V, L, E>::rotate_left(node* n) {
    node* nnew = n->right, *p = n->parent;
    n->right = nnew->left; 
    nnew->left = n;
    n->parent = nnew;
    if (n->right != NULL)
        n->right->parent = n;
    if (p != NULL) {
        if (n == p->left)
            p->left = nnew;
        else if (n == p->right)
            p->right = nnew;
    }
    else
        root = nnew;
    nnew->parent = p;
}
template <class K, class V, class L, class E>
void my::map<K, V, L, E>::rotate_right(node* n) {
    node* nnew = n->left, *p = n->parent;
    n->left = nnew->right;
    nnew->right = n;
    n->parent = nnew;
    if (n->left != NULL)
        n->left->parent = n;
    if (p != NULL) {
        if (n == p->left)
            p->left = nnew;
        else if (n == p->right)
            p->right = nnew;
    }
    else
        root = nnew;
    nnew->parent = p;
}
template <class K, class V, class L, class E>
my::mapNode<K, V>* my::map<K, V, L, E>::BSTInsert(node* root, node* n, node*& s) {
    if (root == NULL) 
       return n;
    if (equals(n->data.first, root->data.first)) {
        s = root;
        return root;
    }
    else if (less (n->data.first, root->data.first)) {
        root->left  = BSTInsert(root->left, n, s); 
        root->left->parent = root;
    }
    else { 
        root->right = BSTInsert(root->right, n, s);
        root->right->parent = root; 
    }
    return root;
}
template <class K, class V, class L, class E>
my::mapNode<K, V>* my::map<K, V, L, E>::search(node* root, const K& key) const{
    if (root == NULL || equals(root->data.first, key))
        return root;
    if (less(key, root->data.first))
        return search(root->left, key);
    return search(root->right, key);
}
template <class K, class V, class L, class E>
my::mapNode<K, V>* my::map<K, V, L, E>::find_replacement(node* x) const {
    if (x->left != NULL and x->right != NULL) {
        x = x->right;
        while (x->left != NULL)
            x = x->left;
        return x;
    }
    if (x->left == NULL) 
        return x->right; 
    else
        return x->left;
}
template <class K, class V, class L, class E>
void my::map<K, V, L, E>::replace_node(node *v, node *u) {
    if (v->parent != NULL) {
        if (v == v->parent->left)
            v->parent->left = u;
        else
            v->parent->right = u;
    }
    else
        root = u;
    if (u != NULL) u->parent = v->parent;
}
template <class K, class V, class L, class E>
void my::map<K, V, L, E>::delete_node(node* v) {
    node* u = find_replacement(v);
    /*---has two non-leaf children: swap u with v---*/
    //Easier to just swap the values, but we have to account for iterators
    if (v->left != NULL and v->right != NULL) {
        my::swap(u->color, v->color);
        node* t_parent = (u->parent == v)? u: u->parent;
        node* t_right = u->right;
        bool isleft = u == u->parent->left;
        u->parent = v->parent;
        if (v->parent != NULL) {
            if (v->parent->left == v)
                v->parent->left = u;
            else
                v->parent->right = u;
        }
        else
            root = u;
        u->left = v->left;
        if (u->left != NULL) u->left->parent = u;
        u->right = (v->right == u)? v: v->right;
        if (u->right != NULL) u->right->parent = u;
        v->parent = t_parent;
        v->right = t_right;
        if (t_right != NULL) t_right->parent = v;
        v->left = NULL;
        if (isleft) t_parent->left = v;
        u = t_right;
    }
    if (v->color == BLACK) {
        if (u != NULL && u->color == RED)
            u->color = BLACK;
        else
            delete_case1(v);
    }
    replace_node(v, u);
    delete v;
}
template <class K, class V, class L, class E>
void my::map<K, V, L, E>::delete_case1(node *n) {
    if (n->parent != NULL)
        delete_case2(n);
}
template <class K, class V, class L, class E>
void my::map<K, V, L, E>::delete_case2(node *n) {
    node* s = n->sibling();
    if (s->color == RED) {
        n->parent->color = RED;
        s->color = BLACK;
        if (n == s->parent->left)
            rotate_left(n->parent);
        else
            rotate_right(n->parent);
    }
    delete_case3(n);
}
template <class K, class V, class L, class E>
void my::map<K, V, L, E>::delete_case3(node *n) {
    node* s = n->sibling();
    if ((s->parent->color == BLACK) && (s->color == BLACK) && !s->has_red_child()) {
        s->color = RED;
        delete_case1(n->parent);
    } 
    else
        delete_case4(n);
}
template <class K, class V, class L, class E>
void my::map<K, V, L, E>::delete_case4(node *n) {
    node* s = n->sibling();
    if ((n->parent->color == RED) && (s->color == BLACK) && !s->has_red_child()) {
        s->color = RED;
        n->parent->color = BLACK;
    }
    else
        delete_case5(n);
}
template <class K, class V, class L, class E>
void my::map<K, V, L, E>::delete_case5(node *n) {
    node* s = n->sibling();
    if ((n == n->parent->left) && (s->right == NULL || s->right->color == BLACK)) {
        s->color = RED;
        if (s->left != NULL) s->left->color = BLACK;
        rotate_right(s);
    }
    else if ((n == n->parent->right) && (s->left == NULL || s->left->color == BLACK)) {
        s->color = RED;
        if (s->right != NULL) s->right->color = BLACK;
        rotate_left(s);
    }
    delete_case6(n);
}
template <class K, class V, class L, class E>
void my::map<K, V, L, E>::delete_case6(node *n) {
    node* s = n->sibling();

    s->color = n->parent->color;
    n->parent->color = BLACK;

    if (n == n->parent->left) {
        if (s->right != NULL) s->right->color = BLACK;
        rotate_left(n->parent);
    }
    else {
        if (s->left != NULL) s->left->color = BLACK;
        rotate_right(n->parent);
    }
}
template <class K, class V, class L, class E>
void my::map<K, V, L, E>::insert_repair(node* x) {
    if (x->parent == NULL) {
        x->color = BLACK;
        return;
    }
    if (x->parent->color == BLACK)
        return;
    if (x->uncle() != NULL && x->uncle()->color == RED) {
        x->parent->color = BLACK;
        x->uncle()->color = BLACK;
        x->grandparent()->color = RED;
        insert_repair(x->grandparent());
        return;
    }
    node *p = x->parent, *g = x->grandparent();
    if (x == p->right && p == g->left) {
        rotate_left(p);
        x = x->left;
    }
    else if (x == p->left && p == g->right) {
        rotate_right(p);
        x = x->right;
    }
    p = x->parent;
    g = x->grandparent();
    if (x == p->left)
        rotate_right(g);
    else
        rotate_left(g);
    p->color = BLACK;
    g->color = RED;
}
template <class K, class V, class L, class E>
my::mapNode<K, V>* my::map<K, V, L, E>::first() const {
    node* temp = root;
    if (temp != NULL) {
        while (temp->left != NULL)
            temp = temp->left;
    }
    return temp;
}
template <class K, class V, class L, class E>
my::mapNode<K, V>* my::map<K, V, L, E>::last() const {
    node* temp = root;
    if (temp != NULL) {
        while (temp->right != NULL)
            temp = temp->right;
    }
    return temp;
}
template <class K, class V, class L, class E>
void my::map<K, V, L, E>::delete_tree(node *n) {
    if (n == NULL) return;
    delete_tree(n->left);
    delete_tree(n->right);
    delete n;
}
template <class K, class V, class L, class E>
void my::map<K, V, L, E>::copy_tree(node* &n, node* p, node*np) {
    if (p == NULL)
        return;
    n = new node(p->data);
    n->color = p->color; n->parent = np;
    copy_tree(n->left, p->left, n);
    copy_tree(n->right, p->right, n);
}
template <class K, class V, class L, class E> template <class P>
void my::map<K, V, L, E>::inorder_print(node* root, const P& func) const {
    if (root != NULL) {
        inorder_print(root->left, func);
        func(root->data);
        inorder_print(root->right, func);
    }
}
template <class K, class V, class L, class E> template <class OutputIterator>
void my::map<K, V, L, E>::inorder_copy(node* n, OutputIterator& first) const {
    if (n == NULL)
        return;
    inorder_copy(n->left, first);
    *first = n->data; ++first;
    inorder_copy(n->right, first);
}
template <class K, class V, class L, class E>
my::map<K, V, L, E>::map(const L& less_comp, const E& equal_comp) : root(NULL), count(0) {
    less = less_comp;
    equals = equal_comp;
}
template <class K, class V, class L, class E> template <class InputIterator>
my::map<K, V, L, E>::map(InputIterator first, InputIterator last): map(L(), E()) {
    insert(first, last);
}
template <class K, class V, class L, class E>
my::map<K, V, L, E>::map(const map& t): map(L(), E()) {
    copy_tree(root, t.root);
    count = t.count;
}
#if __cplusplus >= 201103L
template <class K, class V, class L, class E>
my::map<K, V, L, E>::map(std::initializer_list<value_type> l): map(L(), E()) {
    insert(l.begin(), l.end());
}
template <class K, class V, class L, class E>
void my::map<K, V, L, E>::insert(std::initializer_list<value_type> l) {
    insert(l.begin(), l.end());
}
template <class K, class V, class L, class E>
my::map<K, V, L, E>& my::map<K, V, L, E>::operator=(std::initializer_list<value_type> l) {
    clear();
    insert(l.begin(), l.end());
    return *this;
}
#endif
template <class K, class V, class L, class E> 
my::map<K, V, L, E>& my::map<K, V, L, E>::operator=(const map& t) {
    clear();
    copy_tree(root, t.root);
    count = t.count;
    return *this;
}
template <class K, class V, class L, class E>
V& my::map<K, V, L, E>::at(const K& key) {
    node* n = search(root, key);
    if (n == NULL)
        throw std::out_of_range("my::map::range_check. Key not found\n");
    return n->data.second;
}
template <class K, class V, class L, class E>
V& my::map<K, V, L, E>::operator[](const K& key) {
    node* n = search(root, key);
    if (n == NULL) {
        return insert(value_type(key, V())).first->second;
    }
    return n->data.second;
}
template <class K, class V, class L, class E> 
std::pair<class my::map<K, V, L, E>::iterator, bool> my::map<K, V, L, E>::insert(const value_type& val) {
    node *pt = new node(val);
    node* s = NULL;
    root = BSTInsert(root, pt, s);
    if (s == NULL) {
        insert_repair(pt);
        ++count;
        return std::pair<iterator, bool>(iterator(pt), true);
    }
    delete pt;
    return std::pair<iterator, bool>(iterator(s), false);
}
template <class K, class V, class L, class E> template <class InputIterator>
void my::map<K, V, L, E>::insert(InputIterator first, InputIterator last) {
    for (; first != last; ++first)
        insert(*first);
}
template <class K, class V, class L, class E>
void my::map<K, V, L, E>::erase(const K& val) {
    node* n = search(root, val);
    if (n == NULL)
        return;
    delete_node(n);
    --count;
}
template <class K, class V, class L, class E>
void my::map<K, V, L, E>::erase(const_iterator it) {
    if (root == NULL || it.hd == NULL)
        return;
    delete_node(it.hd);
    --count;
}
template <class K, class V, class L, class E>
void my::map<K, V, L, E>::erase(const_iterator first, const_iterator last) {
    if (root == NULL || first.hd == NULL)
        return;
    node* temp;
    while (first != last) {
        temp = first.hd;
        ++first;
        delete_node(temp);
        --count;
    }
}
template <class K, class V, class L, class E>
void my::map<K, V, L, E>::swap(map& x) {
    node* t_root = root; root = x.root; x.root = t_root;
    size_t t_count = count; count = x.count; x.count = t_count;
}

#endif
