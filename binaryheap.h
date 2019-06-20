#ifndef BINARYHEAP_H
#define BINARYHEAP_H

#include <iostream>

namespace my {
    template <class T> struct greater;
    template <class T> void swap(T& a, T& b);
    template <class T, class C = greater<T> > class binary_heap;
}
template<class T> struct my::greater {
    bool operator()(const T& lhs, const T& rhs) const {
        return lhs > rhs;
    }
};
template <class T> void my::swap(T& a, T& b) {
    T temp = a; a = b; b = temp;
}
template <class T, class C> class my::binary_heap {
    public:
    template <class U> class b_heap_iterator {
        public:
        typedef U value_type;
        typedef U& reference;
        typedef U* pointer;
        typedef std::random_access_iterator_tag iterator_category;
        typedef size_t difference_type;
        b_heap_iterator(size_t p = 0, const binary_heap* t = NULL): ptr(p), bh(t) {}
        b_heap_iterator& operator=(const b_heap_iterator& it) {ptr = it.ptr; bh = it.bh; return *this;} 
        b_heap_iterator& operator+=(int n) {ptr += n; return *this;}
        b_heap_iterator& operator++() {ptr++; return *this;}
        b_heap_iterator operator++(int) {b_heap_iterator it = *this; ++*this; return it;}
        b_heap_iterator operator+(int n) {return b_heap_iterator(ptr+n, bh);}
        friend b_heap_iterator operator+(int n, b_heap_iterator& it) {return b_heap_iterator(it.ptr+n, it.bh);}
        b_heap_iterator& operator-=(int n) {ptr -= n; return *this;}
        b_heap_iterator& operator--() {ptr--; return *this;}
        b_heap_iterator operator-(int n) {return b_heap_iterator(ptr-n, bh);}
        b_heap_iterator operator--(int) {b_heap_iterator it = *this; --*this; return it;}
        difference_type operator-(const b_heap_iterator& it) {return ptr - it.ptr;}
        bool operator<(const b_heap_iterator& it) {return ptr < it.ptr;}
        bool operator<=(const b_heap_iterator& it) {return ptr <= it.ptr;}
        bool operator>(const b_heap_iterator& it) {return ptr > it.ptr;}
        bool operator>=(const b_heap_iterator& it) {return ptr >= it.ptr;}
        bool operator==(const b_heap_iterator& it) {return ptr == it.ptr;}
        bool operator!=(const b_heap_iterator& it) {return ptr != it.ptr;}
        reference operator*() {return bh->arr[ptr];}
        reference operator[](int n) {return bh->arr[ptr+n];}
        pointer operator->() {return &bh->arr[ptr];}

        private:
        size_t ptr;
        const binary_heap* bh;
    };
    typedef size_t size_type;
    typedef b_heap_iterator<T> iterator;
    typedef b_heap_iterator<const T> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    binary_heap(const C& comp = C());
    template<class InputIterator> binary_heap(InputIterator begin, InputIterator end);
    binary_heap(std::initializer_list<T> l);
    binary_heap(const binary_heap& bh);
    ~binary_heap() {delete [] arr;}
    binary_heap& operator=(const binary_heap& bh);
    binary_heap& operator=(std::initializer_list<T> l);
    template<class InputIterator> void assign(InputIterator begin, InputIterator end);
    T& top() { return arr[0];}
    void pop();
    iterator move_up(iterator it, const T& new_val);
    iterator insert(const T& val);
    template<class InputIterator> void insert(InputIterator begin, InputIterator end);
    void insert(std::initializer_list<T> l);
    template <class... Args> void emplace (Args&&... args) {insert(std::forward<Args>(args)...);}
    void erase(iterator it);
    binary_heap operator+(const binary_heap& bh) const;
    bool empty() const {return inUse == 0;}
    size_t size() const {return inUse;}
    size_t capacity() const {return allocated;}
    void reserve(size_t);
    void shrink_to_fit();
    void swap(binary_heap& bh);
    iterator begin() const {return iterator(0, this);}
    iterator end() const {return iterator(inUse, this);}
    const_iterator cbegin() const {return const_iterator(0, this);}
    const_iterator cend() const {return const_iterator(inUse, this);}
    reverse_iterator rbegin() const {return reverse_iterator(end());}
    reverse_iterator rend() const {return reverse_iterator(begin());}
    const_reverse_iterator crbegin() const 
    {return const_reverse_iterator(cend());}
    const_reverse_iterator crend() const
    {return const_reverse_iterator(cbegin());}

    private:
    void growArray(size_t);
    void clear();
    void heapify(size_t);
    size_t parent (size_t i) { return (i-1)/2; }
    size_t left (size_t i) { return (2*i + 1); }
    size_t right(size_t i) { return (2*i + 2); }
    template <class InputIterator>
    void insert_help(InputIterator first, InputIterator& last, std::random_access_iterator_tag);
    template <class InputIterator>
    void insert_help(InputIterator first, InputIterator last, typename std::iterator_traits<InputIterator>::iterator_category);

    T* arr;
    size_t inUse;
    size_t allocated;
    C compare;
};
template <class T, class C>
my::binary_heap<T, C>::binary_heap(const C& comp): arr(NULL), inUse(0), allocated(0) {
    compare = comp;
}
template <class T, class C> template<class InputIterator>
my::binary_heap<T, C>::binary_heap(InputIterator begin, InputIterator end): binary_heap(C()) {
    insert_help(begin, end, typename std::iterator_traits<InputIterator>::iterator_category());
}
template <class T, class C>
my::binary_heap<T, C>::binary_heap(std::initializer_list<T> l): inUse(0), allocated(l.size()) {
    compare = C();
    arr = new T [allocated];
    for(const T& x : l) insert(x);
}
template <class T, class C>
my::binary_heap<T, C>::binary_heap(const binary_heap& bh): allocated(bh.inUse) {
    inUse = bh.inUse;
    arr = new T [allocated];
    for (size_t i = 0; i < inUse; ++i)
        arr[i] = bh.arr[i];
}
template <class T, class C>
my::binary_heap<T, C>& my::binary_heap<T, C>::operator=(const binary_heap& bh) {
    delete [] arr;
    inUse = allocated = bh.inUse;
    arr = new T [allocated];
    for (size_t i = 0; i < inUse; ++i)
        arr[i] = bh.arr[i];  
}
template <class T, class C>
my::binary_heap<T, C>& my::binary_heap<T, C>::operator=(std::initializer_list<T> l) {
    delete [] arr;
    inUse = allocated = l.size();
    arr = new T [allocated];
    for(const T& x : l) insert(x);
}
template <class T, class C> template<class InputIterator>
void my::binary_heap<T, C>::assign(InputIterator begin, InputIterator end) {
    clear();
    insert_help(begin, end, typename std::iterator_traits<InputIterator>::iterator_category());
}
template <class T, class C>
void my::binary_heap<T, C>::pop() {
    if (inUse == 0) return;
    else if (inUse == 1) inUse = 0;
    else {
        arr[0] = arr[--inUse];
        heapify(0);
    }
}
template <class T, class C> class my::binary_heap<T, C>::
b_heap_iterator<T> my::binary_heap<T, C>::move_up(iterator it, const T& new_val) {
    size_t i = it - this->begin();
    arr[i] = new_val;
    while (i != 0 && !compare(arr[parent(i)], arr[i])) { 
       my::swap(arr[i], arr[parent(i)]); 
       i = parent(i); 
    }
    return iterator(i, this); 
}
template <class T, class C> class my::binary_heap<T, C>::
b_heap_iterator<T> my::binary_heap<T, C>::insert(const T& val) {
    if (inUse == allocated)
        growArray(allocated);
    arr[inUse++] = val;
    size_t i = inUse-1;
    while (i != 0 && !compare(arr[parent(i)], arr[i])) { 
       my::swap(arr[i], arr[parent(i)]);
       i = parent(i); 
    }
    return iterator(i, this); 
}
template <class T, class C> template<class InputIterator>
void my::binary_heap<T, C>::insert(InputIterator begin, InputIterator end) {
    insert_help(begin, end, typename std::iterator_traits<InputIterator>::iterator_category());
}
template <class T, class C>
void my::binary_heap<T, C>::insert(std::initializer_list<T> l) {
    reserve(inUse + l.size());
    for(const T& x : l) insert(x);
}
template <class T, class C>
void my::binary_heap<T, C>::erase(iterator it) {
    move_up(it, arr[0]);
    pop();
}
template <class T, class C>
my::binary_heap<T, C> my::binary_heap<T, C>::operator+(const binary_heap& bh) const {
    binary_heap<T, C> h;
    h.allocated = inUse + bh.inUse;
    h.arr = new T [h.allocated];
    size_t i;
    for (i = 0; i < inUse; ++i)
        h.arr[i] = arr[i];
    h.inUse = inUse;
    for (i = 0; i < bh.inUse; ++i)
        h.insert(bh.arr[i]);
    return h;
}
template <class T, class C>
void my::binary_heap<T, C>::reserve(size_t n) {
    if (n > allocated)
        growArray(n-allocated);
}
template <class T, class C>
void my::binary_heap<T, C>::shrink_to_fit() {
    if (allocated > inUse) {
        T* temp = arr;
        allocated = inUse;
        arr = new T [allocated];
        for (int i = 0; i < inUse; i++)
            arr[i] = temp[i];
        delete [] temp;
    }
}
template <class T, class C>
void my::binary_heap<T, C>::swap(binary_heap& bh) {
    T* t_arr = bh.arr; bh.arr = arr; arr = t_arr;
    size_t t_inUse = bh.inUse; bh.inUse = inUse; inUse = t_inUse;
    size_t t_alloc = bh.allocated; bh.allocated = allocated; allocated = t_alloc;
}
template <class T, class C>
void my::binary_heap<T, C>::growArray(size_t n) {
    if (n < 2) n = 2;
    T* temp = arr;
    allocated += n;
    arr = new T [allocated];
    for (size_t i = 0; i < inUse; i++)
        arr[i] = temp[i];
    delete [] temp;
}
template <class T, class C>
void my::binary_heap<T, C>::clear() {
    delete [] arr;
    arr = NULL; inUse = 0; allocated = 0;
}
template <class T, class C>
void my::binary_heap<T, C>::heapify(size_t i) {
    size_t l = left(i), r = right(i), smallest = i;
    if (l < inUse && compare(arr[l], arr[i])) 
        smallest = l; 
    if (r < inUse && compare(arr[r], arr[smallest])) 
        smallest = r; 
    if (smallest != i) { 
        my::swap(arr[i], arr[smallest]); 
        heapify(smallest);
    }
}
template <class T, class C> template <class InputIterator>
void my::binary_heap<T, C>::insert_help(InputIterator first, InputIterator& last, std::random_access_iterator_tag) {
    reserve(inUse+(last-first));
    for (; first != last; ++first)
        insert(*first);
}
template <class T, class C> template <class InputIterator>
void my::binary_heap<T, C>::insert_help(InputIterator first, InputIterator last, typename std::iterator_traits<InputIterator>::iterator_category) {
    for (; first != last; ++first)
        insert(*first);
}

#endif