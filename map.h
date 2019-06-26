#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <stdexcept>
#include "RBTree.h"
#include "utilities.h"

namespace my {
    template <class K, class V>
    struct print {
        void operator()(const std::pair<const K, V>& p) const {
            std::cout << p.first << '\t' << p.second << '\n';
        }
    };
    template <class K, class V, class C>
    class mapClass: virtual public my::RBTree<std::pair<const K, V>, C> {
        protected:
        typedef std::pair<const K, V> value_type;
        typedef treeNode<value_type> node;
        typedef my::RBTree<value_type, C> T;
        typedef class T::iterator iterator;
        node* search(node* root, const K& key) {
            if (root == NULL)
                return root;
            if (this->compare(key, root->data.first))
                return search(root->left, key);
            else if (this->compare(root->data.first, key))
                return search(root->right, key);
            else
                return root;
        }
        node* doupper(node* n, const K& key) const {
            if (n == NULL)
                return n;
            if (this->compare(n->data.first, key))
                return doupper(n->right, key);
            if (this->compare(key, n->data.first)) {
                node* temp = doupper(n->left, key);
                return (temp == NULL)? n: temp;
            }
            return doupper(n->right, key);
        }
        node* dolower(node* n, const K& key) const {
            if (n == NULL)
                return n;
            if (this->compare(n->data.first, key))
                return dolower(n->right, key);
            if (this->compare(key, n->data.first)) {
                node* temp = dolower(n->left, key);
                return (temp == NULL)? n: temp;
            }
            return n;
        }
        public:
        using T::erase;
        class value_compare {
            friend class mapClass;
            protected:
            C comp;
            value_compare (C compare) : comp(compare) {}
            public:
            typedef bool result_type;
            typedef K first_argument_type;
            typedef V second_argument_type;
            bool operator() (const value_type& x, const value_type& y) const {
                return comp(x.first, y.first);
            }
        };
        value_compare value_comp() const {return value_compare(this->compare);}
        void erase(const K& val) {
            node* n = search(this->root, val);
            if (n == NULL)
                return;
            this->delete_node(n);
            --this->size_;
        }
        iterator find(const K& key) {return iterator(search(this->root, key));}
        template <class P>
        void print(P func) {this->inorder_print(this->root, func);}
        void print() {this->inorder_print(this->root, my::print<K, V>());}
        iterator lower_bound(const K& key) const {return iterator(dolower(this->root, key));}
        iterator upper_bound(const K& key) const {return iterator(doupper(this->root, key));}
        std::pair<iterator, iterator> equal_range(const K& key) const {
            return std::pair<iterator, iterator>(lower_bound(), upper_bound());
        }
    };
    template <class K, class V, class C = less<const K> >
    class map: public mapClass<K, V, C> {
        typedef my::RBTree<std::pair<const K, V>, C> T;
        typedef treeNode<std::pair<const K, V> > node;

        public:
        typedef std::pair<const K, V> value_type;
        typedef class mapClass<K, V, C>::value_compare value_compare;
        typedef C key_compare;
        typedef class T::iterator iterator;
        typedef class T::const_iterator const_iterator;
        typedef class T::reverse_iterator reverse_iterator;
        typedef class T::const_reverse_iterator const_reverse_iterator;
        using T::insert;

        explicit map(const C& comp = C()): T(comp) {}
        template <class InputIterator>
        map(InputIterator first, InputIterator last): T() {T::insert(first, last);}
        #if __cplusplus >= 201103L
        map(std::initializer_list<value_type> l): T() { T::insert(l);}
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
        V& at(const K& key) {
            node* n = this->search(this->root, key);
            if (n == NULL)
                throw std::out_of_range("my::map::range_check. Key not found\n");
            return n->data.second;
        }
        V& operator[](const K& key) {
            node* n = this->search(this->root, key);
            if (n == NULL) {
                return insert(value_type(key, V())).first->second;
            }
            return n->data.second;
        }
        size_t count(const K& key) {
            if (this->search(this->root, key) == NULL) return 0;
            return 1;
        }
        protected:
        node* BSTInsert(node* root, node* n, node*& s) {
            if (root == NULL)
                return n;
            if (this->compare(n->data.first, root->data.first)) {
                root->left  = BSTInsert(root->left, n, s); 
                root->left->parent = root;
            }
            else if (this->compare(root->data.first, n->data.first)) {
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
    template <class K, class V, class C = less<const K> >
    class multimap: public mapClass<K, V, C> {
        typedef my::RBTree<std::pair<const K, V>, C> T;
        typedef treeNode<std::pair<const K, V> > node;

        public:
        typedef std::pair<const K, V> value_type;
        typedef class mapClass<K, V, C>::value_compare value_compare;
        typedef C key_compare;
        typedef class T::iterator iterator;
        typedef class T::const_iterator const_iterator;
        typedef class T::reverse_iterator reverse_iterator;
        typedef class T::const_reverse_iterator const_reverse_iterator;
        using T::insert;

        explicit multimap(const C& comp = C()): T(comp) {}
        template <class InputIterator>
        multimap(InputIterator first, InputIterator last): T() {T::insert(first, last);}
        #if __cplusplus >= 201103L
        multimap(std::initializer_list<value_type> l): T() { T::insert(l);}
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
            if (this->compare(n->data.first, root->data.first)) {
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
            if (this->compare(n->data.first, key))
                count_helper(n->right, key, i);
            else if (this->compare(key, n->data.first))
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
