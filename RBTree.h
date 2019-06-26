#ifndef RBTREE_H
#define RBTREE_H

namespace my {
    enum Color {RED, BLACK};
    template <class T> struct treeNode;
    template <class T, class C> class RBTree;
}
template <class T>
struct my::treeNode {
    T data;
    Color color;
    treeNode *parent, *right, *left;
    treeNode(): right(NULL), left(NULL), color(RED) {}
    treeNode(const T& val): parent(NULL), right(NULL), left(NULL), color(RED), data(val) {}
    treeNode* grandparent() {
        if (this->parent == NULL) return NULL;
        return this->parent->parent;
    }
    treeNode* sibling() {
        if (this->parent == NULL) return NULL;
        if (this == this->parent->left)
            return this->parent->right;
        return this->parent->left;
    }
    treeNode* uncle() {
        treeNode* g = this->grandparent();
        if (g == NULL) return NULL;
        return this->parent->sibling();
    }
    bool has_red_child() { 
        return (left != NULL && left->color == RED) || (right != NULL && right->color == RED); 
    } 
    treeNode* next() {
        treeNode* node = this;
        if(node->right != NULL) {
            node = node->right;
            while (node->left != NULL)
                node = node->left;
        }
        else {
            treeNode* p = node->parent;
            while(p != NULL && node == p->right) {
                node = p; p = p->parent;
            } 
            node = p;
        }
        return node;
    }
    treeNode* previous() {
        treeNode* node = this;
        if(node->left != NULL) {
            node = node->left;
            while (node->right != NULL)
                node = node->right;
        }
        else {
            treeNode* p = node->parent;
            while(p != NULL && node == p->left) {
                node = p; p = p->parent;
            } 
            node = p;
        }
        return node;
    }
};

template <class T, class C>
class my::RBTree {
    typedef treeNode<T> node;

    public:
    typedef T value_type;
    typedef C key_compare;
    template <class U>
    class RBTree_iterator {
        friend class RBTree;
        public:
            typedef U value_type;
            typedef U& reference;
            typedef U* pointer;
            typedef std::bidirectional_iterator_tag iterator_category;
            typedef void difference_type;
            RBTree_iterator(node* p): hd(p) {}
            RBTree_iterator& operator=(const RBTree_iterator& it) {hd = it.hd; return *this;} 
            bool operator==(const RBTree_iterator& it) {return hd == it.hd;}
            bool operator!=(const RBTree_iterator& it) {return hd != it.hd;}
            reference operator*() {return hd->data;}
            pointer operator->() { return &(hd->data);}
        private:
            node* hd;
    };
    class iterator : public RBTree_iterator<T> {
        public:
        iterator(node* p = NULL): RBTree_iterator<T>(p) {}
        iterator& operator++() {this->hd = this->hd->next(); return *this;}
        iterator operator++(int) {iterator it(this->hd); ++*this; return it;}
        iterator& operator--() {this->hd = this->hd->previous(); return *this;}
        iterator operator--(int) {iterator it(this->hd); --*this; return it;}
    };
    class const_iterator : public RBTree_iterator<const T>{
        public:
        const_iterator(node* p = NULL): RBTree_iterator<const T>(p) {}
        const_iterator(const iterator& it): RBTree_iterator<const T>(it.hd) {}
        const_iterator& operator++() {this->hd = this->hd->next(); return *this;}
        const_iterator operator++(int) {const_iterator it(this->hd); ++*this; return it;}
        const_iterator& operator--() {this->hd = this->hd->previous(); return *this;}
        const_iterator operator--(int) {const_iterator it(this->hd); --*this; return it;}
    };
    class reverse_iterator : public RBTree_iterator<T> {
        public:
        reverse_iterator(node* p = NULL): RBTree_iterator<T>(p) {}
        reverse_iterator& operator++() {this->hd = this->hd->previous(); return *this;}
        reverse_iterator operator++(int) {reverse_iterator it(this->hd); ++*this; return it;}
        reverse_iterator& operator--() {this->hd = this->hd->next(); return *this;}
        reverse_iterator operator--(int) {reverse_iterator it(this->hd); --*this; return it;}
    };
    class const_reverse_iterator : public RBTree_iterator<const T> {
        public:
        const_reverse_iterator(node* p = NULL): RBTree_iterator<const T>(p) {}
        const_reverse_iterator& operator++() {this->hd = this->hd->previous(); return *this;}
        const_reverse_iterator operator++(int) {const_reverse_iterator it(this->hd); ++*this; return it;}
        const_reverse_iterator& operator--() {this->hd = this->hd->next(); return *this;}
        const_reverse_iterator operator--(int) {const_reverse_iterator it(this->hd); --*this; return it;}
    }; 
    explicit RBTree(const C& comp = C()); //empty constructor
    RBTree(const RBTree& t);  //copy constructor
    #if __cplusplus >= 201103L
    RBTree(RBTree&& t);   //move constructor
    void insert (std::initializer_list<T> l);
    RBTree& operator=(std::initializer_list<T> l);
    RBTree& operator=(RBTree&& t);
    #endif
    ~RBTree() {delete_tree(root);}
    RBTree& operator=(const RBTree& t);
    template <class InputIterator>
    void insert (InputIterator first, InputIterator last);
    void erase(const_iterator it);
    void erase(const_iterator first, const_iterator last);
    void clear() {delete_tree(root); root = NULL;}
    size_t size() {return size_;}
    bool empty() {return size_ == 0;}
    template <class OutputIterator>
    void copy(OutputIterator first) {inorder_copy(root, first);}
    void swap(RBTree& t);
    C key_comp() const {return compare;}
    iterator begin() const {return iterator(first());}
    iterator end() const { return iterator(NULL);}
    const_iterator cbegin() const {return const_iterator(first());}
    const_iterator cend() const { return const_iterator(NULL);}
    reverse_iterator rbegin() const {return reverse_iterator(last());}
    reverse_iterator rend() const { return reverse_iterator(NULL);}
    const_reverse_iterator crbegin() const {return const_reverse_iterator(last());}
    const_reverse_iterator crend() const { return const_reverse_iterator(NULL);}

