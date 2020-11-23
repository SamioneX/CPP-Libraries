#ifndef _ISTRING_H
#define _ISTRING_H

#include <iostream>
#include <type_traits>
#include <cstring>
#include <atomic>
#if __has_include(<string_view>)
#include <string_view>
#endif


/*
 * This is a basic implementation for a null-terminated immutable reference counted string.
 * We assume that any char array is a string literal. Unfortunately C++ does not differentiate this.
 * Small strings and string literals are never dynamically allocated.
 * Only large strings obtained from operations like concatenation or strings from other sources like std::string are heap allocated.
 * The reference counting is atomic, to enable thread safety.
 */

#define ENABLE_IF(...) \
  typename std::enable_if<(__VA_ARGS__)>::type* = nullptr
#define ENABLE_IF_V(...) \
    ENABLE_IF(std::conjunction<__VA_ARGS__>::value)

namespace so {
    class IString;

    namespace detail {
        template<class T>
        struct is_string_literal : std::false_type {};

        template <size_t N>
        struct is_string_literal<char(&)[N]> : std::true_type {};

        template<class T>
        constexpr bool should_pass = !is_string_literal<T>::value && !std::is_same_v<IString, std::remove_cvref_t<T>>;
    }

    struct Slice {
        const char* str;
        size_t len;

        Slice(const char* s, size_t n): str(s), len(n) {}

        template<size_t N>
        Slice(const char(&s)[N]) {
            str = s;
            len = N-1;
        }

        template<class T, ENABLE_IF(!detail::is_string_literal<T>::value &&
                                    std::is_same_v<const char*, T> || std::is_same_v<char*, T>)>
        Slice(T s) {
            std::cout << "using strlen\n";
            str = (const char*) s;
            len = strlen((const char*) s);
        }

        Slice(const std::string& s) {
            str = s.data();
            len = s.length();
        }

#if __cpp_lib_string_view
        Slice(const std::string_view& s) {
            str = s.data();
            len = s.length();
        }
#endif

        friend std::ostream &operator<<(std::ostream &os, const Slice &slice) {
            os.write(slice.str, slice.len);
            return os;
        }
    };

    class IString {
        static constexpr size_t N = sizeof(size_t);
        static constexpr size_t MAX_SMALL_STRING = 2*N-1;
        static constexpr size_t COUNT_LENGTH = (N * 8) - 2;

        struct SmallString {
            char str[MAX_SMALL_STRING];
            uint8_t count : 6;
            uint8_t dummy : 2;
        };
        struct HeapString {
            std::atomic_size_t ref_count;
            char str[];
        };
        struct LargeString {
            union {
                const char* str;
                HeapString* ptr;
            };
            size_t count : COUNT_LENGTH;
            size_t isLarge : 1;
            size_t isRc : 1;
        };
        struct Empty {
            size_t dummy1;
            size_t dummy2;
        };
        union {
            SmallString small;
            LargeString large;
            Empty empty;
        };

    public:
        static constexpr size_t MAX_SIZE = size_t(1) << COUNT_LENGTH;
        typedef const char* iterator;
        typedef std::reverse_iterator<iterator> reverse_iterator;

        IString() {
            empty = {0, 0};
        }

        IString(const char(&)[1]) {
            empty = {0, 0};
        }

        template<size_t P, ENABLE_IF(P > 1 && P <= MAX_SMALL_STRING)>
        constexpr IString(const char(&s)[P]) {
            for (int i = 0; i < P; ++i) {
                small.str[i] = s[i];
            }
            small.count = P-1;
            small.dummy = 0;
        }

