#ifndef TUPLE_H
#define TUPLE_H

#include <iostream>

#if __cplusplus >= 201103L
namespace my {
    template <size_t i, typename T>
    struct tuple_base {
        T data;
        constexpr tuple_base() = default;
        constexpr tuple_base(const tuple_base&) = default;
        constexpr tuple_base(tuple_base&&) = default;
        constexpr tuple_base(const T& val): data(val) {}
        template <typename U>
        constexpr tuple_base(U&& val): data(std::forward<U>(val)) {}
        static constexpr T& head(tuple_base& t) {return t.data;}
        static constexpr const T& head(const tuple_base& t) {return t.data;}
    };
    // tuple helper class
    template <size_t i, typename... T> struct tuple_impl;

    // Recursive implementation: we store the head element and then derive the rest.
    // All recursive steps end in the base case's function
    template <size_t i, typename T, typename... Rest>
    struct tuple_impl<i, T, Rest...> : public tuple_impl<i+1, Rest...>, private tuple_base<i,T> {
        template<size_t, typename...> friend class tuple_impl;
        typedef tuple_impl<i+1, Rest...> next;
        typedef tuple_base<i,T> elem;

        //get reference to the element stored in this' tuple_base
        static constexpr T& head(tuple_impl& t) { return elem::head(t);}
        static constexpr const T& head(const tuple_impl& t) { return elem::head(t);}
        //get reference the next tuple_impl derived from this at index i+1
        static constexpr next& tail(tuple_impl& t) { return t; }
        static constexpr const next& tail(const tuple_impl& t) { return t; }

        constexpr tuple_impl(): next(), elem() {};
        constexpr tuple_impl(const tuple_impl&) = default;

        constexpr tuple_impl(tuple_impl&& t): next(std::move(tail(t))), elem(std::forward<T>(head(t))) {}

        constexpr explicit tuple_impl(const T& t, const Rest&... rest): next(rest...), elem(t) {}

        /*--------------Constructing from other types----------*/
        //check to make sure size of the arguments matches with the size of the tuple
        template <typename UT, typename... URest, typename = typename std::enable_if
        <sizeof...(URest) == sizeof...(Rest)>::type>
        constexpr explicit tuple_impl(UT&& t, URest&&... rest): next(std::forward<URest>(rest)...), elem(std::forward<UT>(t)) {}

        template<typename... UTypes>
        constexpr tuple_impl(const tuple_impl<i, UTypes...>& t):
        next(tuple_impl<i, UTypes...>::tail(t)), elem(tuple_impl<i, UTypes...>::head(t)) {}

        template<typename UT, typename... URest>
        constexpr tuple_impl(tuple_impl<i, UT, URest...>&& t): next(std::move(tuple_impl<i, T, URest...>::tail(t))),
        elem(std::forward<UT> (tuple_impl<i, UT, URest...>::head(t))) {}

        /*----assignment operators: assign the head and recurse for the tail----*/
        tuple_impl& operator=(const tuple_impl& t) {
            head(*this) = head(t);
            tail(*this) = tail(t);
            return *this;
        }
        tuple_impl& operator=(tuple_impl&& t) {
            head(*this) = std::forward<T>(head(t));
            tail(*this) = std::move(tail(t));
            return *this;
        }
        template<typename... UTypes>
        tuple_impl& operator=(const tuple_impl<i, UTypes...>& t) {
            head(*this) = tuple_impl<i, UTypes...>::head(t);
            tail(*this) = tuple_impl<i, UTypes...>::tail(t);
            return *this;
        }
        template<typename UT, typename... URest>
        tuple_impl& operator=(tuple_impl<i, UT, URest...>&& t) {
            head(*this) = std::forward<UT>(tuple_impl<i, UT, URest...>::head(t));
            tail(*this) = std::move(tuple_impl<i, UT, URest...>::tail(t));
            return *this;
        }
        protected:
        //swap the heap and recurse for tail
        void impl_swap(tuple_impl& t) {
            std::swap(head(*this), head(t));
            next::impl_swap(tail(t));
        }
    };

    //base case:
    template <size_t i, typename T>
    struct tuple_impl<i, T> : private tuple_base<i,T> {
        template<size_t, typename...> friend class tuple_impl;
        typedef tuple_base<i,T> elem;

        static constexpr T& head(tuple_impl& t) {return elem::head(t);}
        static constexpr const T& head(const tuple_impl& t) {return elem::head(t);}

