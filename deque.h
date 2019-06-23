#ifndef DEQUE_H
#define DEQUE_H

#include <iostream>
#include <cstdlib>
#include "utilities.h"

namespace my {
    template <class T> class deque;
    template<class T> std::ostream& operator<< ( std::ostream& os, const deque<T>& v);
}

template <class T>
class my::deque {
    friend std::ostream& operator<< <> (std::ostream& os, const deque& dq);
    public:
    template <class U> class deque_iterator {
        public:
        typedef U value_type;
        typedef U& reference;
        typedef U* pointer;
        typedef std::random_access_iterator_tag iterator_category;
        typedef size_t difference_type;
        deque_iterator(size_t p = 0, const deque* t = NULL): ptr(p), dq(t) {}
        deque_iterator& operator=(const deque_iterator& it) {ptr = it.ptr; dq = it.dq; return *this;} 
        deque_iterator& operator+=(int n) {ptr += n; return *this;}
        deque_iterator& operator++() {ptr++; return *this;}
        deque_iterator operator++(int) {deque_iterator it = *this; ++*this; return it;}
        deque_iterator operator+(int n) {return deque_iterator(ptr+n, dq);}
        friend deque_iterator operator+(int n, deque_iterator& it) {return deque_iterator(it.ptr+n, it.dq);}
        deque_iterator& operator-=(int n) {ptr -= n; return *this;}
        deque_iterator& operator--() {ptr--; return *this;}
        deque_iterator operator-(int n) {return deque_iterator(ptr-n, dq);}
        deque_iterator operator--(int) {deque_iterator it = *this; --*this; return it;}
        difference_type operator-(const deque_iterator& it) {return ptr - it.ptr;}
        bool operator<(const deque_iterator& it) {return ptr < it.ptr;}
        bool operator<=(const deque_iterator& it) {return ptr <= it.ptr;}
        bool operator>(const deque_iterator& it) {return ptr > it.ptr;}
        bool operator>=(const deque_iterator& it) {return ptr >= it.ptr;}
        bool operator==(const deque_iterator& it) {return ptr == it.ptr;}
        bool operator!=(const deque_iterator& it) {return ptr != it.ptr;}
        reference operator*() {return (*dq)[ptr];}
        reference operator[](int n) {return (*dq)[ptr+n];}
        pointer operator->() {return &((*dq)[ptr]);}

        private:
        size_t ptr;
        const deque* dq;
    };
    typedef size_t size_type;
    typedef T value_type;
    typedef deque_iterator<T> iterator;
    typedef deque_iterator<const T> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    explicit deque();
    explicit deque(size_t n);
    deque(size_t n, const T& val);
    template<class InputIterator>
    deque(InputIterator begin, InputIterator end);
    deque(const deque& dq);
    ~deque() {free(arr);}
    deque& operator=(const deque& dq);
    void assign(size_t n, const T& val);
    template<class InputIterator>
    void assign(InputIterator begin, InputIterator end);
    T& at(size_t val) const;
    T& operator[](size_t i) const {return arr[(head+i)%size_];;}
    T& front() const;
    T& back() const;
    void push_front(const T& val); 
    void push_back(const T& val);
    void pop_front(); 
    void pop_back(); 
    bool empty() const;
    template<class InputIterator>
    void copy(InputIterator begin, InputIterator end);
    iterator insert(iterator it, const T& val, size_t n = 1);
    template<class InputIterator>
    void insert(iterator it, InputIterator begin, InputIterator end);
    void erase(iterator it);
    void erase(iterator begin, iterator end);
    void eraseAll(const T& val);
    size_t size() const;
    size_t capacity() const {return size_;}
    void resize(size_t n, const T& val);
    void resize(size_t n) {resize(n, T());}
    void shrink_to_fit();
    void swap(deque& vec);
    void clear();
    void sort() {quickSort(0, size()-1, my::less<T>());}
    template <class C>
    void sort(C compare) {quickSort(0, size()-1, compare);}
    iterator find(iterator begin, iterator end, const T& val);
    iterator find_s(iterator begin, iterator end, const T& val);  //uses binary search
    iterator begin() const {return iterator(0, this);}
    iterator end() const {return iterator(size(), this);}
    const_iterator cbegin() const {return const_iterator(0, this);}
    const_iterator cend() const {return const_iterator(size(), this);}
    reverse_iterator rbegin() const {return reverse_iterator(end());}
    reverse_iterator rend() const {return reverse_iterator(begin());}
    const_reverse_iterator crbegin() const 
    {return const_reverse_iterator(cend());}
    const_reverse_iterator crend() const
    {return const_reverse_iterator(cbegin());}

