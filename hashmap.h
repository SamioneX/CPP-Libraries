#ifndef HASHMAP_H
#define HASHMAP_H

#include <iostream>
#include <stdexcept>

namespace my{
    /*----------get default size for table----------------*/
    template <class T> size_t default_size() {return 100000;}
    template <> size_t default_size<char>() {return 95;}
    struct hash {
        template<class T> size_t operator()(T t, size_t size);
    };
    template <> size_t hash::operator() (std::string, size_t);
    template <> size_t hash::operator() (const char* s, size_t);
    template <> size_t my::hash::operator() (double d, size_t size);
    template <> size_t my::hash::operator() (long double d, size_t size);
    template <> size_t hash::operator() (float, size_t);
    template <> size_t hash::operator() (char, size_t);
    template<class T> struct equals;
    template <class T> struct HashNode;
    template <class K, class V, class C = equals<K>, class H = hash> class map;
}
template<class T> struct my::equals {
    bool operator()(const T& lhs, const T& rhs) const {
        return lhs == rhs;
    }
};
/*-------------------for integer data types------------------*/
template <class T> size_t my::hash::operator() (T t, size_t size) {
    return (size_t)t % size;
}
template <> size_t my::hash::operator() (double d, size_t size) {
    d *= 100; return (size_t)d % size;
}
template <> size_t my::hash::operator() (long double d, size_t size) {
    d *= 100; return (size_t)d % size;
}
template <> size_t my::hash::operator() (float f, size_t size) {
    f *= 100; return (size_t)f % size;
}
template <> size_t my::hash::operator() (std::string s, size_t size) {
    const size_t p = 31;
    size_t hash_value = 0;
    unsigned long long p_pow = 1;
    for (char c : s) {
        hash_value = (hash_value + (c - ' ' + 1) * p_pow) % size;
        p_pow = (p_pow * p) % size;
    }
    return hash_value;
}
template <> size_t my::hash::operator() (const char* s, size_t size) {
    const size_t p = 31;
    size_t hash_value = 0;
    unsigned long long p_pow = 1;
    for (int i = 0; s[i] != '\0'; i++) {
        hash_value = (hash_value + (s[i] - ' ' + 1) * p_pow) % size;
        p_pow = (p_pow * p) % size;
    }
    return hash_value;
}
template <> size_t my::hash::operator() (char s, size_t size) {
    return (size_t)(s - ' ') % size;
}
template <class T>
struct my::HashNode {
    T data;
    HashNode* next;
    HashNode(T t): data(t), next(NULL) {}
};
template <class K, class V, class C, class H>
class my::map {
    template <class T> class map_iterator {
        public:
        typedef T value_type;
        typedef T& reference;
        typedef T* pointer;
        typedef std::forward_iterator_tag iterator_category;
        typedef void difference_type;
        map_iterator(map* a = NULL, size_t i = 0, HashNode<std::pair<const K, V> >* h = NULL) {m = a; index = i; p = h;}
        map_iterator& operator=(const map_iterator& it) {m = it.m; index = it.index; p = it.p;}
        bool operator==(const map_iterator& it) {return p == it.p;}
        bool operator!=(const map_iterator& it) {return p != it.p;}
        map_iterator& operator++() {
            if (p->next != NULL)
                p = p->next;
            else {
                ++index;
                while (index < m->size_ && m->arr[index] == NULL) ++index;
                p = (index == m->size_)? NULL: m->arr[index];
            }
            return *this;
        }
        map_iterator operator++(int) {map_iterator it = *this; ++*this; return it;}
        reference operator*() {return p->data;}
        pointer operator->() {return &(p->data);}

        private:
        map* m;
        size_t index;
        HashNode<std::pair<const K, V> >* p;
    };
    void copy(const map& m) {
        for (size_t i = 0; i < m.size_; ++i) {
            HashNode<value_type>* temp = m.arr[i];
            while (temp != NULL) {
                insert(temp->data);
                temp = temp->next;
            }
        }
    }
    public:
    typedef std::pair<const K, V> value_type;
    typedef map_iterator<value_type> iterator;
    typedef map_iterator<const value_type> const_iterator;

    map(size_t size, const C& compfunc = C(), const H& hashfunc = H()): size_(size), count(0) {
        hash = hashfunc;
        comp = compfunc;
        arr = new HashNode<value_type>*[size];
        for (size_t i = 0; i < size; ++i)
            arr[i] = NULL;
    }
    map(const C& compfunc = C(), const H& hashfunc = H()): map(default_size<K>(), compfunc, hashfunc) {}