        constexpr tuple_impl(): elem() {}
        constexpr tuple_impl(const T& val): elem(val) {}
        template<typename U>
        constexpr tuple_impl(U&& val): elem(std::forward<U>(val)) {}
        constexpr tuple_impl(const tuple_impl&) = default;
        constexpr tuple_impl(tuple_impl&& t): elem(std::forward<T>(head(t))) {}
        template<typename U>
        constexpr tuple_impl(const tuple_impl<i, U>& t): elem(tuple_impl<i, U>::head(t)) {}
        template<typename U>
        constexpr tuple_impl(tuple_impl<i, U>&& t): elem(std::forward<U>(tuple_impl<i, U>::head(t))) {}
        tuple_impl& operator=(const tuple_impl& t) {
            head(*this) = head(t);
            return *this;
        }
        tuple_impl& operator=(tuple_impl&& t) {
            head(*this) = std::forward<T>(head(t));
            return *this;
        }
        template<typename U>
        tuple_impl& operator=(const tuple_impl<i, U>& t) {
            head(*this) = tuple_impl<i, U>::head(t);
            return *this;
        }
        template<typename U>
        tuple_impl& operator=(tuple_impl<i, U>&& t) {
            head(*this) = std::forward<U>(tuple_impl<i, U>::head(t));
            return *this;
        }
        protected:
        void impl_swap(tuple_impl& t) {
            std::swap(head(*this), head(t));
        }
    };

    /*--------------------------------tuple class----------------------------------*/
    template<typename... Ts>
    class tuple: public tuple_impl<0, Ts...> {
        typedef tuple_impl<0, Ts...> base_t;
        public:
        constexpr tuple(): base_t() {}
        constexpr tuple(const tuple&) = default;
        constexpr tuple(tuple&&) = default;
        constexpr tuple(const Ts&... elems): base_t(elems...) {}
        template<typename... UTypes, typename std::enable_if<sizeof...(Ts) >= 1 && sizeof...(Ts) == sizeof...(UTypes)>::type>
        constexpr tuple(UTypes&&... elems): base_t(std::forward<UTypes>(elems)...) {}
        template<typename... UTypes>
        constexpr tuple(const tuple<UTypes...>& t): base_t(static_cast<const tuple_impl<0, UTypes...>&>(t)) {}
        template<typename... UTypes>
        constexpr tuple(tuple<UTypes...>&& t): base_t(static_cast<tuple_impl<0, UTypes...>&&>(t)) {}

        tuple& operator=(const tuple& t) {
            static_cast<base_t&>(*this) = t;
            return *this;
        }
        tuple& operator=(tuple&& t) {
            static_cast<base_t&>(*this) = std::move(t);
            return *this;
        }
        template<typename... UTypes> typename std::enable_if<sizeof...(Ts) == sizeof...(UTypes), tuple&>::type
        operator=(const tuple<UTypes...>& t) {
            static_cast<base_t&>(*this) = t;
            return *this;
        }
        template<typename... UTypes> typename std::enable_if<sizeof...(Ts) == sizeof...(UTypes), tuple&>::type
        operator=(tuple<UTypes...>&& t) {
            static_cast<base_t&>(*this) = std::move(t);
            return *this;
        }
        void swap(tuple& t) {base_t::impl_swap(t);}
    };

    //explicit specialization for empty tuple
    template<>
    class tuple<> {
        public:
        void swap(tuple&) {}
        tuple() = default;
    };

    //partial specialization for tuple of 2 types:
    //optimized for construction and assignment from a pair
    template<typename T1, typename T2>
    class tuple<T1, T2> : public tuple_impl<0, T1, T2> {
        typedef tuple_impl<0, T1, T2> base_t;
        public:
        constexpr tuple(): base_t() {}
        constexpr tuple(const tuple&) = default;
        constexpr tuple(tuple&&) = default;
        constexpr tuple(const T1& t1, const T1& t2): base_t(t1, t2) {}
        template<typename U1, typename U2>
        constexpr tuple(U1 u1, U2 u2): base_t(std::forward<U1>(u1), std::forward<U2>(u2)) {}
        template<typename U1, typename U2>
        constexpr tuple(const tuple<U1, U2>& t): base_t(static_cast<const tuple_impl<0, U1, U2>&>(t)) {}
        template<typename U1, typename U2>
        constexpr tuple(tuple<U1, U2>&& t): base_t(static_cast<tuple_impl<0, U1, U2>&&>(t)) {}
        template<typename U1, typename U2>
        constexpr tuple(const std::pair<U1, U2>& p): base_t(p.first, p.second) {}
        template<typename U1, typename U2>
        constexpr tuple(std::pair<U1, U2>&& p): base_t(std::forward<U1>(p.first), std::forward<U2>(p.second)) {}