    protected:
    void rotate_left(node*);
    void rotate_right(node*);
    void insert_repair(node*);
    virtual void insert_help(node* pt) = 0;
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

    node* root;
    size_t size_;
    C compare;
};
template <class T, class C>
void my::RBTree<T, C>::rotate_left(node* n) {
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
template <class T, class C>
void my::RBTree<T, C>::rotate_right(node* n) {
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
template <class T, class C>
my::treeNode<T>* my::RBTree<T, C>::find_replacement(node* x) const {
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
template <class T, class C>
void my::RBTree<T, C>::replace_node(node *v, node *u) {
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
template <class T, class C>
void my::RBTree<T, C>::delete_node(node* v) {
    node* u = find_replacement(v);
    /*---has two non-leaf children: swap u with v---*/
    //Easier to just swap the values, but we have to acsize_ for iterators
    if (v->left != NULL and v->right != NULL) {
        Color c = v->color; v->color = u->color; u->color = c;
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
template <class T, class C>
void my::RBTree<T, C>::delete_case1(node *n) {
    if (n->parent != NULL)
        delete_case2(n);
}
template <class T, class C>
void my::RBTree<T, C>::delete_case2(node *n) {
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
template <class T, class C>
void my::RBTree<T, C>::delete_case3(node *n) {
    node* s = n->sibling();
    if ((s->parent->color == BLACK) && (s->color == BLACK) && !s->has_red_child()) {
        s->color = RED;
        delete_case1(n->parent);
    } 
    else
        delete_case4(n);
}
template <class T, class C>
void my::RBTree<T, C>::delete_case4(node *n) {
    node* s = n->sibling();
    if ((n->parent->color == RED) && (s->color == BLACK) && !s->has_red_child()) {
        s->color = RED;
        n->parent->color = BLACK;
    }
    else
        delete_case5(n);
}
template <class T, class C>
void my::RBTree<T, C>::delete_case5(node *n) {
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
template <class T, class C>
void my::RBTree<T, C>::delete_case6(node *n) {
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
template <class T, class C>
void my::RBTree<T, C>::insert_repair(node* x) {
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
template <class T, class C>
my::treeNode<T>* my::RBTree<T, C>::first() const {
    node* temp = root;
    if (temp != NULL) {
        while (temp->left != NULL)
            temp = temp->left;
    }
    return temp;
}
template <class T, class C>
my::treeNode<T>* my::RBTree<T, C>::last() const {
    node* temp = root;
    if (temp != NULL) {
        while (temp->right != NULL)
            temp = temp->right;
    }
    return temp;
}
template <class T, class C>
void my::RBTree<T, C>::delete_tree(node *n) {
    if (n == NULL) return;
    delete_tree(n->left);
    delete_tree(n->right);
    delete n;
}
template <class T, class C>
void my::RBTree<T, C>::copy_tree(node* &n, node* p, node*np) {
    if (p == NULL)
        return;
    n = new node(p->data);
    n->color = p->color; n->parent = np;
    copy_tree(n->left, p->left, n);
    copy_tree(n->right, p->right, n);
}
template <class T, class C> template <class P>
void my::RBTree<T, C>::inorder_print(node* root, const P& func) const {
    if (root != NULL) {
        inorder_print(root->left, func);
        func(root->data);
        inorder_print(root->right, func);
    }
}
template <class T, class C> template <class OutputIterator>
void my::RBTree<T, C>::inorder_copy(node* n, OutputIterator& first) const {
    if (n == NULL)
        return;
    inorder_copy(n->left, first);
    *first = n->data; ++first;
    inorder_copy(n->right, first);
}
template <class T, class C>
my::RBTree<T, C>::RBTree(const C& comp): root(NULL), size_(0), compare(comp) {}

template <class T, class C>
my::RBTree<T, C>::RBTree(const RBTree& t): root(NULL), size_(0), compare(t.compare) {
    copy_tree(root, t.root);
    size_ = t.size_;
}
#if __cplusplus >= 201103L
template <class T, class C>
my::RBTree<T, C>::RBTree(RBTree&& t): compare(t.compare) {
    root = t.root; t.root = NULL;
    size_ = t.size_; t.size_ = 0;
}
template <class T, class C>
void my::RBTree<T, C>::insert(std::initializer_list<T> l) {
    insert(l.begin(), l.end());
}
template <class T, class C>
my::RBTree<T, C>& my::RBTree<T, C>::operator=(std::initializer_list<T> l) {
    clear();
    insert(l.begin(), l.end());
    return *this;
}
template <class T, class C> 
my::RBTree<T, C>& my::RBTree<T, C>::operator=(RBTree&& t) {
    if (this != &t) {
        delete_tree(root);
        root = t.root; t.root = NULL;
        size_ = t.size_; t.size_ = 0;
    }
    return *this;
}
#endif
template <class T, class C> 
my::RBTree<T, C>& my::RBTree<T, C>::operator=(const RBTree& t) {
    clear();
    copy_tree(root, t.root);
    size_ = t.size_;
    return *this;
}
template <class T, class C> template <class InputIterator>
void my::RBTree<T, C>::insert(InputIterator first, InputIterator last) {
    for (; first != last; ++first)
        insert_help(new node(*first));
}
template <class T, class C>
void my::RBTree<T, C>::erase(const_iterator it) {
    if (root == NULL || it.hd == NULL)
        return;
    delete_node(it.hd);
    --size_;
}
template <class T, class C>
void my::RBTree<T, C>::erase(const_iterator first, const_iterator last) {
    if (root == NULL || first.hd == NULL)
        return;
    node* temp;
    while (first != last) {
        temp = first.hd;
        ++first;
        delete_node(temp);
        --size_;
    }
}
template <class T, class C>
void my::RBTree<T, C>::swap(RBTree& x) {
    node* t_root = root; root = x.root; x.root = t_root;
    size_t t_size_ = size_; size_ = x.size_; x.size_ = t_size_;
}

#endif
