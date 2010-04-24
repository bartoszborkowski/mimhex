#ifndef AUTO_POINTER_H
#define AUTO_POINTER_H

template <typename T>
class AutoPointer {
public:
    AutoPointer() : _ptr(NULL) { }
    AutoPointer(T* ptr) : _ptr(ptr) { }
    ~AutoPointer() { delete _ptr; }

    AutoPointer& operator=(T* ptr) {
        if (ptr == _ptr)
            return *this;
        delete _ptr;
        _ptr = ptr;
        return *this;
    }
    T& operator*() const { return *_ptr; }
    T* operator->() const { return _ptr; }
    bool operator==(T* ptr) const { return ptr == _ptr; }
    bool operator!=(T* ptr) const { return ptr != _ptr; }

    T* GetPointer() const { return _ptr; }
    T* Detach() {
        T* ptr = _ptr;
        _ptr = NULL;
        return ptr;
    }
private:
    T* _ptr;
};

template <typename T>
class AutoTable {
public:
    AutoTable() : _ptr(NULL) { }
    AutoTable(T* ptr) : _ptr(ptr) { }
    ~AutoTable() { delete[] _ptr; }

    AutoTable& operator=(T* ptr) {
        if (ptr == _ptr)
            return *this;
        delete[] _ptr;
        _ptr = ptr;
        return *this;
    }
    T& operator*() const { return *_ptr; }
    T* operator->() const { return _ptr; }
    T& operator[](int idx) const { return _ptr[idx]; }
    bool operator==(const T* ptr) const { return ptr == _ptr; }
    bool operator!=(const T* ptr) const { return ptr != _ptr; }

    T* GetPointer() const { return _ptr; }
    T* Detach() {
        T* ptr = _ptr;
        _ptr = NULL;
        return ptr;
    }
private:
    T* _ptr;
};

#endif