    #if __cplusplus >= 201103L
    deque(std::initializer_list<T> l);
    deque(deque&& dq);
    deque& operator=(std::initializer_list<T> l);
    deque& operator=(deque&& dq);
    void insert(iterator it, std::initializer_list<T> l);
    template <class... Args>
    iterator emplace(iterator it, Args&&... args) {return insert(it, T(args...));}
    template <class... Args>
    void emplace_back(Args&&... args) {push_back(T(args...));}
    template <class... Args>
    void emplace_front(Args&&... args) {push_front(T(args...));}
    #endif

    private:
    T* arr;
    size_t size_;
    size_t head;
    size_t tail;
    void growArray(size_t n);
    bool isFull();
    template <class C>
    size_t partition(size_t l, size_t h, const C& comp);
    template <class C>
    void quickSort(size_t low, size_t high, const C& comp);
};
template <class T>
bool my::deque<T>::isFull() {
    return ((head == tail+1) || (head == 0 && tail == size_-1));
}
template <class T>
void my::deque<T>::growArray(size_t n) {
    T* temp = (T*)malloc(sizeof(T)*(size_+n));
    size_t size = this->size();
    for (size_t i = 0; i < size; i++)
        temp[i] = arr[(head+i)%size_];
    free(arr);
    arr = temp; size_ += n; head = 0; tail = size-1;
}

template <class T>
void my::deque<T>::resize(size_t n, const T& val) {
    if (n == 0) {
        clear(); return;
    }
    if (n > size_) {
        growArray(n-size_);
    }
    if (n < size()) {
        T* temp = (T*)malloc(sizeof(T)*n);
        for (size_t i = 0; i < n; i++)
            temp[i] = arr[(head+i)%size_];
        free(arr);
        arr = temp; size_ = n; head = 0; tail = n-1;
    }
    if (n > size()) {
        for (size_t i = size(); i < n; i++)
            push_back(val);
    }
}
template <class T>
void my::deque<T>::shrink_to_fit() {
    size_t size = this->size();
    T* temp = (T*)malloc(sizeof(T)*size);
    for (size_t i = 0; i < size; i++)
        temp[i] = arr[(head+i)%size_];
    free(arr);
    arr = temp; size_ = size; head = 0; tail = size-1;
}
template <class T>
void my::deque<T>::swap(deque& dq) {
    T* t_arr = dq.arr; dq.arr = this->arr; this->arr = t_arr;
    size_t t_size_ = dq.size_; dq.size_= this->size_; this->size_= t_size_;
    size_t t_head = dq.head; dq.head = this->head; this->head = t_head;
    size_t t_tail = dq.tail; dq.tail = this->tail; this->tail = t_tail;
}
template <class T>
void my::deque<T>::clear() {
    free(arr);
    arr = NULL; size_ = 0; head = -1; tail = -1;
}
template <class T>
bool my::deque<T>::empty() const {
    return (head == -1);
}
template <class T>
size_t my::deque<T>::size() const {
    if (empty()) return 0;
    return (head > tail) ? size_-head + tail+1 : tail - head + 1;
}
template <class T> template<class InputIterator>
void my::deque<T>::copy(InputIterator begin, InputIterator end) {
    while (begin != end) {
        this->push_back(*begin);
        ++begin;
    }
}
template <class T>
my::deque<T>::deque(): arr(NULL), size_(0), head(-1), tail(-1) {}

template <class T>
my::deque<T>::deque(size_t size): size_(size), head(0), tail(size-1) {
    arr = (T*)malloc(sizeof(T)*size_);
    for (int i = 0; i < size; i++)
        arr[i] = T();
}
template <class T>
my::deque<T>::deque(size_t size, const T& val): size_(size), head(0), tail(size-1) {
    arr = (T*)malloc(sizeof(T)*size_);
    for (int i = 0; i < size; i++)
        arr[i] = val;
}
template <class T> template<class InputIterator>
my::deque<T>::deque(InputIterator begin, InputIterator end): arr(NULL), size_(0), head(-1), tail(-1) {
    this->copy(begin, end);
}
#if __cplusplus >= 201103L
template <class T>
my::deque<T>::deque(std::initializer_list<T> l): deque(l.begin(), l.end()) {}

