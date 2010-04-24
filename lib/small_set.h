#ifndef SMALL_SET_H_
#define SMALL_SET_H_

#include "conditional_assert.h"
#include "random.h"

template <typename T, unsigned MAXSIZE>
class SmallSet;

template <typename T, unsigned MAXSIZE>
class SmallSetIterator {
    friend class SmallSet<T, MAXSIZE>;
public:
    bool IsEnd() {
        return position >= set.elements_number;
    }
    SmallSetIterator<T, MAXSIZE>& operator++() {
        ++position;
        return *this;
    }
    SmallSetIterator<T, MAXSIZE> operator++(int) {
        SmallSetIterator<T, MAXSIZE> new_iterator(*this);
        ++position;
        return new_iterator;
    }
    T operator*() {
        ASSERT(!IsEnd());
        return set[position];
    }

private:
    SmallSetIterator(SmallSet<T, MAXSIZE>& set_to_iterate)
        : position(0), set(set_to_iterate) { }
    unsigned short position;
    SmallSet<T, MAXSIZE>& set;
};

template <typename T, unsigned MAXSIZE = 3>
class SmallSet {
    friend class SmallSetIterator<T, MAXSIZE>;
public:
    SmallSet() : elements_number(0) { }
    bool Contains(const T& el) const {
        for (unsigned i = 0; i < elements_number; ++i) {
            if (elements[i] == el)
                return true;
        }
        return false;
    }
    void Insert(const T& el) {
        if (Contains(el))
            return;
        InsertNew(el);
    }
    void InsertNew(const T& el) {
        ASSERT(!Contains(el));
        ASSERT(elements_number < MAXSIZE);
        elements[elements_number++] = el;
    }
    void Remove(const T& el) {
        for (unsigned i = 0; i < elements_number; ++i) {
            if (elements[i] == el) {
                if(i+1<elements_number)
                    elements[i] = elements[--elements_number];
                else
                    --elements_number;
                return;
            }
        }
    }
    void Clear() {
        elements_number = 0;
    }
    bool Empty() const{
        return elements_number == 0;
    }
    T RandomElem() const{
        ASSERT(!Empty());
        int rand  = Rand::next_rand(elements_number);
        return elements[rand];
    }
    SmallSetIterator<T, MAXSIZE> GetIterator() {
        return SmallSetIterator<T, MAXSIZE>(*this);
    }
    unsigned Size() const {
        return elements_number;
    }

    T operator[](int i){
        return elements[i];
    }


private:
    unsigned short elements_number;
    T elements[MAXSIZE];
};

#endif /* SMALL_SET_H_ */
