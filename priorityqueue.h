#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include <iostream>
#include "binaryheap.h"

namespace my {
    template <class T, class C = my::greater<T> > class priority_queue: public my::binary_heap<T,C> {
        public:
        priority_queue(const C& comp = C()): my::binary_heap<T,C>(comp) {}
        template <class InputIterator>
        priority_queue (InputIterator begin, InputIterator end, const C& comp = C()): my::binary_heap<T,C>(comp) {
            my::binary_heap<T,C>::insert(begin, end);
        }
    };
}

#endif