#ifndef STRING_H
#define STRING_H

#include <iostream>
#include <cstdlib>

namespace my {
    size_t strlen(const char* s);
    class string;
    std::istream& getline (std::istream& is, string& st, char delim = '\n');
    double stod (const string& str, size_t* idx = 0);
    float stof (const string& str, size_t* idx = 0) {return float(stod(str, idx));}
    long stol (const string& str, size_t* idx = 0, int base = 10);
    int stoi (const string& str, size_t* idx = 0, int base = 10) {return int(stol(str, idx, base));}
    long double stold (const string&  str, size_t* idx = 0);
    long long stoll (const string&  str, size_t* idx = 0, int base = 10);
    unsigned long stoul (const string&  str, size_t* idx = 0, int base = 10);
    unsigned long long stoull (const string&  str, size_t* idx = 0, int base = 10);
    string operator+ (const string& lhs, const string& rhs);
    string operator+ (const string& lhs, const char*   rhs);
    string operator+ (const char*   lhs, const string& rhs);
    string operator+ (const string& lhs, char          rhs);
    string operator+ (char          lhs, const string& rhs);
    string to_string (int val);
    string to_string (long val);
    string to_string (long long val);
    string to_string (unsigned val);
    string to_string (unsigned long val);
    string to_string (unsigned long long val);
    string to_string (float val);
    string to_string (double val, size_t precision = 6);
    string to_string (long double val, size_t precision = 6);
}
class my::string {
    template <class T> class string_iterator {
        public:
        typedef T value_type;
        typedef T& reference;
        typedef size_t pointer;
        typedef std::random_access_iterator_tag iterator_category;
        typedef size_t difference_type;
        string_iterator(pointer p = 0, const string* st = NULL): ptr(p), s(st) {}
        string_iterator& operator=(const string_iterator& it) {ptr = it.ptr; s = it.s; return *this;} 
        string_iterator& operator+=(int v) {ptr += v; return *this;}
        string_iterator& operator++() {ptr++; return *this;}
        string_iterator operator++(int) {string_iterator it = *this; ++*this; return it;}
        string_iterator operator+(int v) {return string_iterator(ptr+v, s);}
        friend string_iterator operator+(int v, string_iterator& iter)
        {return string_iterator((iter.ptr)+v, iter.s);}
        string_iterator& operator-=(int v) {ptr -= v; return *this;}
        string_iterator& operator--() {ptr--; return *this;}
        string_iterator operator-(int v) {return string_iterator(ptr-v, s);}
        string_iterator operator--(int) {string_iterator it = *this; --*this; return it;}
        difference_type operator-(const string_iterator& it) {return ptr - it.ptr;}
        bool operator<(const string_iterator& it) {return ptr < it.ptr;}
        bool operator<=(const string_iterator& it) {return ptr <= it.ptr;}
        bool operator>(const string_iterator& it) {return ptr > it.ptr;}
        bool operator>=(const string_iterator& it) {return ptr >= it.ptr;}
        bool operator==(const string_iterator& it) {return ptr == it.ptr;}
        bool operator!=(const string_iterator& it) {return ptr != it.ptr;}
        reference operator*() {return s->str[ptr];}
        reference operator[](int v) {return s->str[ptr+v];}

        private:
        pointer ptr;
        const string* s;
    };
    private:
    char* str;
    size_t inUse;
    size_t allocated;
    void growArray(size_t n);
    void replace_help(size_t& pos, size_t& len, size_t& n, size_t& end);

