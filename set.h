#ifndef SET_H
#define SET_H

#include <iostream>
#include <stdexcept>
#include "RBTree.h"
#include "utilities.h"

namespace my {
    template <class T>
    struct print {
        void operator()(const T& t) const {
            std::cout << t << ' ';
        }
    };
    template <class T, class C>
    class setClass: virtual public my::RBTree<const T, C> {
        protected:
        typedef treeNode<const T> node;
        typedef class my::RBTree<const T, C>::iterator iterator;
        node* search(node* root, const T& key) {
            if (root == NULL)
                return root;
            if (this->compare(key, root->data))
                return search(root->left, key);
            else if (this->compare(root->data, key))
                return search(root->right, key);
            else
                return root;
        }
        node* doupper(node* n, const T& key) const {
            if (n == NULL)
                return n;
            if (this->compare(n->data, key))
                return doupper(n->right, key);
            if (this->compare(key, n->data)) {
                node* temp = doupper(n->left, key);
                return (temp == NULL)? n: temp;
            }
            return doupper(n->right, key);
        }
        node* dolower(node* n, const T& key) const {
            if (n == NULL)
                return n;
            if (this->compare(n->data, key))
                return dolower(n->right, key);
            if (this->compare(key, n->data)) {
                node* temp = dolower(n->left, key);
                return (temp == NULL)? n: temp;
            }
            return n;
        }
        public:
        using my::RBTree<const T, C>::erase;
        C value_comp() const {return value_compare(this->compare);}
        void erase(const T& val) {
            node* n = search(this->root, val);
            if (n == NULL)
                return;
            this->delete_node(n);
            --this->size_;
        }
        iterator find(const T& key) {return iterator(search(this->root, key));}
        template <class P>
        void print(P func) {this->inorder_print(this->root, func);}
        void print() {this->inorder_print(this->root, my::print<T>());}
        iterator lower_bound(const T& key) const {return iterator(dolower(this->root, key));}
        iterator upper_bound(const T& key) const {return iterator(doupper(this->root, key));}
        std::pair<iterator, iterator> equal_range(const T& key) const {
            return std::pair<iterator, iterator>(lower_bound(), upper_bound());
        }
    };
    template <class K, class C = less<const K> >
    class set: public setClass<K, C> {
        typedef my::RBTree<const K, C> T;
        typedef treeNode<const K> node;

        public:
        typedef K value_type;
        typedef C value_compare;
        typedef C key_compare;
        typedef class T::iterator iterator;
        typedef class T::const_iterator const_iterator;
        typedef class T::reverse_iterator reverse_iterator;
        typedef class T::const_reverse_iterator const_reverse_iterator;
        using T::insert;

        explicit set(const C& comp = C()): T(comp) {}
        template <class InputIterator>
        set(InputIterator first, InputIterator last): T() {T::insert(first, last);}
        #if __cplusplus >= 201103L
        set(std::initializer_list<const K> l): T() { T::insert(l);}
        template <class... Args>
        std::pair<iterator, bool> emplace(Args&&... args) {return insert(T(args...));}
        #endif

        std::pair<iterator, bool> insert(const value_type& val) {
            node *pt = new node(val);
            node* s = NULL;
            this->root = BSTInsert(this->root, pt, s);
            if (s == NULL) {
                this->insert_repair(pt);
                ++this->size_;
                return std::pair<iterator, bool>(iterator(pt), true);
            }
            delete pt;
            return std::pair<iterator, bool>(iterator(s), false);
        }
        size_t count(const value_type& key) {
            if (this->search(this->root, key) == NULL) return 0;
            return 1;
        }
        protected:
        node* BSTInsert(node* root, node* n, node*& s) {
            if (root == NULL)
                return n;
            if (this->compare(n->data, root->data)) {
                root->left  = BSTInsert(root->left, n, s); 
                root->left->parent = root;
            }
            else if (this->compare(root->data, n->data)) {
                root->right = BSTInsert(root->right, n, s);
                root->right->parent = root; 
            }
            else
                s = root;
            return root;
        }
        void insert_help(node* pt) {
            node* s = NULL;
            this->root = BSTInsert(this->root, pt, s);
            if (s == NULL) {
                this->insert_repair(pt);
                ++this->size_;
            }
            else
                delete pt;
        }
    };
    template <class K, class C = less<const K> >
    class multiset: public setClass<K, C> {
        typedef my::RBTree<const K, C> T;
        typedef treeNode<const K> node;

        public:
        typedef K value_type;
        typedef C value_compare;
        typedef C key_compare;
        typedef class T::iterator iterator;
        typedef class T::const_iterator const_iterator;
        typedef class T::reverse_iterator reverse_iterator;
        typedef class T::const_reverse_iterator const_reverse_iterator;
        using T::insert;

        explicit multiset(const C& comp = C()): T(comp) {}
        template <class InputIterator>
        multiset(InputIterator first, InputIterator last): T() {T::insert(first, last);}
        #if __cplusplus >= 201103L
        multiset(std::initializer_list<const K> l): T() { T::insert(l);}
        template <class... Args>
        iterator emplace(Args&&... args) {return insert(T(args...));}
        #endif

        iterator insert(const value_type& val) {
            node *pt = new node(val);
            this->root = BSTInsert(this->root, pt);
            this->insert_repair(pt);
            ++this->size_;
            return iterator(pt);
        }
        size_t count(const K& key) {size_t i(0); count_helper(this->root, key, i); return i;}
        
        protected:
        node* BSTInsert(node* root, node* n) {
            if (root == NULL)
                return n;
            if (this->compare(n->data, root->data)) {
                root->left  = BSTInsert(root->left, n); 
                root->left->parent = root;
            }
            else {
                root->right = BSTInsert(root->right, n);
                root->right->parent = root; 
            }
            return root;
        }
        void insert_help(node* pt) {
            this->root = BSTInsert(this->root, pt);
            this->insert_repair(pt);
            ++this->size_;
        }
        void count_helper(node* n, const K& key, size_t& i) {
            if (n == NULL)
                return;
            if (this->compare(n->data, key))
                count_helper(n->right, key, i);
            else if (this->compare(key, n->data))
                count_helper(n->left, key, i);
            else {
                ++i;
                count_helper(n->left, key, i);
                count_helper(n->right, key, i);
            }
        }
    };
}

#endif
