#ifndef COMPARE_H
#define COMPARE_H

namespace my {
    int strcmp(const char* lhs, const char* rhs) {
        size_t i;
        for (i = 0; lhs[i] != '\0' && rhs[i] != '\0'; ++i) {
            if (lhs[i] != rhs[i])
                return lhs[i] - rhs[i];
        }
        if (lhs[i] == '\0' && rhs[i] == '\0') return 0;
        if (lhs[i] == '\0') return -1;
        return 1;
    }
    template <class T> struct greater {
        bool operator()(const T& lhs, const T& rhs) const {
            return lhs > rhs;
        }
    };
    template <> struct greater <const char*> {
        bool operator()(const char* lhs, const char* rhs) const {
            return strcmp(lhs, rhs) > 0;
        }
    };
    template <class T> struct less {
        bool operator()(const T& lhs, const T& rhs) const {
            return lhs < rhs;
        }
    };
    template <> struct less <const char*> {
        bool operator()(const char* lhs, const char* rhs) const {
            return strcmp(lhs, rhs) < 0;
        }
    };
    template <class T> struct equals {
        bool operator()(const T& lhs, const T& rhs) const {
            return lhs == rhs;
        }
    };
    template <> struct equals <const char*> {
        bool operator()(const char* lhs, const char* rhs) const {
            return strcmp(lhs, rhs) == 0;
        }
    };
}

#endif