    public:
    static const size_t npos = -1;
    typedef size_t size_type;
    typedef char value_type;
    typedef string_iterator<char> iterator;
    typedef string_iterator<const char> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    string();
    string (const string& st);
    string (const string& st, size_t pos, size_t len = npos);
    string (const char* s, size_t n);
    string (const char* s);
    string (size_t n, char c);
    ~string() {free(str);}
    template <class InputIterator>
    string (InputIterator first, InputIterator last);
    string& operator= (const string& s) {clear(); insert(0, s); return *this;}
    string& operator= (const char* s) {clear(); insert(0, s); return *this;}
    int compare (const string& s) const {return compare(0, inUse, s, 0, s.inUse);}
    int compare (size_t pos, size_t len, const string& s) const
    {return compare(pos, len, s, 0, s.inUse);}
    int compare (size_t pos, size_t len, const string& s, size_t subpos, size_t sublen) const;
    int compare (const char* s) const {return compare(0, inUse, s, my::strlen(s));}
    int compare (size_t pos, size_t len, const char* s) const
    {return compare(pos, len, s, my::strlen(s));}
    int compare (size_t pos, size_t len, const char* s, size_t n) const;
    char& at(size_t i) const;
    char& operator[](size_t i) const {return this->at(i);}
    char& front() const;
    char& back() const;
    const char* data() const {return str;}
    const char* c_str() const {return str;}
    void push_back(const char& val);
    void pop_back();
    string& operator+= (const string& s) {insert(inUse, s); return *this;}
    string& operator+= (const char* s) {insert(inUse, s); return *this;}
    string& operator+= (char c) {push_back(c); return *this;}
    size_t copy (char* s, size_t len, size_t pos = 0) const;
    string& insert (size_t pos, const string& s) {insert(pos, s, 0, s.inUse); return *this;}
    string& insert (size_t pos, const string& s, size_t subpos, size_t sublen);
    string& insert (size_t pos, const char* s) {insert(pos, s, my::strlen(s)); return *this;}
    string& insert (size_t pos, const char* s, size_t n);
    string& insert (size_t pos, size_t n, char c);
    void insert (iterator p, size_t n, char c) {insert(p - begin(), n, c);}
    iterator insert (iterator p, char c)
    {int n = p - begin(); insert(n, 1, c); return iterator(n, this);}
    template <class InputIterator>
    void insert (iterator p, InputIterator first, InputIterator last);
    string& erase (size_t pos = 0, size_t len = npos);
    iterator erase (iterator it) {return erase(it, it+1);}
    iterator erase (iterator first, iterator last);
    string& replace (size_t pos, size_t len, const string& s)
    {replace(pos, len, s.str, s.inUse); return *this;}
    string& replace (iterator i1, iterator i2, const string& s)
    {replace(i1-begin(), i2-i1, s.str, s.inUse); return *this;}
    string& replace (size_t pos, size_t len, const string& s, size_t subpos, size_t sublen)
    {replace(pos, len, s.str, sublen, subpos); return *this;}
    string& replace (size_t pos, size_t len, const char* s)
    {replace(pos, len, s, strlen(s)); return *this;}
    string& replace (iterator i1, iterator i2, const char* s)
    {replace(i1-begin(), i2-i1, s, strlen(s)); return *this;}
    string& replace (size_t pos, size_t len, const char* s, size_t n, size_t subpos = 0);
    string& replace (iterator i1, iterator i2, const char* s, size_t n)
    {replace(i1-begin(), i2-i1, s, n); return *this;}
    string& replace (size_t pos, size_t len,  size_t n, char c);
    string& replace (iterator i1, iterator i2, size_t n, char c)
    {replace(i1-begin(), i2-i1, n, c); return *this;}
    template <class InputIterator>
    string& replace (iterator i1, iterator i2, InputIterator first, InputIterator last);
    size_t find (const string& s, size_t pos = 0) const {return find(s.str, pos, s.inUse);}
    size_t find (const char* s, size_t pos = 0) const {return find(s, pos, strlen(s));}
    size_t find (const char* s, size_t pos, size_t n) const;
    size_t find (char c, size_t pos = 0) const;
    size_t rfind (const string& s, size_t pos = npos) const
    {return rfind(s.str, pos, s.inUse);}
    size_t rfind (const char* s, size_t pos = npos) const
    {return rfind(s, pos, strlen(s));}
    size_t rfind (const char* s, size_t pos, size_t n) const;
    size_t rfind (char c, size_t pos = npos) const;
    size_t find_first_not_of (const string& s, size_t pos = 0) const
    {return find_first_not_of(s.str, pos, s.inUse);}
    size_t find_first_not_of (const char* s, size_t pos = 0) const
    {return find_first_not_of(s, pos, strlen(s));}
    size_t find_first_not_of (const char* s, size_t pos, size_t n) const;
    size_t find_first_not_of (char c, size_t pos = 0) const;
    size_t size() const {return inUse;}
    size_t capacity() const {return allocated;}
    bool empty() const {return inUse == 0;}
    void resize(size_t v, char val = char());
    void reserve(size_t v);
    void shrink_to_fit();
    void swap(string& st);
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
    string substr (size_t pos = 0, size_t len = npos) const;
    void clear();