template <class T>
my::deque<T>& my::deque<T>::operator=(std::initializer_list<T> l) {
    clear();
    this->copy(l.begin(), l.end());
    return *this;
}
template <class T>
my::deque<T>::deque(deque&& v) {
    size_ = v.size_; v.size_ = 0;
    head = v.head; v.head = -1;
    tail = v.tail; v.tail = -1;
    arr = v.arr; v.arr = NULL;
}
template <class T>
my::deque<T>& my::deque<T>::operator=(deque&& v) {
    if (this != &v) {
        free(arr);
        size_ = v.size_; v.size_ = 0;
        head = v.head; v.head = -1;
        tail = v.tail; v.tail = -1;
        arr = v.arr; v.arr = NULL;
    }
    return *this;
}
template <class T>
void my::deque<T>::insert(iterator it, std::initializer_list<T> l) {
    size_t n = l.size();
    this->resize(size()+n);
    size_t first = it.ptr;
    size_t last = first + n - 1;
    size_t i;
    for (i = size()-1; i > last; i--)
        (*this)[i] = (*this)[i-n];
    auto iter = l.begin();
    for (i = first; i <= last; i++) {
        (*this)[i] = *iter;
        ++iter;
    }
}
#endif

template <class T>
my::deque<T>::deque(const deque& dq): deque(dq.begin(), dq.end()) {}

