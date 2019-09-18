#ifndef XORLIST_H
#define XORLIST_H

#include <iostream>
#include <inttypes.h>
#include <stdexcept>

template <class T>
class XORList {
    private:
    class Node {
        public:
        T data;
        Node* npx;

        Node(T val) {
            data = val;
            npx = NULL;
        }
        static Node* XOR(Node* next, Node* prev) {
            return (Node*) ((uintptr_t)next ^ (uintptr_t)prev);
        }
    };
    template <class U>
    class list_iterator {
        friend class XORList;
        public:
            typedef U value_type;
            typedef U& reference;
            typedef U* pointer;
            typedef std::bidirectional_iterator_tag iterator_category;
            typedef void difference_type;
            list_iterator(Node* c = NULL, Node* p = NULL, Node* n = NULL) {
                hd = c; prev = p; next = n;
            }
            list_iterator& operator=(const list_iterator& it) {
                hd = it.hd; next = it.next; prev = it.prev;
                return *this;
            }
            list_iterator& operator++() {
                this->advance();
                return *this;
            }
            list_iterator operator++(int) {
                list_iterator it(hd, prev, next); ++*this; return it;
            }
            list_iterator& operator--() {
                this->retreat();
                return *this;
            }
            list_iterator operator--(int) {
                list_iterator it(hd, prev, next); ++*this; return it;
            }
            bool operator==(const list_iterator& it) const {return hd == it.hd;}
            bool operator!=(const list_iterator& it) const {return hd != it.hd;}
            reference operator*() const {return hd->data;}
            pointer operator->() const { return &(hd->data);}
        protected:
            Node* hd;
            Node* prev;
            Node* next;
            virtual void advance() {
                prev = hd;
                hd = next;
                if (next != NULL)
                    next = Node::XOR(prev, next->npx);
            }
            virtual void retreat() {
                next = hd;
                hd = prev;
                if (prev != NULL)
                    prev = Node::XOR(next, prev->npx);
            }
    };
    template <class U>
    class reverse_list_iterator: public list_iterator<U> {
        protected:
        void advance() {
            this->next = this->hd;
            this->hd = this->prev;
            if (this->prev != NULL)
                this->prev = Node::XOR(this->next, this->prev->npx);
        }
        void retreat() {
            this->prev = this->hd;
            this->hd = this->next;
            if (this->next != NULL)
                this->next = Node::XOR(this->prev, this->next->npx);
        }
        public:
        reverse_list_iterator(Node* c, Node* p, Node* n) {
            this->hd = c; this->prev = p; this->next = n;
        }
    };
    Node* head;
    Node* tail;
    size_t length;

    public:
    typedef list_iterator<T> iterator;
    typedef list_iterator<const T> const_iterator;
    typedef reverse_list_iterator<T> reverse_iterator;
    typedef reverse_list_iterator<const T> const_reverse_iterator;

    XORList() {
        head = NULL;
        tail = NULL;
        length = 0;
    }
    ~XORList() { clear(); }

    void push_back(T val) {
        Node* newNode = new Node(val);
        
        if (head == NULL) {
            head = newNode;
            tail = newNode;
        }
        else {
            newNode->npx = Node::XOR(NULL, tail);
            tail->npx = Node::XOR(newNode, tail->npx);
            tail = newNode;
        }
        ++length;
    }
    void push_front(T val) {
        Node* newNode = new Node(val);
        
        if (head == NULL) {
            head = newNode;
            tail = newNode;
        }
        else {
            newNode->npx = Node::XOR(head, NULL);
            head->npx = Node::XOR(head->npx, newNode);
            head = newNode;
        }
        ++length;
    }
    void pop_back() {
        if (length == 0)
            return;
        Node* temp = tail;
        if (length == 1) {
            tail = NULL;
            head = NULL;
        } 
        else {
            Node* prev = tail->npx;
            prev->npx = Node::XOR(prev->npx, tail);
            tail = prev;
        }
        --length;
    }
    void pop_front() {
        if (length == 0)
            return;
        Node* temp = tail;
        if (length == 1) {
            tail = NULL;
            head = NULL;
        } 
        else {
            Node* next = head->npx;
            next->npx = Node::XOR(next->npx, head);
            head = next;
        }
        --length;
    }
    T& back() const {
        if (length == 0)
            throw std::out_of_range("In XORList::range_check. Size is 0");
        return tail->data;
    }
    T& front() const {
        if (length == 0)
            throw std::out_of_range("In XORList::range_check. Size is 0");
        return head->data;
    }
    void clear() {
        Node *next, *prev = NULL;
        while (head != NULL) {
            next = Node::XOR(prev, head->npx);
            prev = head;
            delete head;
            head = next;
        }
        length = 0;
    }
    bool empty() const {return length == 0;}
    
    size_t size() const {return length;}
    
    iterator begin() const {return iterator(head, NULL, head->npx);}
    iterator end() const {return iterator(NULL, tail, NULL);}
    const_iterator cbegin() const {return const_iterator(head, NULL, head->npx);}
    const_iterator cend() const {return const_iterator(NULL, tail, NULL);}
    reverse_iterator rbegin() const {return reverse_iterator(tail, tail->npx, NULL);}
    reverse_iterator rend() const {return reverse_iterator(NULL, NULL, head);}
    const_reverse_iterator crbegin() const {return const_reverse_iterator(tail, tail->npx, NULL);}
    const_reverse_iterator crend() const {return const_reverse_iterator(NULL, NULL, head);}

    friend std::ostream& operator<< (std::ostream& os, const XORList& l) {
        Node *curr = l.head;
        Node *prev = NULL;  
        Node *next;

        os << "[ ";
        while (curr != NULL) {
            os << curr->data << ' ';
            next = Node::XOR(prev, curr->npx);
            prev = curr;
            curr = next;
        }
        os << ']';
        return os;
    }
};

#endif
