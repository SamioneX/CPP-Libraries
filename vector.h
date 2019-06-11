#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>
#include <iterator>
#include <cstdlib>

namespace my {
    template <class T> class vector;
    template<class T> std::ostream& operator<< ( std::ostream& os, const vector<T>& v);
}
template <class T>
class my::vector {
    friend std::ostream& operator<< <> (std::ostream& os, const vector& v);
    public:
    template <class U> class vector_iterator {
        public:
        typedef U value_type;
        typedef U& reference;
        typedef U* pointer;
        typedef std::random_access_iterator_tag iterator_category;
        typedef size_t difference_type;
        vector_iterator(size_t p = 0, const vector* t = NULL): ptr(p), v(t) {}
        vector_iterator& operator=(const vector_iterator& it) {ptr = it.ptr; v = it.v; return *this;} 
        vector_iterator& operator+=(int n) {ptr += n; return *this;}
        vector_iterator& operator++() {ptr++; return *this;}
        vector_iterator operator++(int) {vector_iterator it = *this; ++*this; return it;}
        vector_iterator operator+(int n) {return vector_iterator(ptr+n, v);}
        friend vector_iterator operator+(int n, vector_iterator& it) {return vector_iterator(it.ptr+n, it.v);}
        vector_iterator& operator-=(int n) {ptr -= n; return *this;}
        vector_iterator& operator--() {ptr--; return *this;}
        vector_iterator operator-(int n) {return vector_iterator(ptr-n, v);}
        vector_iterator operator--(int) {vector_iterator it = *this; --*this; return it;}
        difference_type operator-(const vector_iterator& it) {return ptr - it.ptr;}
        bool operator<(const vector_iterator& it) {return ptr < it.ptr;}
        bool operator<=(const vector_iterator& it) {return ptr <= it.ptr;}
        bool operator>(const vector_iterator& it) {return ptr > it.ptr;}
        bool operator>=(const vector_iterator& it) {return ptr >= it.ptr;}
        bool operator==(const vector_iterator& it) {return ptr == it.ptr;}
        bool operator!=(const vector_iterator& it) {return ptr != it.ptr;}
        reference operator*() {return v->arr[ptr];}
        reference operator[](int n) {return v->arr[ptr+n];}
        pointer operator->() {return v->arr+ptr;}

        private:
        size_t ptr;
        const vector* v;
    };
    typedef size_t size_type;
    typedef vector_iterator<T> iterator;
    typedef vector_iterator<const T> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    vector();
    vector(size_t n, T val = T());
    template<class InputIterator>
    vector(InputIterator begin, InputIterator end);
    vector(std::initializer_list<T> l);
    vector(const vector& vec);
    ~vector();
    vector& operator=(const vector& vec);
    vector& operator=(std::initializer_list<T> l);
    void assign(size_t n, const T& val);
    template<class InputIterator>
    void assign(InputIterator begin, InputIterator end);
    T& at(size_t val) const;
    T& operator[](int val) const {return this->at(val);}
    T& front () const;
    T& back() const;
    T* data() const {return arr;}
    void push_back(const T& val);
    void pop_back();
    template<class InputIterator>
    void copy(InputIterator begin, InputIterator end);
    iterator insert(iterator it, const T& val, size_t n = 1);
    template<class InputIterator>
    void insert(iterator it, InputIterator begin, InputIterator end);
    void insert(iterator it, std::initializer_list<T> l);
    iterator emplace(iterator it, const T& val) {return insert(it, val);}
    void emplace_back(const T& val) {push_back(val);}
    void erase(iterator it);
    void erase(iterator begin, iterator end);
    void eraseAll(const T& val);
    bool empty() const {return inUse == 0;}
    size_t size() const {return inUse;}
    size_t capacity() const {return allocated;}
    void resize(size_t v, T val = T());
    void reserve(size_t v);
    void shrink_to_fit();
    void swap(vector& vec);
    void sort() {quickSort(0, size()-1);}
    iterator find(iterator begin, iterator end, const T& val) const;
    iterator find_s(iterator begin, iterator end, const T& val) const;
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
    T* arr;
    size_t inUse;
    size_t allocated;

    void growArray(size_t n);
    void clear();
    size_t partition(size_t l, size_t h);
    void quickSort(size_t low, size_t high);
};

template <class T> template<class InputIterator>
void my::vector<T>::copy(InputIterator begin, InputIterator end) {
    while (begin != end) {
        this->push_back(*begin);
        ++begin;
    }
}
template <class T>
my::vector<T>::vector(): arr(NULL), inUse(0), allocated(0) {}

