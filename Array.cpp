// Type your code here, or load an example.

#include <iostream>
#include <stdexcept>
#include <type_traits>

template <class T, size_t I>
struct VectorHead {
    T m_data;

    template <class U>
    VectorHead(U&& u): m_data(std::forward<U>(u)) {}

    static constexpr T& data(VectorHead& hd) {
        return hd.m_data;
    }

    static constexpr const T& data(const VectorHead& hd) {
        return hd.m_data;
    }
};



template <class T, size_t N>
struct Vector : VectorHead<T, N>, Vector<T, N-1> {
    typedef VectorHead<T, N> Head;
    typedef Vector<T, N-1> Tail;

    static constexpr T& head(Vector& v) {
        return Head::data(v);
    }

    static constexpr const T& head(const Vector& v) {
        return Head::data(v);
    }

    static constexpr Tail& tail(Vector& v) {
        return v;
    }

    static constexpr const Tail& tail(const Vector& v) {
        return v;
    }

    template<class U, class... Rest>
    constexpr Vector(U&& u, Rest&&... rest): Head(std::forward<U>(u)), Tail(std::forward<Rest>(rest)...) {}

    constexpr T& operator[](size_t i) {
        if (i == 0) return head(*this);
        return tail(*this)[i-1];
    }

    constexpr const T& operator[](size_t i) const {
        if (i == 0) return head(*this);
        return tail(*this)[i-1];
    }

};

template <class T>
struct Vector<T, 0> {
    constexpr T& operator[](size_t) {
        throw std::out_of_range("in Vector::at: index is out of range");
    }

    constexpr T& operator[](size_t i) const {
        throw std::out_of_range("in Vector::at: index is out of range");
    }
};

template<class T, size_t N>
std::ostream& operator<<(std::ostream& os, const Vector<T, N>& vec) {
    os << '[';
    if constexpr (N != 0) {
        os << vec[0];
        for(size_t i = 1; i < N; ++i) {
            os << ", " << vec[i];
        }
    }
    return os << ']';
}

template <class U, class... Rest>
Vector(U&&, Rest&&...) -> Vector<std::common_type_t<U, Rest...>, 1 + sizeof...(Rest)>;


template<class T, size_t N>
__declspec(noinline) T sum(const Vector<T, N>& vec)  {
    T result = 0;
    for (size_t i = 0; i < N; ++i) {
        result += vec[i];
    }
    return result;
}

template<class T, size_t D>
class AbstractArray {};

template<class T, size_t I, size_t... Is>
class Array : Vector<T, (I * ... * Is)>, AbstractArray<T, 1 + sizeof...(Is)> {};

int main() {
    auto v = Vector(1, 2, 3, 4, 5);

    std::cout << v << std::endl;

    std::cout << "sum: " << sum(v) << std::endl;

    return 0;
}