    map(std::initializer_list<value_type> l): map(default_size<K>(), C(), H()) {
        insert(l);
    }
    template <class InputIterator>
    map(InputIterator first, InputIterator last): map(default_size<K>(), C(), H()) {
        insert(first, last);
    }
    map(const map& m): map(default_size<K>(), C(), H()) {
        copy(m);
    }
    ~map() {clear(); delete [] arr;}

    std::pair<value_type*, bool> insert(const value_type& p) {
        size_t index = hash(p.first, size_);
        if (arr[index] == NULL) {
            arr[index] = new HashNode<value_type>(p);
            ++count;
            return std::pair<value_type*, bool>(&(arr[index]->data), true);
        }
        else {
            HashNode<value_type>* temp = arr[index], *prev;
            while (temp != NULL) {
                if (comp(temp->data.first, p.first))
                    return std::pair<value_type*, bool>(&(temp->data), false);
                prev = temp;
                temp = temp->next;
            }
            prev->next = new HashNode<value_type>(p);
            ++count;
            return std::pair<value_type*, bool>(&(prev->next->data), true);
        }
    }
    template <class InputIterator>
    void insert(InputIterator first, InputIterator last) {
        for (; first != last; ++first)
            insert(*first);
    }
    void insert(std::initializer_list<value_type> l) {
        for (auto it = l.begin(); it != l.end(); it++)
            insert(*it);
    }
    std::pair<value_type*, bool> emplace(const K& key, V val) {
        return insert(value_type(key, val));
    }
    void erase(const K& val) {
        size_t index = hash(val, size_);
        if (arr[index] != NULL) {
            HashNode<value_type>* temp = arr[index], *prev;
            if (comp(temp->data.first, val)) {
                arr[index] = arr[index]->next;
                delete temp; count--; return;
            }
            while (temp != NULL && !comp(temp->data.first, val)) { 
                prev = temp;
                temp = temp->next; 
            }
            if (temp == NULL) return; 
            prev->next = temp->next;
            delete temp; count--;
        }
    }
    value_type* find(K val) {
        size_t index = hash(val, size_);
        if (arr[index] != NULL) {
            HashNode<value_type>* temp = arr[index];
            while (temp != NULL) {
                if (comp(temp->data.first, val))
                    return &(temp->data);
                temp = temp->next; 
            }
        }
        return NULL;
    }
    V& at(const K& val) {
        value_type* p = find(val);
        if (p == NULL) throw std::out_of_range("map::at() Key not found!");
        return p->second;
    }
    V& operator[](const K& val) {
        value_type* p = find(val);
        if (p != NULL)
            return p->second;
        return insert(value_type(val, V())).first->second;
    }
    map& operator=(std::initializer_list<value_type> l) {
        clear();
        insert(l);
        return *this;
    }
    map& operator=(const map& m) {
        clear();
        copy(m);
        return *this;
    }
    template <class InputIterator>
    void assign(InputIterator first, InputIterator last) {
        clear();
        insert(first, last);
    }
    size_t size() {return count;}

    bool empty() {return count == 0;}

    void clear() {
        if (count != 0) {
            for (size_t i = 0; i < size_; i++) {
                if (arr[i] != NULL) {
                    HashNode<value_type>* temp = arr[i], *next;
                    while (temp != NULL) {
                        next = temp->next;
                        delete temp;
                        temp = next;
                    }
                    arr[i] = NULL;
                }
            }
        }
        count = 0;
    }
    iterator begin() {
        if (arr[0] != NULL)
            return iterator(this, 0, arr[0]);
        size_t i = 0;
        while (i < size_ && arr[i] == NULL) ++i;
        if (i == size_) return iterator();
        return iterator(this, i, arr[i]);
    }
    iterator end() {return iterator();}
    const_iterator cbegin() {
        if (arr[0] != NULL)
            return const_iterator(this, 0, arr[0]);
        size_t i = 0;
        while (i < size_ && arr[i] == NULL) ++i;
        if (i == size_) return const_iterator();
        return const_iterator(this, i, arr[i]);
    }
    const_iterator cend() {return const_iterator();}

    private:
    HashNode<value_type>** arr;
    size_t count;
    size_t size_;
    H hash;
    C comp;
};

#endif