        tuple& operator=(const tuple& t) {
            static_cast<base_t&>(*this) = t;
            return *this;
        }
        tuple& operator=(tuple&& t) {
            static_cast<base_t&>(*this) = std::move(t);
            return *this;
        }
        template<typename U1, typename U2>
        tuple& operator=(const tuple<U1, U2>& t) {
            static_cast<base_t&>(*this) = t;
            return *this;
        }
        template<typename U1, typename U2>
        tuple& operator=(tuple<U1, U2>&& t) {
            static_cast<base_t&>(*this) = std::move(t);
            return *this;
        }
        template<typename U1, typename U2>
        tuple& operator=(const std::pair<U1, U2>& p) {
            this->head(*this) = p.first;
            this->tail(*this).head(*this) = p.second;
        }
        template<typename U1, typename U2>
        tuple& operator=(std::pair<U1, U2>&& p) {
            this->head(*this) = std::forward<U1>(p.first);
            this->tail(*this).head(*this) = std::forward<U2>(p.second);
        }
        void swap(tuple& t) {base_t::impl_swap(t);}
    };

    /*--------------tuple_size class-------------*/
    template<typename T> struct tuple_size;
    template<typename... Ts>
    struct tuple_size<tuple<Ts...> >: public std::integral_constant<size_t, sizeof...(Ts)> {};

    /*--------------tuple_element class-------------*/
    template <size_t i, class T> class tuple_element;

    //specialization
    //Recursive: strip off the head of each tuple recursively while
    //counting i down. Recursion stops when i == 0 (we've reached the element we're seeking)
    //or there are no more types (error case: i is out of range)
    template<size_t i, typename T, typename... Rest>
    struct tuple_element<i, tuple<T, Rest...> >: tuple_element<i - 1, tuple<Rest...> > {};

    //base case: i == 0;
    template<typename T, typename... Rest>
    struct tuple_element<0, tuple<T, Rest...> > {
        typedef T type;
    };

    //error case: out of range index
    template<size_t i>
    struct tuple_element<i, tuple<> > {
        static_assert(i < tuple_size<tuple<>>::value, "tuple::out_of_range check");
    };

    /*-------------------------------get functions------------------------*/
    template<size_t i, typename T, typename... Rest>
    constexpr T& get_helper(tuple_impl<i, T, Rest...>& t) {
        return tuple_impl<i, T, Rest...>::head(t);
    }
    template<size_t i, typename T, typename... Rest>
    constexpr const T& get_helper(const tuple_impl<i, T, Rest...>& t) {
        return tuple_impl<i, T, Rest...>::head(t);
    }

    //return a reference to the ith element of a tuple
    template<size_t i, typename... Ts>
    constexpr typename tuple_element<i, tuple<Ts...>>::type& get(tuple<Ts...>& t) {
        return get_helper<i>(t);
    }
    //return a const reference to the ith element of a const tuple
    template<size_t i, typename... Ts>
    constexpr const typename tuple_element<i, tuple<Ts...>>::type& get(const tuple<Ts...>& t) {
        return get_helper<i>(t);
    }
    //return an rvalue reference to the ith element of a tuple rvalue
    template<size_t i, typename... Ts>
    constexpr typename tuple_element<i, tuple<Ts...>>::type&& get(tuple<Ts...>&& t) {
        return std::forward<tuple_element<i, tuple<Ts...>>&&>(get<i>(t));
    }

    // get<T> functions
    template<typename T, size_t i, typename... Rest>
    constexpr T& get_helper2(tuple_impl<i, T, Rest...>& t) {
        return tuple_impl<i, T, Rest...>::head(t);
    }
    template<typename T, size_t i, typename... Rest>
    constexpr const T& get_helper2(const tuple_impl<i, T, Rest...>& t) {
        return tuple_impl<i, T, Rest...>::head(t);
    }
    //return a reference to a unique element of type T in a tuple
    template <typename T, typename... Ts>
    constexpr T& get(tuple<Ts...>& t) {return get_helper2<T>(t);}

