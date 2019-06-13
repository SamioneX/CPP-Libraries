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
    template<class T, class U> struct pair;
    template<class K, class V> struct HashNode;
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
    return operator()((long double)d, size);
}
template <> size_t my::hash::operator() (long double d, size_t size) {
    d = (d < 0)? -d: d;
    d *= 100;
    return (size_t)d % size;
}
template <> size_t my::hash::operator() (float f, size_t size) {
    return operator()((long double)f, size);
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
template <> size_t my::hash::operator()<const char*>(const char* s, size_t size) {
    const size_t p = 31;
    size_t hash_value = 0;
    unsigned long long p_pow = 1;
    for (int i = 0; s[i] != '\0'; i++) {
        hash_value = (hash_value + (s[i] - ' ' + 1) * p_pow) % size;
        p_pow = (p_pow * p) % size;
    }
    return hash_value;
}
template <> size_t my::hash::operator()<char> (char s, size_t size) {
    return (size_t)(s - ' ') % size;
}
template<class T, class U>
struct my::pair {
    T first;
    U second;
    pair(T val1 = T(), U val2 = U()): first(val1), second(val2) {}
    pair(const pair& p): first(p.first), second(p.second) {}
    pair(std::initializer_list<void*> l) {
        auto it = l.begin();
        first = *((T*)(*it));
        it++;
        second = *((U*)(*it));
    }
    pair& operator=(const pair& p) {
        first = p.first; second = p.second; return *this;
    }
};
template<class K, class V>
struct my::HashNode {
    pair<K, V> data;
    HashNode* next;
    HashNode(pair<K, V> p): data(p), next(NULL) {}
};
template <class K, class V, class C, class H>
class my::map {
    private:
    HashNode<K,V>** arr;
    size_t count;
    size_t size_;
    H hash;
    C comp;

    public:
    map(size_t size, const C& compfunc = C(), const H& hashfunc = H()): size_(size), count(0) {
        hash = hashfunc;
        comp = compfunc;
        arr = new HashNode<K,V>*[size];
        for (size_t i = 0; i < size; ++i)
            arr[i] = NULL;
    }
    map(const C& compfunc = C(), const H& hashfunc = H()): map(default_size<K>(), compfunc, hashfunc) {}

    map(std::initializer_list<my::pair<K, V> > l): map(default_size<K>(), C(), H()) {
        insert(l);
    }
    template <class InputIterator>
    map(InputIterator first, InputIterator last): map(default_size<K>(), C(), H()) {
        insert(first, last);
    }
    ~map() {clear(); delete [] arr;}

    pair<pair<K,V>*, bool> insert(my::pair<K, V> p) {
        size_t index = hash(p.first, size_);
        if (arr[index] == NULL) {
            arr[index] = new HashNode<K, V>(p);
            ++count;
            return pair<pair<K,V>*, bool>(&(arr[index]->data), true);
        }
        else {
            HashNode<K, V>* temp = arr[index], *prev;
            while (temp != NULL) {
                if (comp(temp->data.first, p.first))
                    return pair<pair<K,V>*, bool>(&(temp->data), false);
                prev = temp;
                temp = temp->next;
            }
            prev->next = new HashNode<K, V>(p);
            ++count;
            return pair<pair<K,V>*, bool>(&(prev->next->data), true);
        }
    }
    template <class InputIterator>
    void insert(InputIterator first, InputIterator last) {
        for (; first != last; ++first)
            insert(*first);
    }
    void insert(std::initializer_list<my::pair<K, V> > l) {
        for (auto it = l.begin(); it != l.end(); it++)
            insert(*it);
    }
    pair<pair<K,V>*, bool> emplace(K key, V val) {
        return insert(pair<K, V>(key, val));
    }
    void erase(K val) {
        size_t index = hash(val, size_);
        if (arr[index] != NULL) {
            HashNode<K, V>* temp = arr[index], *prev;
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
    pair<K,V>* find(K val) {
        size_t index = hash(val, size_);
        if (arr[index] != NULL) {
            HashNode<K, V>* temp = arr[index];
            while (temp != NULL) {
                if (comp(temp->data.first, val))
                    return &(temp->data);
                temp = temp->next; 
            }
        }
        return NULL;
    }
    V& at(K val) {
        pair<K,V>* p = find(val);
        if (p == NULL) throw std::out_of_range("map::at() Key not found!");
        return p->second;
    }
    V& operator[](K val) {
        pair<K,V>* p = find(val);
        if (p != NULL)
            return p->second;
        return insert(pair<K,V>(val)).first->second;
    }
    map& operator=(std::initializer_list<my::pair<K, V> > l) {
        clear();
        insert(l);
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
                    HashNode<K, V>* temp = arr[i], *next;
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
};

#endif