template <class T>
my::vector<T>::vector(size_t n, T val): inUse(n), allocated(n) {
    arr = (T*)malloc(sizeof(T)*allocated);
    for (size_t i = 0; i < n; i++)
        arr[i] = val;
}
template <class T> template<class InputIterator>
my::vector<T>::vector(InputIterator begin, InputIterator end): vector() {
    this->copy(begin, end);
}
template <class T>
my::vector<T>::vector(std::initializer_list<T> l): allocated(l.size()) {
    arr = (T*)malloc(sizeof(T)*allocated);
    auto it = l.begin();
    for (size_t i = 0; i < allocated; ++i)
        arr[i] = *it++;
    inUse = allocated;
}
template <class T>
my::vector<T>::vector(const vector& vec): allocated(vec.size()) {
    arr = (T*)malloc(sizeof(T)*allocated);
    for (size_t i = 0; i < allocated; i++) {
        arr[i] = vec.arr[i];
    }
    inUse = allocated;
}
template <class T>
my::vector<T>::~vector() {
    this->clear();
}
template <class T>
my::vector<T>& my::vector<T>::operator=(const vector& vec) {
    clear();
    this->copy(vec.begin(), vec.end());
    return *this;
}
template <class T>
my::vector<T>& my::vector<T>::operator= (std::initializer_list<T> l) {
    clear();
    this->copy(l.begin(), l.end());
    return *this;
}
template <class T>
void my::vector<T>::assign(size_t n, const T& val) {
    clear();
    inUse = allocated = n;
    arr = (T*)malloc(sizeof(T)*allocated);
    for (size_t i = 0; i < inUse; i++)
        arr[i] = val;
}
template <class T> template<class InputIterator>
void my::vector<T>::assign(InputIterator begin, InputIterator end) {
    clear();
    copy(begin, end);
}
template <class T>
T& my::vector<T>::at(size_t val) const {
     try {
        if (val >= inUse) throw val;
    }
    catch (size_t i) {
        std::cout << "Error in my::vector::range_check: index " << i << " is out of range\nthis->size() is " << inUse << "\n";
        _Exit (EXIT_FAILURE);
    }
    return arr[val];
}
template <class T>
T& my::vector<T>::front() const {
    try {
        if (inUse == 0) throw "Error in my::vector::range_check in call to my::vector::front()\n String is empty.\n";
    }
    catch (const char* s) {
        std::cout << s;
        _Exit (EXIT_FAILURE);
    }
    return arr[0];
}
template <class T>
T& my::vector<T>::back() const {
    try {
        if (inUse == 0) throw "Error in my::vector::range_check in call to my::vector::back()\n String is empty.\n";
    }
    catch (const char* s) {
        std::cout << s;
        _Exit (EXIT_FAILURE);
    }
    return arr[inUse-1];
}
template <class T>
void my::vector<T>::growArray(size_t n) {
    if (n < 2) n = 2;
    T* temp = arr;
    allocated += n;
    arr = (T*)malloc(sizeof(T)*allocated);
    for (size_t i = 0; i < inUse; i++)
        arr[i] = temp[i];
    free(temp);
}
template <class T>
void my::vector<T>::resize(size_t v, T val) {
    size_t i(0);
    if (v < inUse) {
        T* temp = arr;
        allocated = v;
        arr = (T*)malloc(sizeof(T)*allocated);
        for (; i < v; i++) {
            arr[i] = temp[i];
        }
        free(temp);
    }
    this->reserve(v);
    if (v > inUse) {
        for (i = inUse; i < v; i++)
            arr[i] = val;
    }
    inUse = v;
}
template <class T>
void my::vector<T>::reserve(size_t v) {
    if (v > allocated)
        growArray(v-allocated);
}
template <class T>
void my::vector<T>::push_back(const T& val) {
    if (inUse == allocated)
        growArray(allocated);
    arr[inUse++] = val;
}
template <class T>
void my::vector<T>::pop_back() {
    --inUse;
    if (inUse == 0)
        clear();
}
template <class T>
void my::vector<T>::shrink_to_fit() {
    T* temp = arr;
    allocated = inUse;
    arr = (T*)malloc(sizeof(T)*allocated);
    for (int i = 0; i < inUse; i++)
        arr[i] = temp[i];
    free(temp);
}
template <class T>
void my::vector<T>::swap(vector& vec) {
    T* t_arr = vec.arr; vec.arr = this->arr; this->arr = t_arr;
    size_t t_inUse = vec.inUse; vec.inUse = this->inUse; this->inUse = t_inUse;
    size_t t_alloc = vec.allocated; vec.allocated = this->allocated; this->allocated = t_alloc;
}
template <class T>
void my::vector<T>::clear() {
    free(arr);
    arr = NULL; inUse = 0; allocated = 0;
}
template <class T> class my::vector<T>::
vector_iterator<T> my::vector<T>::insert(iterator it, const T& val, size_t n) {
    int begin = it - this->begin();
    int end = begin + n - 1;
    this->resize(inUse+n);
    int i;
    for (i = inUse-1; i > end; i--)
        arr[i] = arr[i-n];
    for (; i >= begin; i--)
        arr[i] = val;
    return iterator(arr+begin);
}
template <class InputIterator>
int distance(InputIterator first, InputIterator& last, std::random_access_iterator_tag) {
    return last - first;
}
template <class InputIterator>
int distance(InputIterator first, InputIterator& last, typename std::iterator_traits<InputIterator>::iterator_category) {
    int n = 0;
    for (; first != last; ++first)
        n++;
    return n;
}
template <class T> template<class InputIterator>
void my::vector<T>::insert(iterator it, InputIterator begin, InputIterator end) {
    int n = distance(begin, end, typename std::iterator_traits<InputIterator>::iterator_category());
    int first = it - this->begin();
    int last = first + n - 1;
    this->resize(inUse+n);
    int i;
    for (i = inUse-1; i > last; i--)
        arr[i] = arr[i-n];
    for (i = first; i <= last; i++) {
        arr[i] = *begin;
        ++begin;
    }
}
template <class T>
void my::vector<T>::insert(iterator it, std::initializer_list<T> l) {
    int n = l.size();
    int first = it - this->begin();
    int last = first + n - 1;
    this->resize(inUse+n);
    int i;
    for (i = inUse-1; i > last; i--)
        arr[i] = arr[i-n];
    auto iter = l.begin();
    for (i = first; i <= last; i++) {
        arr[i] = *iter;
        ++iter;
    }
}
template <class T>
void my::vector<T>::erase(iterator it) {
    if (inUse == 0)
        return;
    for (int i = it - this->begin(); i < inUse-1; i++)
        arr[i] = arr[i+1];
    this->pop_back();
}
template <class T>
void my::vector<T>::erase(iterator begin, iterator end) {
    if (inUse == 0)
        return;
    int n = end - begin;
    for (int i = begin.index; i < inUse-n; i++)
        arr[i] = arr[i+n];
    this->resize(inUse-n);
}
template <class T>
void my::vector<T>::eraseAll(const T& val) {
    if (inUse == 0)
        return;
    int i = 0;
    for (int j = 0; j < inUse; j++) {
        if (arr[j] != val)
            arr[i++] = arr[j];
    }
    this->resize(i);
}
template <class T>
size_t my::vector<T>::partition(size_t l, size_t h) {
    T temp;
    bool done = false;
   
    // Pick middle element as pivot
    size_t midpoint = l + (h - l) / 2;
    T pivot = arr[midpoint];
    while (!done) {
        // Increment l while numbers[l] < pivot
        while (arr[l] < pivot) ++l;
        // Decrement h while pivot < numbers[h]
        while (pivot < arr[h]) --h;
        // If there are zero or one elements remaining, all numbers are partitioned. Return h
        if (l >= h)
            done = true;
        else {
            // Swap numbers[l] and numbers[h], update l and h
            temp = arr[l];
            arr[l] = arr[h];
            arr[h] = temp;
            ++l; --h;
        }
    }
    return h;
}
template <class T>
void my::vector<T>::quickSort(size_t l, size_t h) {
    // Base case: If there are 1 or zero elements to sort, partition is already sorted
    if (l >= h)
        return;
    // Partition the data within the array. Value j returned
    // from partitioning is location of last element in low partition.
    int m = partition(l, h);

    // Recursively sort low partition (l to m) and high partition (m + 1 to h)
    quickSort(l, m);
    quickSort(m + 1, h);
}
template <class T> class my::vector<T>::
vector_iterator<T> my::vector<T>::find(iterator begin, iterator end, const T& val) const {
    int start = begin - this->begin(), stop = end - this->begin();
    for (; start < stop; start++) {
        if (arr[start] == val)
            break;
    }
    iterator it(arr+start);
    return it;
}
template <class T> class my::vector<T>::
vector_iterator<T> my::vector<T>::find_s(iterator begin, iterator end, const T& val) const {
    int mid, low(begin - this->begin()), high((end - this->begin())- 1);

    while (high >= low) {
        mid = (high + low) / 2;
        if (arr[mid] < val && val < arr[mid+1])
            return mid;
        else if (arr[mid] < val)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return end; // not found
}
template<class T>
std::ostream& my::operator << ( std::ostream& os, const my::vector<T>& v ) {
    os << "[ ";
    for (int i = 0; i < v.size(); i++)
        os << v.at(i) << " ";
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
bool operator==(my::vector<T>& v1, my::vector<T>& v2) {
    if (v1.size() != v2.size())
        return false;
    for (int i = 0; i < v1.size(); i++) {
        if (v1.at(i) != v2.at(i))
            return false;
    }
    return true;
}
template <class T>
bool operator!=(my::vector<T>& v1, my::vector<T>& v2) {
    return !(v1 == v2);
}
template <class T>
bool operator<(my::vector<T>& v1, my::vector<T>& v2) {
    return(lexicographicalCompare(v1.begin(), v1.end(), v2.begin(), v2.end()));
}
template <class T>
bool operator<=(my::vector<T>& v1, my::vector<T>& v2) {
    return (v1 == v2 || v1 < v2);
}
template <class T>
bool operator>(my::vector<T>& v1, my::vector<T>& v2) {
    return !(v1 == v2 || v1 < v2);
}
template <class T>
bool operator>=(my::vector<T>& v1, my::vector<T>& v2) {
    return !(v1 < v2);
}

#endif
