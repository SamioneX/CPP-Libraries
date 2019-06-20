#ifndef LIST_H
#define LIST_H

#include <iostream>
#include <stdexcept>
#include "utilities.h"

namespace my {
    template <class T> class node;
    template <class T> class list;
    template<class T> std::ostream& operator<< (std::ostream& os, const list<T>& v);
}
template <class T>
class my::node {
    public:
    T data;
    node<T>* next;
    node<T>* prev;
    node(): next(NULL), prev(NULL) {}
    node(T val): data(val), next(NULL), prev(NULL) {}
};
template <class T>
class my::list {
    friend std::ostream& operator<< <> (std::ostream& os, const list<T>& v);
    template <class U, class V>
    class list_iterator {
        friend class list;
        public:
            typedef U value_type;
            typedef U& reference;
            typedef U* pointer;
            typedef std::bidirectional_iterator_tag iterator_category;
            typedef void difference_type;
            list_iterator(V p): hd(p) {}
            list_iterator& operator=(const list_iterator& it) {hd = it.hd; return *this;} 
            bool operator==(const list_iterator& it) {return hd == it.hd;}
            bool operator!=(const list_iterator& it) {return hd != it.hd;}
            reference operator*() {return hd->data;}
            pointer operator->() { return &(hd->data);}
        protected:
            V hd;
    };
    private:
        node<T> *head;
        node<T> *tail;
        size_t count;
        
        void append(node<T>* newNode);
        void remove(node<T>* temp);
        void insert(node<T>* pTemp, node<T>* start, node<T>* stop);
        template <class func>
        node<T>* merge(node<T> *first, node<T> *second, func compare);
        node<T>* split(node<T>* hd);
        template <class func> node<T>* mergeSort(node<T>* hd, func compare);
        template <class func> void storeTail(node<T>* theNode, func compare);
        template <class func> void doUnique(func compare);
        
    public:
        class iterator : public list_iterator<T, node<T>* > {
            public:
            iterator(node<T>* p = NULL): list_iterator<T, node<T>*>(p) {}
            iterator& operator++() {this->hd = this->hd->next; return *this;}
            iterator operator++(int) {iterator it(this->hd); ++*this; return it;}
            iterator& operator--() {this->hd = this->hd->prev; return *this;}
            iterator operator--(int) {iterator it(this->hd); --*this; return it;}
        };
        class reverse_iterator : public list_iterator<T, node<T>*> {
            public:
            reverse_iterator(node<T>* p = NULL): list_iterator<T, node<T>*>(p) {}
            reverse_iterator& operator++() {this->hd = this->hd->prev; return *this;}
            reverse_iterator operator++(int) {reverse_iterator it(this->hd); ++*this; return it;}
            reverse_iterator& operator--() {this->hd = this->hd->next; return *this;}
            reverse_iterator operator--(int) {reverse_iterator it(this->hd); --*this; return it;}
        };
        class const_iterator : public list_iterator<const T, const node<T>*>{
            public:
            const_iterator(const node<T>* p = NULL): list_iterator<const T, const node<T>*>(p) {}
            const_iterator& operator++() {this->hd = this->hd->next; return *this;}
            const_iterator operator++(int) {const_iterator it(this->hd); ++*this; return it;}
            const_iterator& operator--() {this->hd = this->hd->prev; return *this;}
            const_iterator operator--(int) {const_iterator it(this->hd); --*this; return it;}
        };
        class const_reverse_iterator : public list_iterator<const T, const node<T>*> {
            public:
            const_reverse_iterator(const node<T>* p = NULL): list_iterator<const T, const node<T>*>(p) {}
            const_reverse_iterator& operator++() {this->hd = this->hd->prev; return *this;}
            const_reverse_iterator operator++(int) {const_reverse_iterator it(this->hd); ++*this; return it;}
            const_reverse_iterator& operator--() {this->hd = this->hd->next; return *this;}
            const_reverse_iterator operator--(int) {const_reverse_iterator it(this->hd); --*this; return it;}
        };
        list();
        list(size_t n, T val = T());
        list(std::initializer_list<T> l);
        list(const list& l);
        template<class InputIterator>
        list(InputIterator begin, InputIterator end);
        ~list() {clear();}
        void operator=(const list& l);
        void operator=(std::initializer_list<T> l);
        void assign(size_t n, T val);
        template<class InputIterator>
        void assign(InputIterator begin, InputIterator end);
        T& at(size_t val);
        T& operator[](size_t val) {return this->at(val);}
        T& front();
        T& back();
        void push_back(const T& val);
        void push_front(const T& val);
        void pop_front() {remove(head);}
        void pop_back() {remove(tail);}
        template<class InputIterator>
        void copy(InputIterator begin, InputIterator end);
        iterator insert(iterator it, const T& val, size_t n = 1);
        template<class InputIterator>
        void insert(iterator it, InputIterator begin, InputIterator end);
        void insert(iterator it, std::initializer_list<T> l);
        template <class... Args>
        iterator emplace(iterator it, Args&&... args) {return insert(it, T(args...));}
        template <class... Args>
        void emplace_back(Args&&... args) {push_back(T(args...));}
        template <class... Args>
        void emplace_front(Args&&... args) {push_front(T(args...));}
        void erase(iterator iter) {remove(iter.hd);}
        void erase(iterator begin, iterator end);
        void removeAll(const T& val);
        template <class func>
        void remove_if(func compare);
        size_t size() const {return count;}
        void resize(size_t v, T val = T());
        bool empty() {return (head == NULL);}
        void sort () {head = mergeSort(head, my::less<T>());}
        template <class func>
        void sort(func compare) {head = mergeSort(head, compare);}
        void unique() {doUnique(my::equals<T>());}
        template <class func>
        void unique(func compare) {doUnique(compare);}
        iterator find(iterator begin, iterator end, T val);
        void clear();
        void reverse();
        void swap(list& l);
        void splice(iterator it, list& l) {splice(it, l, l.begin(), l.end());}
        void splice(iterator it, list& l, iterator i) {splice(it, l, i, ++i);}
        void splice (iterator it, list& l, iterator begin, iterator end) {
            insert(it, begin, end); l.erase(begin, end);
        }
        iterator begin() const {return iterator(head);}
        iterator end() const {return iterator(NULL);}
        const_iterator cbegin() const {return const_iterator(head);}
        const_iterator cend() const {return const_iterator(NULL);}
        reverse_iterator rbegin() const {return reverse_iterator(tail);}
        reverse_iterator rend() const {return reverse_iterator(NULL);}
        const_reverse_iterator crbegin() const {return const_reverse_iterator(tail);}
        const_reverse_iterator crend() const {return const_reverse_iterator(NULL);}
};
/*---------------------------CONSTRUCTORS & DESTRUCTOR---------------------*/
template <class T>
my::list<T>::list(): head(NULL), tail(NULL), count(0) {}