template <class T>
my::deque<T>& my::deque<T>::operator=(const deque& dq) {
    clear();
    this->copy(dq.begin(), dq.end());
    return *this;
}
template <class T>
void my::deque<T>::assign(size_t n, const T& val) {
    clear();
    for (size_t i = 0; i < n; i++)
        push_back(val);
}
template <class T> template<class InputIterator>
void my::deque<T>::assign(InputIterator begin, InputIterator end) {
    clear();
    copy(begin, end);
}
template <class T>
T& my::deque<T>::at(size_t val) const {
    try {
        if (val >= size()) throw val;
    }
    catch (size_t i) {
        std::cout << "Error in my::deque::range_check: ptr " << i << " is out of range\nthis->size() is " << size() << "\n";
        _Exit (EXIT_FAILURE);
    }
    return arr[(head+val)%size_];
}
template <class T>
T& my::deque<T>::front() const {
    try {
        if (empty()) throw "Error in my::deque::range_check in call to my::vector::front()\n String is empty.\n";
    }
    catch (const char* s) {
        std::cout << s;
        _Exit (EXIT_FAILURE);
    }
    return arr[head];
}
template <class T>
T& my::deque<T>::back() const {
    try {
        if (empty()) throw "Error in my::deque::range_check in call to my::vector::back()\n String is empty.\n";
    }
    catch (const char* s) {
        std::cout << s;
        _Exit (EXIT_FAILURE);
    }
    return arr[tail];
}
template <class T>
void my::deque<T>::push_front(const T& val) {
    if (size_ == 0) {
        size_ = 10;
        arr = new T [size_];
    }
    else if (isFull()) growArray(size_);
    if (head == -1) {
        head = 0; tail = 0;
    }
    else if (head == 0)
        head = size_-1;
    else
        --head;
    arr[head] = val;
}
template <class T>
void my::deque<T>::push_back(const T& val) {
    if (size_ == 0) {
        size_ = 10;
        arr = new T [size_];
    }
    else if (isFull()) growArray(size_);
    if (tail == -1) {
        head = 0; tail = 0;
    }
    else if (tail == size_-1)
        tail = 0;
    else
        ++tail;
    arr[tail] = val;
}
template <class T> class my::deque<T>::
deque_iterator<T> my::deque<T>::insert(iterator it, const T& val, size_t n) {
    this->resize(size()+n);
    size_t begin = it.ptr;
    size_t end = begin + n - 1;
    size_t i;
    for (i = size()-1; i > end; i--)
        (*this)[i] = (*this)[i-n];
    for (; i >= begin; i--)
        (*this)[i] = val;
    return iterator(this, begin);
}
template <class InputIterator>
size_t distance(InputIterator first, InputIterator& last, std::random_access_iterator_tag) {
    return last - first;
}
template <class InputIterator>
size_t distance(InputIterator first, InputIterator& last, typename std::iterator_traits<InputIterator>::iterator_category) {
    int n = 0;
    for (; first != last; ++first)
        n++;
    return n;
}
template <class T> template<class InputIterator>
void my::deque<T>::insert(iterator it, InputIterator begin, InputIterator end) {
    size_t n = distance(begin, end, typename std::iterator_traits<InputIterator>::iterator_category());
    this->resize(size()+n);
    size_t first = it.ptr;
    size_t last = first + n - 1;
    size_t i;
    for (i = size()-1; i > last; i--)
        (*this)[i] = (*this)[i-n];
    for (i = first; i <= last; i++) {
        (*this)[i] = *begin;
        ++begin;
    }
}
template <class T>
void my::deque<T>::erase(iterator it) {
    size_t size = this->size();
    if (size == 0)
        return;
    size_t ptr = it.ptr;
    for (size_t i = ptr; i < size-1; i++) {
        (*this)[i] = (*this)[i+1];
    }
    this->pop_back();
}
template <class T>
void my::deque<T>::erase(iterator begin, iterator end) {
    size_t size = this->size();
    if (size == 0)
        return;
    size_t n = end.ptr - begin.ptr;
    for (size_t i = begin.ptr; i < size-n; i++)
        (*this)[i] = (*this)[i+n];
    this->resize(size-n);
}
template <class T>
void my::deque<T>::eraseAll(const T& val) {
    size_t size = this->size();
    if (size == 0)
        return;
    size_t i = 0;
    for (size_t j = 0; j < size; j++) {
        if ((*this)[j] != val)
            (*this)[i++] = (*this)[j];
    }
    this->resize(i);
}
template <class T>
void my::deque<T>::pop_front() {
    if(empty()) {
        std::cout << "deque is empty\n";
        return;
    }
    if (head == tail)
        head = tail = -1;
    else {
        if (head == size_-1)
            head = 0;
        else
            ++head;
    }
}
template <class T>
void my::deque<T>::pop_back() {
    if(empty()) {
        std::cout << "deque is empty\n";
        return;
    }
    if (head == tail)
        head = tail = -1;
    else if (tail == 0) 
        tail = size_-1;
    else
        --tail; 
}
template <class T> template <class C>
size_t my::deque<T>::partition(size_t l, size_t h, const C& comp) {
    bool done = false;
    int midpoint = l + (h - l) / 2;
    T pivot = (*this)[midpoint];
    while (!done) {
        while (comp((*this)[l], pivot)) ++l;
        while (comp(pivot, (*this)[h])) --h;
        if (l >= h)
            done = true;
        else {
            my::swap((*this)[l], (*this)[h]);
            ++l; --h;
        }
    }
    return h;
}
template <class T> template <class C>
void my::deque<T>::quickSort(size_t l, size_t h, const C& comp) {
    if (l >= h)
        return;
    int m = partition(l, h, comp);
    quickSort(l, m, comp);
    quickSort(m + 1, h, comp);
}
template <class T> class my::deque<T>::
deque_iterator<T> my::deque<T>::find(iterator begin, iterator end, const T& val) {
    size_t start = begin - this->begin(), stop = end - this->begin();
    for (; start < stop; start++) {
        if ((*this)[start] == val)
            break;
    }
    return iterator(this, start);
}
template <class T> class my::deque<T>::
deque_iterator<T> my::deque<T>::find_s(iterator begin, iterator end, const T& val) {
    size_t mid, low(begin - this->begin()), high((end - this->begin())- 1);

    while (high >= low) {
        mid = (high + low) / 2;
        if ((*this)[mid] < val)
            low = mid + 1;
        else if ((*this)[mid] > val)
            high = mid - 1;
        else
            return iterator(this, mid);
    }
    return iterator(this, size()); // not found
}
template<class T>
std::ostream& my::operator << (std::ostream& os, const my::deque<T>& dq) {
    os << "[ ";
    for (int i = 0; i < dq.size(); i++) {
        os << dq[i] << " ";
    }
    os << "]" << std::endl;
    return os;
}
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
bool operator==(my::deque<T>& d1, my::deque<T>& d2) {
    if (d1.size() != d2.size())
        return false;
    for (int i = 0; i < d1.size(); i++) {
        if (d1[i] != d2[i])
            return false;
    }
    return true;
}
template <class T>
bool operator!=(my::deque<T>& d1, my::deque<T>& d2) {
    return !(d1 == d2);
}
template <class T>
bool operator<(my::deque<T>& d1, my::deque<T>& d2) {
    return(lexicographicalCompare(d1.begin(), d1.end(), d2.begin(), d2.end()));
}
template <class T>
bool operator<=(my::deque<T>& d1, my::deque<T>& d2) {
    return (d1 == d2 || d1 < d2);
}
template <class T>
bool operator>(my::deque<T>& d1, my::deque<T>& d2) {
    return !(d1 == d2 || d1 < d2);
}
template <class T>
bool operator>=(my::deque<T>& d1, my::deque<T>& d2) {
    return !(d1 < d2);
}

#endif