    #if __cplusplus >= 201103L
    string(string&& s);
    string (std::initializer_list<char> l);
    string& operator=(string&& s);
    void insert (iterator p, std::initializer_list<char> l) {insert(p, l.begin(), l.end());}
    string& replace (iterator i1, iterator i2, std::initializer_list<char> l)
    {replace(i1, i2, l.begin(), l.end()); return *this;}

    #endif
};
#if __cplusplus >= 201103L
my::string::string(string&& s) {
    inUse = s.inUse; s.inUse = 0;
    allocated = s.allocated; s.allocated = 1;
    str = s.str; s.str = (char*)malloc(1); s.str[0] = '\0';
}
my::string::string(std::initializer_list<char> l): inUse(l.size()) {
    allocated = inUse+1;
    str = (char*)malloc(allocated);
    int i = 0;
    for (auto &x : l)
        str[i++] = x;
    str[inUse] = '\0';
}
my::string& my::string::operator=(string&& s) {
    if (this != &s) {
        free(str);
        inUse = s.inUse; s.inUse = 0;
        allocated = s.allocated; s.allocated = 1;
        str = s.str; s.str = (char*)malloc(1); s.str[0] = '\0';
    }
    return *this;
}
#endif
my::string::string(): inUse(0), allocated(1) {
    str = (char*)malloc(1);
    str[0] = '\0';
}
my::string::string(const string& st): inUse(st.size()) {
    allocated = inUse+1;
    str = (char*)malloc(allocated);
    for (int i = 0; i < inUse; i++)
        str[i] = st.str[i];
    str[inUse] = '\0';
}
my::string::string (const string& st, size_t pos, size_t len) {
    if (len == npos)
        len = st.inUse - pos;
    inUse = len;
    allocated = inUse+1;
    str = (char*)malloc(allocated);
    for (int i = 0; i < inUse; i++)
        str[i] = st.str[pos++];
    str[inUse] = '\0';
}
my::string::string (const char* s): inUse(my::strlen(s)) {
    allocated = inUse+1;
    str = (char*)malloc(allocated);
    for (int i = 0; i < inUse; i++)
        str[i] = s[i];
    str[inUse] = '\0';
}
my::string::string (const char* s, size_t n): inUse(n) {
    allocated = inUse+1;
    str = (char*)malloc(allocated);
    for (int i = 0; i < inUse; i++)
        str[i] = s[i];
    str[inUse] = '\0';
}
my::string::string(size_t n, char c): inUse(n) {
    allocated = inUse+1;
    str = (char*)malloc(allocated);
    for (int i = 0; i < inUse; i++)
        str[i] = c;
    str[inUse] = '\0';
}
template <class InputIterator>
size_t distance(InputIterator first, InputIterator& last, std::random_access_iterator_tag) {
    return last - first;
}
template <class InputIterator>
size_t distance(InputIterator first, InputIterator& last, typename std::iterator_traits<InputIterator>::iterator_category) {
    size_t n = 0;
    for (; first != last; ++first)
        n++;
    return n;
}
template <class InputIterator>
my::string::string (InputIterator first, InputIterator last) {
    size_t n = distance(first, last, typename std::iterator_traits<InputIterator>::iterator_category());
    inUse = n;
    allocated = inUse+1;
    n = 0;
    str = (char*)malloc(allocated);
    for (; first != last; ++first)
        str[n++] = *first;
    str[inUse] = '\0';
}
int my::string::compare (size_t pos, size_t len, const string& s, size_t subpos, size_t sublen) const {
    size_t n = (len < sublen)? len: sublen;
    for (size_t i = 0; i < n; i++) {
        if (str[pos] != s.str[subpos])
            return str[pos] - s.str[subpos];
        ++pos; ++subpos;
    }
    return len - sublen;
}
int my::string::compare (size_t pos, size_t len, const char* s, size_t n) const {
    size_t p = (len < n)? len: n;
    for (size_t i = 0; i < p; i++) {
        if (str[pos] != s[i])
            return str[pos] - s[i];
        ++pos;
    }
    return len - n;
}
char& my::string::at(size_t i) const {
    try {
        if (i >= inUse) throw i;
    }
    catch (size_t i) {
        std::cout << "Error in my::string::range_check: index " << i << " is out of range\nthis->size() is " << inUse << "\n";
        _Exit (EXIT_FAILURE);
    }
    return str[i];
}
char& my::string::front() const {
    try {
        if (inUse == 0) throw "Error in my::string::range_check in call to my::string::front()\n String is empty.\n";
    }
    catch (const char* s) {
        std::cout << s;
        _Exit (EXIT_FAILURE);
    }
    return str[0];
}
char& my::string::back() const {
    try {
        if (inUse == 0) throw "Error in my::string::range_check in call to my::string::back()\n String is empty.\n";
    }
    catch (const char* s) {
        std::cout << s;
        _Exit (EXIT_FAILURE);
    }
    return str[inUse-1];
}
void my::string::growArray(size_t n) {
    char* temp = str;
    str = (char*)malloc(allocated+n);
    for (int i = 0; i < inUse; i++)
        str[i] = temp[i];
    str[inUse] = '\0';
    free(temp);
    allocated += n;
}
void my::string::resize(size_t v, char val) {
    if (v == 0) {
        clear(); return;
    }
    int i(0);
    if (v < inUse) {
        char* temp = str;
        allocated = v+1;
        str = (char*)malloc(allocated);
        for (; i < v; i++)
            str[i] = temp[i];
        str[i] = '\0';
        free(temp);
    }
    this->reserve(v);
    if (v > inUse) {
        if (val != char()) {
            for (i = inUse; i < v; i++)
                str[i] = val;
        }
        str[inUse] = val; str[v] = '\0';
    }
    inUse = v;
}
void my::string::reserve(size_t v) {
    if (v > allocated-1)
        growArray(v-allocated+1);
}
void my::string::push_back(const char& val) {
    if (inUse == allocated-1)
        growArray(allocated);
    str[inUse++] = val;
    str[inUse] = '\0';
}
void my::string::pop_back() {
    --inUse;
    str[inUse] = '\0';
    if (inUse == 0)
        clear();
}
size_t my::string::copy (char* s, size_t len, size_t pos) const {
    if (pos >= inUse)
        return 0;
    if (pos+len > inUse)
        len = inUse - pos;
    for (int i = 0; i <= len; ++i)
        s[i] = str[pos++];
    return len;
}
void my::string::replace_help(size_t& pos, size_t& len, size_t& n, size_t& end) {
    size_t diff;
    if (n < len) {
        diff = len - n;
        inUse -= diff;
        for (int i = end; i < inUse; i++)
            str[i] = str[i+diff];
        str[inUse] = '\0';
    }
    else if (n > len) {
        diff = n - len;
        inUse += diff;
        if (inUse > allocated-1) {
            char* temp = str;
            allocated = inUse+1;
            str = (char*)malloc(allocated);
            for (int i = 0; i < pos; i++)
                str[i] = temp[i];
            for (int i = inUse-1; i >= end; i--)
                str[i] = temp[i-diff];
            free(temp);
        }
        else {
            for (int i = inUse-1; i >= end; i--)
                str[i] = str[i-diff];
        }
        str[inUse] = '\0';
    }
}
my::string& my::string::replace (size_t pos, size_t len, size_t n, char c) {
    if (pos >= inUse) return *this;
    size_t end = pos + n;
    replace_help(pos, len, n, end);
    for (; pos < end; ++pos)
        str[pos] = c;
    return *this;
}
my::string& my::string::replace (size_t pos, size_t len, const char* s, size_t n, size_t subpos) {
    if (pos >= inUse) return *this;
    size_t end = pos + n;
    replace_help(pos, len, n, end);
    for (; pos < end; ++pos)
        str[pos] = s[subpos++];
    return *this;
}
template <class InputIterator>
my::string& my::string::replace (iterator i1, iterator i2, InputIterator first, InputIterator last) {
    size_t pos = i1-begin(), len = i2-i1;
    size_t n = distance(first, last, typename std::iterator_traits<InputIterator>::iterator_category());
    size_t end = pos + n;
    replace_help(pos, len, n, end);
    for (; pos < end; ++pos) {
        str[pos] = *first;
        ++first;
    }
    return *this;
}
my::string& my::string::insert (size_t pos, const my::string& s, size_t subpos, size_t sublen) {
    if (subpos >= s.inUse || pos > inUse) return *this;
    size_t n = (subpos + sublen > s.inUse)? s.inUse-pos: sublen;
    int end = pos + n - 1;
    this->resize(inUse+n);
    int i;
    for (i = inUse-1; i > end; i--)
        str[i] = str[i-n];
    for (i = pos; i <= end; i++)
        str[i] = s.str[subpos++];
    return *this;
}
my::string& my::string::insert (size_t pos, const char* s, size_t n) {
    if (pos > inUse) return *this;
    int end = pos + n - 1;
    this->resize(inUse+n);
    int i;
    for (i = inUse-1; i > end; i--)
        str[i] = str[i-n];
    int p(0);
    for (i = pos; i <= end; i++)
        str[i] = s[p++];
    return *this;
}
my::string& my::string::insert (size_t pos, size_t n, char c) {
    if (pos > inUse) return *this;
    int end = pos + n - 1;
    this->resize(inUse+n);
    int i;
    for (i = inUse-1; i > end; i--)
        str[i] = str[i-n];
    for (; i >= pos; i--)
        str[i] = c;
    return *this;
}
template <class InputIterator>
void my::string::insert(iterator it, InputIterator begin, InputIterator end) {
    size_t n = distance(begin, end, typename std::iterator_traits<InputIterator>::iterator_category());
    size_t first = it - this->begin();
    if (first > inUse) return;
    size_t last = first + n - 1;
    this->resize(inUse+n);
    int i;
    for (i = inUse-1; i > last; i--)
        str[i] = str[i-n];
    for (i = first; i <= last; i++) {
        str[i] = *begin;
        ++begin;
    }
}
my::string& my::string::erase (size_t pos, size_t len) {
    if (inUse == 0)
        return *this;
    if (len == npos) len = inUse;
    for (int i = pos; i <= inUse-len; i++)
        str[i] = str[i+len];
    inUse -= len;
    return *this;
}
my::string::iterator my::string::erase(iterator begin, iterator end) {
    if (inUse == 0)
        return iterator(0, this);
    size_t n = end - begin;
    int start = begin - this->begin();
    for (int i = start; i <= inUse-n; i++)
        str[i] = str[i+n];
    inUse -= n;
    return iterator(start, this);
}
size_t my::string::find (const char* s, size_t pos, size_t n) const {
    for (size_t i = pos; i <= inUse - n; i++) {
        size_t j;
        for (j = 0; j < n; j++)
            if (str[i+j] != s[j])
                break;
        if (j == n)
            return i;
    }
    return npos;
}
size_t my::string::rfind (const char* s, size_t pos, size_t n) const {
    if (pos == npos) pos = inUse-1;
    size_t end = n-1;
    for (size_t i = pos; i >= end; i--) {
        size_t j, m = n-1;
        for (j = 0; j < n; j++) {
            if (str[i-j] != s[m--])
                break;
        }
        if (j == n)
            return i-end;
    }
    return npos;
}
size_t my::string::find (char c, size_t pos) const {
    for (; pos < inUse; ++pos) {
        if (str[pos] == c)
            return pos;
    }
    return npos;
}
size_t my::string::rfind (char c, size_t pos) const {
    if (pos == npos) pos = inUse-1;
    for (int i = pos; i >= 0; --i) {
        if (str[i] == c)
            return i;
    }
    return npos;
}
size_t my::string::find_first_not_of (const char* s, size_t pos, size_t n) const {
    for (size_t i = pos; i < inUse; i++) {
        size_t j;
        for (j = 0; j < n; j++)
            if (str[i] == s[j])
                break;
        if (j == n)
            return i;
    }
    return npos;
}
size_t my::string::find_first_not_of (char c, size_t pos) const {
    for (; pos < inUse; ++pos) {
        if (str[pos] != c)
            return pos;
    }
    return npos;
}
void my::string::shrink_to_fit() {
    if (allocated > inUse+1) {
        allocated = inUse+1;
        char* temp = str;
        str = (char*)malloc(allocated);
        for (int i = 0; i <= inUse; i++)
            str[i] = temp[i];
        free(temp);
    }
}
void my::string::swap(string& st) {
    char* t_str = st.str; st.str = str; str = t_str;
    size_t t_inUse = st.inUse; st.inUse = inUse; inUse = t_inUse;
    size_t t_alloc = st.allocated; st.allocated = allocated; allocated = t_alloc;
}
my::string my::string::substr (size_t pos, size_t len) const {
    size_t end = (len == npos)? inUse: pos+len;
    return string(str+pos, str+end);
}
void my::string::clear() {
    free(str);
    inUse = 0; allocated = 1;
    str = (char*)malloc(1);
    str[0] = '\0';
}
std::ostream& operator<<(std::ostream& os, const my::string& st) {
    os << st.data();
    return os;
}
std::istream& operator>> (std::istream& is, my::string& st) {
    st.clear();
    char buffer[75];
    is >> buffer;
    st.insert(0, buffer);
    return is;
}
std::istream& my::getline (std::istream& is, my::string& st, char delim) {
    st.clear();
    char buffer[1001];
    is.getline(buffer, 1001, delim);
    st.insert(0, buffer);
    return is;
}
my::string my::operator+ (const string& lhs, const string& rhs) {
    string s(lhs);
    s.insert(s.size(), rhs);
    return s;
}
my::string my::operator+ (const string& lhs, const char* rhs) {
    string s(lhs);
    s.insert(s.size(), rhs);
    return s;
}
my::string my::operator+ (const char* lhs, const string& rhs) {
    string s(lhs);
    s.insert(s.size(), rhs);
    return s;
}
my::string my::operator+ (const string& lhs, char rhs) {
    string s(lhs);
    s.insert(s.size(), 1, rhs);
    return s;
}
my::string my::operator+ (char lhs, const string& rhs) {
    string s(lhs, 1);
    s.insert(s.size(), rhs);
    return s;
}
bool operator== (const my::string& lhs, const my::string& rhs) {
    return lhs.compare(rhs) == 0;
}
bool operator== (const char* lhs, const my::string& rhs) {
    return rhs.compare(rhs) == 0;
}
bool operator== (const my::string& lhs, const char* rhs) {
    return lhs.compare(rhs) == 0;
}
bool operator!= (const my::string& lhs, const my::string& rhs) {
    return !(lhs == rhs);
}
bool operator!= (const char* lhs, const my::string& rhs) {
    return !(lhs == rhs);
}
bool operator!= (const my::string& lhs, const char* rhs) {
    return !(lhs == rhs);
}
bool operator<  (const my::string& lhs, const my::string& rhs) {
    return lhs.compare(rhs) < 0;
}
bool operator<  (const char* lhs, const my::string& rhs) {
    int i = rhs.compare(lhs);
    return (i == 0 || i > 0)? false: true;
}
bool operator<  (const my::string& lhs, const char* rhs) {
    return lhs.compare(rhs) < 0;
}
bool operator<= (const my::string& lhs, const my::string& rhs) {
    return (lhs < rhs || lhs == rhs);
}
bool operator<= (const char* lhs, const my::string& rhs) {
    return (lhs < rhs || lhs == rhs);
}
bool operator<= (const my::string& lhs, const char* rhs) {
    return (lhs < rhs || lhs == rhs);
}
bool operator>  (const my::string& lhs, const my::string& rhs) {
    return lhs.compare(rhs) > 0;
}
bool operator>  (const char* lhs, const my::string& rhs) {
    int i = rhs.compare(lhs);
    return (i == 0 || i < 0)? false: true;
}
bool operator>  (const my::string& lhs, const char* rhs) {
    return lhs.compare(rhs) > 0;
}
bool operator>= (const my::string& lhs, const my::string& rhs) {
    return (lhs > rhs || lhs == rhs);
}                
bool operator>= (const char* lhs, const my::string& rhs) {
    return (lhs > rhs || lhs == rhs);
}
bool operator>= (const my::string& lhs, const char* rhs) {
    return (lhs > rhs || lhs == rhs);
}
double my::stod (const my::string& str, size_t* idx) {
    char* pEnd;
    double d = strtod (str.data(), &pEnd);
    if (idx != 0)
        *idx = pEnd - str.data();
    return d;
}
long my::stol (const my::string& str, size_t* idx, int base) {
    char* pEnd;
    long i = strtol(str.data(), &pEnd, base);
    if (idx != 0)
        *idx = pEnd - str.data();
    return i;
}
long double my::stold (const my::string&  str, size_t* idx) {
    char* pEnd;
    long double d = strtold (str.data(), &pEnd);
    if (idx != 0)
        *idx = pEnd - str.data();
    return d;
}
long long my::stoll (const my::string&  str, size_t* idx, int base) {
    char* pEnd;
    long long int i = strtoll (str.data(), &pEnd, base);
    if (idx != 0)
        *idx = pEnd - str.data();
    return i;
}
unsigned long my::stoul (const my::string&  str, size_t* idx, int base) {
    char* pEnd;
    unsigned long u = strtoul (str.data(), &pEnd, base);
    if (idx != 0)
        *idx = pEnd - str.data();
    return u;
}
unsigned long long my::stoull (const my::string&  str, size_t* idx, int base) {
    char* pEnd;
    unsigned long long u = strtoul (str.data(), &pEnd, base);
    if (idx != 0)
        *idx = pEnd - str.data();
    return u;
}
size_t my::strlen(const char* s) {
    size_t n = 0;
    for (int i = 0; s[i] != '\0'; ++i) ++n;
    return n;
}
my::string my::to_string (int val) {
    return to_string ((long long)val);
}
my::string my::to_string (long val) {
    return to_string ((long long)val);
}
my::string my::to_string (unsigned val) {
    return to_string ((long long)val);
}
my::string my::to_string (unsigned long val) {
    return to_string ((long long)val);
}
my::string my::to_string (unsigned long long val) {
    return to_string ((long long)val);
}
my::string my::to_string (long long val) {
    long long v = (val < 0)? -val: val;
    char s[50];
    int count = 49;
    s[count--] = '\0';
    s[count--] = '0' + v%10;
    while (v >= 10) {
        v /= 10;
        s[count--] = '0' + v%10;
    }
    if (val < 0)
        s[count] = '-';
    else
        ++count;
    return string(s+count);
}
my::string my::to_string (float val) {
    return to_string ((long double)val);
}
my::string my::to_string (double val, size_t p) {
    return to_string ((long double)val, p);
}
my::string my::to_string (long double val, size_t p) {
    long long i = (long long)val;
    val = (val < 0)? (-val)+i: val - i;
    char s[p+1];
    int count = 0;
    for (int i = 0; i < p; i++) {
        val *= 10;
        s[count++] = '0' + (long long)val %10;
    }
    s[count] = '\0';
    return (to_string(i) + '.' + s);
}

#endif