template <class T>
my::list<T>::list(size_t n, T val): list() {
    for (int i = 0; i < n; i++) {
      node<T>* pNew = new node<T>(val);
      append(pNew);
    }
}
template <class T>
my::list<T>::list(std::initializer_list<T> l): list() {copy(l.begin(), l.end());}

template <class T>
my::list<T>::list(const list& l): list() {
    node<T>* temp = l.head;
    while (temp != NULL) {
        node<T>* pNew = new node<T>(temp->data);
        append(pNew);
        temp = temp->next;
    }
}
template <class T> template<class InputIterator>
my::list<T>::list(InputIterator begin, InputIterator end): list() {copy(begin, end);}

/*--------------------------------Assignment Operators-----------------------------*/
template <class T>
void my::list<T>::operator=(const list& l) {
    clear();
    node<T>* temp = l.head;
    while (temp != NULL) {
        node<T>* pNew = new node<T>(temp->data);
        append(pNew);
        temp = temp->next;
    }
}
template <class T>
void my::list<T>::operator=(std::initializer_list<T> l) {clear(); copy(l.begin(), l.end());}

template <class T>
void my::list<T>::assign(size_t n, T val) {
    clear();
    for (size_t i = 0; i < n; i++)
        push_back(val);
}
template <class T> template<class InputIterator>
void my::list<T>::assign(InputIterator begin, InputIterator end) {
    clear();
    copy(begin, end);
}