    //return a const reference to a unique element of type T in a const tuple
    template <typename T, typename... Ts>
    constexpr T& get(const tuple<Ts...>& t) {return get_helper2<T>(t);}

    //return an rvalue reference to a unique element of type T in a tuple rvalue
    template <typename T, typename... Ts>
    constexpr T&& get(tuple<Ts...>&& t) {return std::forward<T&&>(get_helper2<T>(t));}

    /*-----------------------------tuple comparison functions-------------------------*/

    //Recursive comparison functions: recursion ends in the base case when i == size
    template<typename lhs, typename rhs, size_t i, size_t size>
    struct tuple_compare {
        //compare the head element, and recurse for the rest
        static constexpr bool equals(const lhs& l, const rhs& r) {
            return bool(get<i>(l) == get<i>(r)) && tuple_compare<lhs, rhs, i + 1, size>::equals(l, r);
        }
        //check if head element of lhs is less than or equal to that rhs
        //recursion ends if get<i>(l) < get<i>(r) (returns true) or get<i>(r) < get<i>(l) (returns false)
        //else it ends in base case (returns false)
        static constexpr bool less(const lhs& l, const rhs& r) {
            return bool(get<i>(l) < get<i>(r))|| (!bool(get<i>(r) < get<i>(r))
                    && tuple_compare<lhs, rhs, i + 1, size>::less(l, r));
        }
    };
    //specialization for base case
    template<typename lhs, typename rhs, size_t size>
    struct tuple_compare<lhs, rhs, size, size> {
        // empty tuples are equal so return true
        static constexpr bool equals(const lhs& l, const rhs& r) { return true;}
        // an empty tuple is not less than an empty tuple
        static constexpr bool less(const lhs& l, const rhs& r) {return false;}
    };
    template<typename... Ts, typename... Us>
    constexpr bool operator==(const tuple<Ts...>& t, const tuple<Us...>& u) {
        static_assert(sizeof...(Ts) == sizeof...(Us), "tuple sizes must be equal to compare tuples.");
        return tuple_compare< tuple<Ts...>, tuple<Us...>, 0, sizeof...(Ts)>::equals(t, u);
    }
    template<typename... Ts, typename... Us>
    constexpr bool operator<(const tuple<Ts...>& t, const tuple<Us...>& u) {
        static_assert(sizeof...(Ts) == sizeof...(Us), "tuple sizes must be equal to compare tuples.");
        return tuple_compare< tuple<Ts...>, tuple<Us...>, 0, sizeof...(Ts)>::less(t, u);
    }
    template<typename... Ts, typename... Us>
    constexpr bool operator!=(const tuple<Ts...>& t, const tuple<Us...>& u) {return !(t == u);}

    template<typename... Ts, typename... Us>
    constexpr bool operator>(const tuple<Ts...>& t, const tuple<Us...>& u) {return u < t;}

    template<typename... Ts, typename... Us>
    constexpr bool operator<=(const tuple<Ts...>& t, const tuple<Us...>& u) {return !(u < t);}

    template<typename... Ts, typename... Us>
    constexpr bool operator>=(const tuple<Ts...>& t, const tuple<Us...>& u) {return !(t < u);}

    /*--------------------------other tuple functions------------------------*/
    template<typename... Ts>
    constexpr tuple<typename std::remove_cv<Ts>::type...> make_tuple(Ts&&... args) {
        return tuple<typename std::remove_cv<Ts>::type...>(std::forward<Ts>(args)...);
    }
    template<typename... Ts> constexpr tuple<Ts&&...> forward_as_tuple(Ts&&... args) {
        return tuple<Ts&&...>(std::forward<Ts>(args)...);
    }
    template<typename... Ts> constexpr tuple<Ts&...> tie(Ts&... args){
        return tuple<Ts&...>(args...);
    }
    /*-------------------------------tuple_cat---------------------------*/
    template<typename Tuple>
    struct strip_tuple {
        typedef typename std::remove_cv <typename std::remove_reference<Tuple>::type>::type type;
    };

    // Combines several tuples into a single one.
    template<typename...>
    struct combine_tuples;

    //base case: no tuple to combine, "type" is tuple<>
    template<>
    struct combine_tuples<> { typedef tuple<> type;};