        template<size_t P, ENABLE_IF(P > MAX_SMALL_STRING)>
        constexpr IString(const char(&s)[P]) {
            large.str = s;
            large.count = P-1;
            large.isLarge = true;
            large.isRc = false;
        }

#define RC_SETUP(...) \
        char* s;      \
        if (__VA_ARGS__ < MAX_SMALL_STRING) {   \
            s = small.str;      \
            small.count = __VA_ARGS__;  \
            small.dummy = 0;    \
        }else { \
            auto* obj = (HeapString*) malloc(sizeof(HeapString) + sizeof(char) * ((__VA_ARGS__) + 1));   \
            if (!obj) { \
                throw std::bad_alloc(); \
            }   \
            obj->ref_count = 1; \
            s = obj->str;   \
            large.ptr = obj;    \
            large.count = __VA_ARGS__;   \
            large.isLarge = true;    \
            large.isRc = true;  \
        }

        template<class... Args, ENABLE_IF(sizeof...(Args) > 1 && (std::is_convertible_v<Args, Slice> && ...))>
        IString(Args&&... args) {
            Slice slices[] = {args...};
            size_t total_length = 0;
            for (auto& slice : slices) {
                total_length += slice.len;
            }
            RC_SETUP(total_length)
            for (auto& slice : slices) {
                memcpy(s, slice.str, slice.len);
                s += slice.len;
            }
            *s = '\0';
        }

        template<class T, ENABLE_IF(detail::should_pass<T> && std::is_convertible_v<T, Slice>)>
        IString(const T& arg) {
            Slice slice = arg;
            RC_SETUP(slice.len)
            memcpy(s, slice.str, slice.len);
            s[slice.len] = '\0';
        }

#undef RC_SETUP
#define COPY_ASSIGN \
        large = other.large;    \
        if (other.large.isRc) { \
            ++large.ptr->ref_count; \
        }
#define MOVE_ASSIGN \
        large = other.large;    \
        other.empty = {0, 0};

        IString(const IString& other) {
            COPY_ASSIGN
        }

        IString(IString&& other) {
            MOVE_ASSIGN
        }

        IString& operator=(const IString& other) {
            this->~IString();
            COPY_ASSIGN
            return *this;
        }

        IString& operator=(IString&& other)  noexcept {
            this->~IString();
            MOVE_ASSIGN
            return *this;
        }

#undef COPY_ASSIGN
#undef MOVE_ASSIGN

        ~IString() {
            if (large.isRc) {
                if (--large.ptr->ref_count == 0) {
                    free(large.ptr);
                }
            }
        }

        operator Slice() const {
            if (large.isLarge) {
                if (large.isRc) {
                    return {large.ptr->str, large.count};
                }else {
                    return {large.str, large.count};
                }
            }else {
                return {small.str, small.count};
            }
        }

#if __cpp_lib_string_view
        explicit operator std::string_view() const {
            Slice slice = *this;
            return {slice.str, slice.len};
        }
#endif

        template<class T, ENABLE_IF(std::is_convertible_v<T, Slice>)>
        IString operator+(const T& other) const {
            return IString(*this, other);
        }

        [[nodiscard]] constexpr size_t size() const {
            return large.isLarge? large.count : small.count;
        }

        [[nodiscard]] constexpr size_t length() const {
            return large.isLarge? large.count : small.count;
        }

        [[nodiscard]] constexpr bool isHeapAllocated() const {
            return large.isRc;
        }

        char operator[](size_t i) const {
            Slice s = *this;
            if (i < s.len)
                return s.str[i];
            throw std::out_of_range("In IString::operator[]: Index is out of range");
        }

        [[nodiscard]] char get_unchecked(size_t i) const {
            return Slice(*this).str[i];
        }

        [[nodiscard]] iterator begin() const {
            return Slice(*this).str;
        }

        [[nodiscard]] iterator end() const {
            Slice slice = *this;
            return slice.str + slice.len;
        }

        [[nodiscard]] bool isEmpty() const {
            return !(large.isLarge? large.count : small.count);
        }

        [[nodiscard]] reverse_iterator rbegin() const {
            return reverse_iterator(this->end());
        }

        [[nodiscard]] reverse_iterator rend() const {
            return reverse_iterator(this->begin());
        }

        friend std::ostream &operator<<(std::ostream &os, const IString &iString) {
            return os << (Slice)iString;
        }
    };
}

#endif //_ISTRING_H