/*-------------------------------GET FUNCTIONS------------------------------------*/
template <class T>
T& my::list<T>::at(size_t val) {
    if (val >= count) throw std::out_of_range("my::list::range_check");
    node<T>* temp;
    size_t j;
    if (val < count/2) {
        temp = head;
        for (j = 0; j < val; j++)
            temp = temp->next;
    }
    else {
        temp = tail; val++;
        for (j = count; j > val; j--)
            temp = temp->prev;
    }
    return temp->data;
}
template <class T>
T& my::list<T>::front() {
    if (count == 0) throw std::out_of_range("my::list::range_check. this->size is 0.");
    return head->data;
}   
template <class T>
T& my::list<T>::back() {
    if (count == 0) throw std::out_of_range("my::list::range_check. this->size is 0.");
    return tail->data;
}
/*-------------INSERT FUNCTIONS-------------*/
template <class T>
void my::list<T>::append(node<T>* pNew) {
    if (head == NULL) {
        head = pNew;
        tail = pNew;
    }
    else {
        pNew->prev = tail;
        tail->next = pNew;
        tail = pNew;
    }
    ++count;
}
template <class T>
void my::list<T>::push_back(const T& val) {
    node<T>* pNew = new node<T>(val);
    append(pNew);
}
template <class T>
void my::list<T>::push_front(const T& val) {
    node<T>* pNew = new node<T>(val);
    if (head == NULL) {
        head = pNew;
        tail = pNew;
    }
    else {
        pNew->next = head;
        head->prev = pNew;
        head = pNew;
    }
    ++count;
}
template <class T>
void my::list<T>::insert(node<T>* pTemp, node<T>* start, node<T>* stop) {
    node<T>* t = start;
    if (head == NULL) {  //if empty
        head = pTemp; tail = stop;
    }
    else if ((head != NULL) && (start == NULL)) { //if we're inserting at end
        tail->next = pTemp;
        pTemp->prev = tail;
        tail = stop;
    }
    else if (start == head) {  //if we're inserting at the head
        stop->next = head;
        head->prev = stop;
        head = pTemp;
    }
    else {
        pTemp->prev = start->prev;
        start->prev->next = pTemp;
        stop->next = start;
        start->prev = stop;
    }
    if (t == tail) tail = stop;
}
template <class T> class my::list<T>::
iterator my::list<T>::insert(iterator it, const T& val, size_t n) {
    node<T> *start = it.hd, *pTemp = new node<T>(val);
    node<T> *stop = pTemp;
    //create and link all nodes to be inserted when n > 1
    for (size_t i = 1; i < n; i++) {
        node<T>* pNew = new node<T>(val);
        pNew->prev = stop;
        stop->next = pNew;
        stop = pNew;
    }
    //insert the nodes now bounded by pTemp and stop at start
    insert(pTemp, start, stop);
    count += n;
    return iterator(pTemp);
}
template <class T> template <class InputIterator>
void my::list<T>::insert(iterator it, InputIterator begin, InputIterator end) {
    if (begin == end)
        return;
    node<T> *start = it.hd, *pTemp = new node<T>(*begin++);
    node<T> *stop = pTemp;
    size_t n = 1;
    for (; begin != end; ++begin, ++n) {
        node<T>* pNew = new node<T>(*begin);
        pNew->prev = stop;
        stop->next = pNew;
        stop = pNew;
    }
    insert(pTemp, start, stop);
    count += n;
}
template <class T>
void my::list<T>::insert(iterator it, std::initializer_list<T> l) {
    insert(it, l.begin(), l.end());
}
template <class T> template <class InputIterator>
void my::list<T>::copy(InputIterator begin, InputIterator end) {
    for (; begin != end; ++begin) {
        node<T>* pNew = new node<T>(*begin);
        append(pNew);
    }
}
/*-----------------ERASE FUNCTIONS-------------*/
template <class T>
void my::list<T>::remove(node<T>* temp) {
    if (head == NULL || temp == NULL)
        return;
    if (head == temp)
        head = temp->next;
    if (tail == temp)
        tail = tail->prev;
    if (temp->next != NULL)
        temp->next->prev = temp->prev;
    if (temp->prev != NULL)
        temp->prev->next = temp->next;
    delete temp;
    --count;
}
template <class T>
void my::list<T>::erase(iterator begin, iterator end) {
    node<T>* first = begin.hd;
    node<T>* last = end.hd;
    if (first == last || first == NULL) return;
    while (first != last) {
        node<T>* temp = first;
        first = first->next;
        remove(temp);
    }
}
template <class T>
void my::list<T>::removeAll(const T& val) {
    node<T>* current = head, *next;
    while (current != NULL) { 
        if (current->data == val) { 
            next = current->next; 
            remove(current);
            current = next; 
        }
        else
            current = current->next;
    }
}
template <class T> template <class func>
void my::list<T>::remove_if(func compare) {
    node<T>* current = head, *next;
    while (current != NULL) { 
        if (compare(current->data)) {
            next = current->next; 
            remove(current);
            current = next;
        }
        else
            current = current->next;
    }
}
/*--------------Other Utility functions--------*/
template <class T> template <class func>
void my::list<T>::doUnique(func compare) {
    if (head == NULL)
        return;
    node<T>* current = head, *next, *temp;
    while (current != NULL) {
        next = current->next;
        while(next != NULL && compare(current->data, next->data)) {
            temp = next->next;
            remove(next);
            next = temp;
        }
        current = next;
    }
}
template <class T>
void my::list<T>::resize(size_t v, T val) {
    while (count < v) {
        node<T>* pNew = new node<T>(val);
        append(pNew);
    }
    while (count > v) {
        pop_back();
    }
}
template <class T>
void my::list<T>::swap(list& l) {
    node<T>* t_head = l.head; l.head = this->head; this->head = t_head;
    node<T>* t_tail = l.tail; l.tail = this->tail; this->tail = t_tail;
    size_t t_count = l.count; l.count = this->count; this->count = t_count;
}
template <class T> template <class func>
void my::list<T>::storeTail(node<T>* theNode, func compare) {
    if (!compare(theNode->data, tail->data))
        tail = theNode;
}
template <class T> template <class func>
my::node<T>* my::list<T>::merge(node<T> *first, node<T> *second, func compare) {
    // If first linked list is empty
    if (!first)
        return second;
    // If second linked list is empty
    if (!second)
        return first;
    // Pick the smaller value
    if (compare(first->data, second->data)) {
        storeTail(second, compare);
        first->next = merge(first->next, second, compare);
        first->next->prev = first;
        first->prev = NULL;
        return first;
    }
    else {
        storeTail(first, compare);
        second->next = merge(first,second->next, compare);
        second->next->prev = second;
        second->prev = NULL;
        return second;
    }
}
template <class T>
my::node<T>* my::list<T>::split(node<T>* hd) {
    // Split a doubly linked list (DLL) into 2 DLLs of half sizes
    node<T> *fast = hd, *slow = hd;
    while (fast->next && fast->next->next) {
        fast = fast->next->next;
        slow = slow->next;
    }
    node<T> *temp = slow->next;
    slow->next = NULL;
    return temp;
}
template <class T> template <class func>
my::node<T>* my::list<T>::mergeSort(node<T>* hd, func compare) {
    if (!hd || !hd->next)
        return hd;
    node<T> *second = split(hd);
        
    // Recur for left and right halves
    hd = mergeSort(hd, compare);
    second = mergeSort(second, compare);
        
    // Merge the two sorted halves
    return merge(hd,second, compare);
}
template <class T>
void my::list<T>::reverse() {
    tail = head;
    node<T>* prev = NULL, *curr = head, *next; 
    while (curr) {
        next = curr->next; 
        curr->next = prev;
        prev = curr; 
        curr = next; 
    } 
    head = prev;
}
template <class T> class my::list<T>::
iterator my::list<T>::find(iterator begin, iterator end, T val) {
    node<T>* temp = head;
    while (temp != NULL) {
        if (temp->data == val) {
            return iterator(temp);
        }
        temp = temp->next;
    }
    return iterator();
}
template <class T>
void my::list<T>::clear() {
    node<T>* current = head;
    node<T>* next;
    while (current != NULL) {
        next = current->next;
        delete current;
        current = next;
    }
    head = NULL; tail = NULL; count = 0;
}
template<class T>
std::ostream& my::operator << (std::ostream& os, const my::list<T>& v) {
    my::node<T>* temp = v.head;
    int n = 0;
    os << "[ ";
    while (temp != NULL) {
        os << temp->data << " ";
        temp = temp->next;
    }
    os << "] " << std::endl;
    return os;
}
/*-----------RELATIONAL OPERATORS------------------*/
template <class InputIterator1, class InputIterator2>
  bool lexicographicalCompare (InputIterator1 first1, InputIterator1 last1,
                                InputIterator2 first2, InputIterator2 last2) {
    while (first1!=last1) {
        if (first2==last2 || *first2<*first1) return false;
        else if (*first1<*first2) return true;
        ++first1; ++first2;
    }
    return (first2!=last2);
}
template <class T>
bool operator==(my::list<T>& l1, my::list<T>& l2) {
    if (l1.size() != l2.size())
        return false;
    auto it1 = l1.begin(); auto it2 = l2.begin();
    for (; it1 != l1.end(); it1++, it2++) {
        if (*it1 != *it2)
            return false;
    }
    return true;
}
template <class T>
bool operator!=(my::list<T>& l1, my::list<T>& l2) {
    return !(l1 == l2);
}
template <class T>
bool operator<(my::list<T>& l1, my::list<T>& l2) {
    return(lexicographicalCompare(l1.begin(), l1.end(), l2.begin(), l2.end()));
}
template <class T>
bool operator<=(my::list<T>& l1, my::list<T>& l2) {
    return (l1 == l2 || l1 < l2);
}
template <class T>
bool operator>(my::list<T>& l1, my::list<T>& l2) {
    return !(l1 == l2 || l1 < l2);
}
template <class T>
bool operator>=(my::list<T>& l1, my::list<T>& l2) {
    return !(l1 < l2);
}

#endif