    //takes a tuple with Ts elements and calls it "type"
    template<typename... Ts>
    struct combine_tuples<tuple<Ts...>> {
        typedef tuple<Ts...> type;
    };
    //recursive case:
    //1. let T1s... and T2s... be the types of the first 2 tuples and Rem... be the remaining tuples
    //2. Combine T1s... and T2s... to a single tuple, A. A now has types (T1s..., T2s...). Recurse for rem
    //3. If Rem is empty (i.e there are no more tuples to be concatenated, we stop. "type" is now tuple<T1s..., T2s...>
    //4. Else T1s... now becomes the type of A, and T2s becomes the type of the next tuple. 
    template<typename... T1s, typename... T2s, typename... Rem>
    struct combine_tuples<tuple<T1s...>, tuple<T2s...>, Rem...> {
        typedef typename combine_tuples<tuple<T1s..., T2s...>, Rem...>::type type;
    };

    //returns a the type of a tuple containing all elements in each of the tuples being concatenated
    template<typename... Tpls>
    struct tuple_cat_result {
        typedef typename combine_tuples <typename strip_tuple<Tpls>::type...>::type type;
    };
    /*----------------------  metafunctions used to make indices-----------------------*/
    //structure to store a sequence of indices
    template <size_t... indices>
    struct index_sequence {
        using type = index_sequence<indices...>;
    };
    // add an index to a current sequence of indices
    template <size_t i, class sequence>
    struct cat_index_sequence;

    //specialization
    template <size_t i, size_t... indices>
    struct cat_index_sequence<i, index_sequence<indices...>>
    : index_sequence<indices..., i> {};

    //recursive metafunction to make the indices. Recurse till N == 1 (i.e index_sequence<0>) 
    //and then calls cat_index_sequence recursively, which appends, 1, 2, ..., N-1 to the index_sequence
    template <size_t N>
    struct make_index_sequence
    : cat_index_sequence<N-1, typename make_index_sequence<N-1>::type>::type {};

    //base case
    template <>
    struct make_index_sequence<1> : index_sequence<0> {};

    /*----------------------------------------------------------------------------------*/
    //metafunction to make indices of each of the tuples
    template<typename...>
    struct make_1st_indices;

    //empty case
    template<>
    struct make_1st_indices<> {
        typedef index_sequence<> type;
    };
    //strip out the first tuple and build its indices
    template<typename Tp, typename... Tpls>
    struct make_1st_indices<Tp, Tpls...> {
        typedef typename make_index_sequence<tuple_size<typename std::remove_reference<Tp>::type>::value>::type type;
    };

    // This performs the actual concatenation. When cat is called the first time,
    // sizeof...(us) is 0, when it's called for next, "us" becomes the elements in the first tuple
    // obtained through get. Base case is reached when there are no more tuples in the arguments list.
    template<typename Ret, typename indices, typename... Tpls>
    struct tuple_concater;

    //specialization:
    template<typename Ret, size_t... indices, typename Tp, typename... Tpls>
    struct tuple_concater<Ret, index_sequence<indices...>, Tp, Tpls...> {
        template<typename... Us>
        static constexpr Ret cat(Tp&& tp, Tpls&&... tps, Us&&... us) {
            typedef typename make_1st_indices<Tpls...>::type idx;
            typedef tuple_concater<Ret, idx, Tpls...> next;
            return next::cat(std::forward<Tpls>(tps)..., std::forward<Us>(us)..., get<indices>(std::forward<Tp>(tp))...);
        }
    };
    //base case:
    template<typename Ret>
    struct tuple_concater<Ret, index_sequence<>> {
        template<typename... Us>
        static constexpr Ret cat(Us&&... us) {return Ret(std::forward<Us>(us)...);}
    };

    template<typename... Tpls>
    constexpr auto tuple_cat(Tpls&&... tpls) -> typename tuple_cat_result<Tpls...>::type {
        typedef typename tuple_cat_result<Tpls...>::type ret;
        typedef typename make_1st_indices<Tpls...>::type idx;
        typedef tuple_concater<ret, idx, Tpls...> concater;
        return concater::cat(std::forward<Tpls>(tpls)...);
    }

    /*---------------ignore object---------------*/
    //swallows the assignment operation on an ignore object
    struct swallow_assign {
        template<class Tp>
        constexpr const swallow_assign& operator=(const Tp&) const{ return *this; }
    };

    constexpr swallow_assign ignore{};
}
#endif

#endif
