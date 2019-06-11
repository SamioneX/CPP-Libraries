#ifndef HASHMAP_H
#define HASHMAP_H

#include <iostream>
#include <stdexcept>

namespace my{
    const size_t MAX_SIZE = 100000;
    struct hash {
        template<class T> size_t operator()(T s, size_t size);
    };
    template <> size_t hash::operator()<std::string> (const std::string, size_t);
    template <> size_t hash::operator()<const char*> (const char*, size_t);
    template <> size_t hash::operator()<char> (char, size_t);

    template<class T> struct equals {
        bool operator()(const T& lhs, const T& rhs) const {
            return lhs == rhs;
        }
    };
    template<class T, class U> struct pair;
    template<class K, class V> struct HashNode;
    template<class K, class V, class C, class H> class MapClass;
    template <class K, class V, class C = equals<K>, class H = hash>
    class map: public MapClass<K, V, C, H> {
        public:
        map(const C& comp = C(), size_t size = MAX_SIZE, const H& hashfunc = H()):
        MapClass<K, V, C, H>(comp, size, hashfunc) {}

        map(std::initializer_list<my::pair<K, V> > l, const C& comp = C(), size_t size = MAX_SIZE, const H& hashfunc = H()):
        MapClass<K, V, C, H>(comp, size, hashfunc) {
            MapClass<K, V, C, H>::insert(l);
        }
        template <class InputIterator>
        map(InputIterator first, InputIterator last, const C& comp = C(), size_t size = MAX_SIZE, const H& hashfunc = H()):
        MapClass<K, V, C, H>(comp, size, hashfunc) {
            MapClass<K, V, C, H>::insert(first, last);
        }
    };
    template <class V> class map<char, V, equals<char>, hash>: public MapClass<char, V, equals<char>, hash> {
        public:
        map(size_t size = 95):
        MapClass<char, V, equals<char>, hash>(equals<char>(), size, hash()) {}

        map(std::initializer_list<my::pair<char, V> > l, size_t size = 95):
        MapClass<char, V, equals<char>, hash>(equals<char>(), size, hash()) {
            MapClass<char, V, equals<char>, hash>::insert(l);
        }
        template <class InputIterator>
        map(InputIterator first, InputIterator last, size_t size = 95):
        MapClass<char, V, equals<char>, hash>(equals<char>(), size, hash()) {
            MapClass<char, V, equals<char>, hash>::insert(first, last);
        }
    };
}
template <class T> size_t my::hash::operator() (T s, size_t size) {
    return operator()(std::to_string(s), size);
}
template <> size_t my::hash::operator()<std::string> (const std::string s, size_t size) {
    const size_t p = 31;
    size_t hash_value = 0;
    size_t p_pow = 1;
    for (char c : s) {
        hash_value = (hash_value + (c - ' ' + 1) * p_pow) % size;
        p_pow = (p_pow * p) % size;
    }
    std::cout << "hash value is " << hash_value << "\n";
    return hash_value;
}
template <> size_t my::hash::operator()<const char*>(const char* s, size_t size) {
    const size_t p = 31;
    size_t hash_value = 0;
    size_t p_pow = 1;
    for (int i = 0; s[i] != '\0'; i++) {
        hash_value = (hash_value + (s[i] - ' ' + 1) * p_pow) % size;
        p_pow = (p_pow * p) % size;
    }
    return hash_value;
}
template <> size_t my::hash::operator()<char> (char s, size_t size) {
    return (unsigned long)(s - ' ') % size;
}
template<class T, class U>
struct my::pair {
    T first;
    U second;
    pair(T val1 = T(), U val2 = U()): first(val1), second(val2) {}
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
    HashNode(K k, V v = V()): next(NULL) {
        data.first = k; data.second = v;
    }
    HashNode(pair<K, V> p): next(NULL) {
        data = p;
    }
};
template<class K, class V, class C, class H>
class my::MapClass {
    private:
    HashNode<K,V>** arr;
    size_t count;
    size_t size_;
    H hash;
    C comp;
    
    public:
    MapClass(C compfunc, size_t size, H hashfunc): count(0), size_(size) {
        hash = hashfunc;
        comp = compfunc;
        arr = new HashNode<K,V>*[size];
        for (size_t i = 0; i < size; ++i)
            arr[i] = NULL;
    }
    ~MapClass() {clear(); delete [] arr;}

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
                    return my::pair<pair<K,V>*, bool>(&(temp->data), false);
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
        if (p == NULL) throw std::invalid_argument(std::string("Invalid key in call to map::at(") + val + ')');
        return p->second;
    }
    V& operator[](K val) {
        pair<K,V>* p = find(val);
        if (p == NULL) throw std::invalid_argument(std::string("Invalid key in call to map::[") + val + ']');
        return p->second;
    }
    MapClass& operator=(std::initializer_list<my::pair<K, V> > l) {
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
