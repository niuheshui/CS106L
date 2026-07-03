/*
 * Assignment 3 Final Starter Code
 * TODO: Fill in this comment!
 */

#ifndef GAPBUFFER_H
#define GAPBUFFER_H
#include <vector>
#include <iostream> // for cout in debug
#include <algorithm>
#include <sstream> // for stringstreams
#include <memory> // for unique_ptr

const int kDefaultSize = 10;

// forward declaration for the GapBufferIterator class
template <typename T>
class GapBufferIterator;

// declaration for the GapBuffer class
template <typename T>
class GapBuffer {
public:
    friend class GapBufferIterator<T>;

    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using iterator = GapBufferIterator<T>;

    explicit GapBuffer();
    explicit GapBuffer(size_type count, const value_type& val = value_type());
    ~GapBuffer();
    GapBuffer(std::initializer_list<T> init);
    GapBuffer(const GapBuffer& other);
    GapBuffer(GapBuffer&& other);
    GapBuffer& operator=(const GapBuffer& rhs);
    GapBuffer& operator=(GapBuffer&& rhs);

    void insert_at_cursor(const_reference element);
    void insert_at_cursor(value_type&& element);
    template <typename... Args>
    void emplace_at_cursor(Args&&... args); // optional
    void delete_at_cursor();
    reference get_at_cursor();
    const_reference get_at_cursor() const;
    reference operator[](size_type pos);
    const_reference operator[](size_type pos) const;
    reference at(size_type pos);
    const_reference at(size_type pos) const;
    void move_cursor(int num);
    void reserve(size_type new_size);
    size_type size() const;
    size_type cursor_index() const;
    bool empty() const;
    void debug() const;

    iterator begin();
    iterator end();
    iterator cursor();

private:
    size_type _buffer_size;  // uses array_index
    size_type _cursor_index; // uses array_index
    size_type _gap_size;
    pointer _elems;          // uses array_index

    size_type to_external_index(size_type array_index) const;
    size_type to_array_index(size_type external_index) const;
    void move_to_left_of_buffer(size_type num);
};

// Class declaration of the GapBufferIterator class
template <typename T>
class GapBufferIterator : public std::iterator<std::random_access_iterator_tag, T> {
public:
    friend class GapBuffer<T>;
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = GapBufferIterator<T>;

    reference operator*();
    reference operator*() const;
    iterator& operator++();
    iterator operator++(int);
    iterator& operator--();
    iterator operator--(int);

    // the operators below are implemented as non-friend non-members
    // iterator operator+(const iterator& lhs, size_type diff);
    // iterator operator-(const iterator& lhs, size_type diff);
    // iterator operator+(size_type diff, const iterator& rhs);

    /* we implemented these for you. don't change these */
    friend bool operator==(const iterator& lhs, const iterator& rhs) { return lhs._index == rhs._index; }
    friend bool operator!=(const iterator& lhs, const iterator& rhs) { return !(lhs == rhs); }
    friend difference_type operator-(const iterator& lhs, const iterator& rhs) { return lhs._index - rhs._index; }
    iterator& operator+=(size_type diff) { _index += diff; return *this; }
    iterator& operator-=(size_type diff) { _index -= diff; return *this; }
    friend bool operator>(const iterator& lhs, const iterator& rhs)  { return lhs._index < rhs._index; }
    friend bool operator<(const iterator& lhs, const iterator& rhs)  { return lhs._index > rhs._index; }
    friend bool operator<=(const iterator& lhs, const iterator& rhs)  { return !(lhs > rhs); }
    friend bool operator>=(const iterator& lhs, const iterator& rhs)  { return !(lhs < rhs); }
    reference operator[](size_t index) { return *(*this + index); }

private:
    GapBuffer<T>* _pointee;
    size_t _index;
    GapBufferIterator(GapBuffer<T>* pointee, size_t index) : _pointee(pointee), _index(index) {}
};

// Part 1: basic functions
template <typename T>
GapBuffer<T>::GapBuffer() :
    _elems(nullptr),
    _buffer_size(0),
    _cursor_index(0),
    _gap_size(0) {}

template <typename T>
GapBuffer<T>::GapBuffer(size_type count, const value_type& val) : GapBuffer<T>() {
    reserve(count * 2);
    for (size_type i = 0; i < count; ++i) {
        insert_at_cursor(val);
    }
}

template <typename T>
void GapBuffer<T>::insert_at_cursor(const_reference element) {
    insert_at_cursor(T{element});
}

template <typename T>
void GapBuffer<T>::delete_at_cursor() {
    if (_cursor_index == 0) {
        return;
    }
    ++_gap_size;
    --_cursor_index;
}

template <typename T>
typename GapBuffer<T>::reference GapBuffer<T>::get_at_cursor() {
    return const_cast<GapBuffer<T>::reference>(static_cast<const GapBuffer<T>*>(this)->get_at_cursor());
}

template <typename T>
typename GapBuffer<T>::reference GapBuffer<T>::at(size_type pos) {
    pos = to_array_index(pos);
    if (pos >= _buffer_size) {
        throw std::out_of_range("GapBuffer::at: index out of bounds");
    }
    return _elems[pos];
}

template <typename T>
typename GapBuffer<T>::size_type GapBuffer<T>::size() const {
    return _buffer_size - _gap_size;
}

template <typename T>
typename GapBuffer<T>::size_type GapBuffer<T>::cursor_index() const {
    return _cursor_index;
}

template <typename T>
bool GapBuffer<T>::empty() const {
    return size() == 0;
}

// Part 2: const-correctness

template <typename T>
typename GapBuffer<T>::const_reference GapBuffer<T>::get_at_cursor() const {
    return at(_cursor_index);
}

template <typename T>
typename GapBuffer<T> ::const_reference GapBuffer<T>::at(size_type pos) const {
    return const_cast<GapBuffer<T>*>(this)->at(pos);
}

// Part 3: operator overloading
template <typename T>
typename GapBuffer<T>::reference GapBuffer<T>::operator[](size_type pos) {
    return at(pos);
}

template <typename T>
typename GapBuffer<T>::const_reference GapBuffer<T>::operator[](size_type pos) const {
    return at(pos);
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const GapBuffer<T>& buf) {
    os << "{";
    typename GapBuffer<T>::size_type buf_size = buf.size();
    typename GapBuffer<T>::size_type cursor_index = buf.cursor_index();
    for (typename GapBuffer<T>::size_type i = 0; i < buf_size; i++) {
        if (i != 0) {
            os << ", ";
        }
        if (i == cursor_index) {
            os << "^";
        }
        os << buf[i];
    }
    if (cursor_index == buf_size) {
        os << "^";
    }
    os << "}";
    return os;
}

template <typename T>
bool operator==(const GapBuffer<T>& lhs, const GapBuffer<T>& rhs) {
    return lhs.size() == rhs.size() && std::equal(const_cast<GapBuffer<T>&>(lhs).begin(),
                                                  const_cast<GapBuffer<T>&>(lhs).end(),
                                                  const_cast<GapBuffer<T>&>(rhs).begin());
}

template <typename T>
bool operator!=(const GapBuffer<T>& lhs, const GapBuffer<T>& rhs) {
    return !(lhs == rhs);
}

template <typename T>
bool operator<(const GapBuffer<T>& lhs, const GapBuffer<T>& rhs) {
    return std::lexicographical_compare(const_cast<GapBuffer<T>&>(lhs).begin(),
                                        const_cast<GapBuffer<T>&>(lhs).end(),
                                        const_cast<GapBuffer<T>&>(rhs).begin(),
                                        const_cast<GapBuffer<T>&>(rhs).end());
}

template <typename T>
bool operator>(const GapBuffer<T>& lhs, const GapBuffer<T>& rhs) {
    return lhs.size() != rhs.size() && !(lhs < rhs);
}

template <typename T>
bool operator<=(const GapBuffer<T>& lhs, const GapBuffer<T>& rhs) {
    return !(lhs > rhs);
}

template <typename T>
bool operator>=(const GapBuffer<T>& lhs, const GapBuffer<T>& rhs) {
    return !(lhs < rhs);
}

// Part 4: turn everything into a template!


// Part 5: Implement iterators
template <typename T>
typename GapBufferIterator<T>::reference GapBufferIterator<T>::operator*() {
    return _pointee->at(_index);
}

template <typename T>
typename GapBufferIterator<T>::reference GapBufferIterator<T>::operator*() const {
    return _pointee->at(_index);
}

template <typename T>
GapBufferIterator<T>& GapBufferIterator<T>::operator++() {
    _index++;
    return *this;
}

template <typename T>
GapBufferIterator<T> GapBufferIterator<T>::operator++(int) {
    _index + 1;
    return GapBufferIterator<T>(_pointee, _index - 1);
}

template <typename T>
GapBufferIterator<T>& GapBufferIterator<T>::operator--() {
    _index--;
    return *this;
}

template <typename T>
GapBufferIterator<T> GapBufferIterator<T>::operator--(int) {
    _index--;
    return GapBufferIterator<T>(_pointee, _index + 1);
}

template <typename T>
GapBufferIterator<T> operator+(const GapBufferIterator<T>& lhs,
                               typename GapBufferIterator<T>::size_type diff) {
    GapBufferIterator<T> it = lhs;
    return it += diff;
}

template <typename T>
GapBufferIterator<T> operator+(typename GapBufferIterator<T>::size_type diff,
                               const GapBufferIterator<T>& rhs) {
    GapBufferIterator<T> it = rhs;
    return it += diff;
}

template <typename T>
GapBufferIterator<T> operator-(const GapBufferIterator<T>& lhs,
                               typename GapBufferIterator<T>::size_type diff) {
    GapBufferIterator<T> it = lhs;
    return it -= diff;
}

// The functions that are part of the GapBuffer class is provided for you!
template <typename T>
typename GapBuffer<T>::iterator GapBuffer<T>::begin() {
    return iterator(this, 0);
}

template <typename T>
typename GapBuffer<T>::iterator GapBuffer<T>::end() {
    return iterator(this, size());
}

template <typename T>
typename GapBuffer<T>::iterator GapBuffer<T>::cursor() {
    return iterator(this, _cursor_index);
}

// Part 6: Constructors and assignment

template <typename T>
GapBuffer<T>::~GapBuffer() {
    if (_elems != nullptr) {
        delete[] _elems;
    }
}

template <typename T>
GapBuffer<T>::GapBuffer(std::initializer_list<T> init) : GapBuffer<T>() {
    for (const T& i : init) {
        insert_at_cursor(i);
    }
}

template <typename T>
GapBuffer<T>::GapBuffer(const GapBuffer& other)
    : _elems(new T[other._buffer_size])
    , _buffer_size(other._buffer_size)
    , _cursor_index(other._cursor_index)
    , _gap_size(other._gap_size)
{
    std::copy(other._elems, other._elems + other._buffer_size, _elems);
}

template <typename T>
GapBuffer<T>& GapBuffer<T>::operator=(const GapBuffer<T>& rhs) {
    // 1. 处理自赋值
    if (this == &rhs) {
        return *this;
    }

    // 2. 分配新内存
    pointer new_elems = nullptr;
    try {
        new_elems = new T[rhs._buffer_size];           // 可能抛异常
        std::copy(rhs._elems, rhs._elems + rhs._buffer_size, new_elems);

        // 3. 释放旧内存
        delete[] _elems;

        // 4. 接管新资源
        _elems = new_elems;
        _buffer_size = rhs._buffer_size;
        _cursor_index = rhs._cursor_index;
        _gap_size = rhs._gap_size;

        new_elems = nullptr;  // 防止下面 catch 中重复释放

    } catch (...) {
        delete[] new_elems;   // 发生异常时清理新分配的内存
        throw;
    }

    return *this;
}

// Part 7: Move semantics
template <typename T>
GapBuffer<T>::GapBuffer(GapBuffer&& other)
    :  _elems(std::move(other._elems))
    , _buffer_size(other._buffer_size)
    , _cursor_index(other._cursor_index)
    , _gap_size(other._gap_size)
{
    other._elems = nullptr;
    other._buffer_size = 0;
    other._cursor_index = 0;
    other._gap_size = 0;
}

template <typename T>
GapBuffer<T>& GapBuffer<T>::operator=(GapBuffer<T>&& rhs) {
    if (this == &rhs) {
        return *this;
    }

    delete[] _elems;

    // move resource
    _elems = std::move(rhs._elems);
    _buffer_size = rhs._buffer_size;
    _cursor_index = rhs._cursor_index;
    _gap_size = rhs._gap_size;

    rhs._elems = nullptr;
    rhs._buffer_size = 0;
    rhs._cursor_index = 0;
    rhs._gap_size = 0;

    return *this;
}

template <typename T>
void GapBuffer<T>::insert_at_cursor(value_type&& element) {
    if (_gap_size == 0) {
        reserve(_buffer_size == 0 ? 1 : _buffer_size * 2);
    }
    _elems[_cursor_index++] = std::move(element);
    --_gap_size;
}

// Part 8: Make your code RAII-compliant - change the code throughout

// optional:
template <typename T>
template <typename... Args>
void GapBuffer<T>::emplace_at_cursor(Args&&... args) {
    insert_at_cursor(T(std::forward<Args>(args)...));
}


// We've implemented the following functions for you.
// However...they do use raw pointers, so you might want to turn them into smart pointers!
template <typename T>
void GapBuffer<T>::move_cursor(int delta) {
    int new_index = _cursor_index + delta;
    if (new_index < 0 || new_index > static_cast<int>(size())) {
        throw std::string("move_cursor: delta moves cursor out of bounds");
    }
    if (delta > 0) {
        auto begin_move = _elems + _cursor_index + _gap_size;
        auto end_move = begin_move + delta;
        auto destination = _elems + _cursor_index;
        std::move(begin_move, end_move, destination);
    } else {
        auto end_move = _elems + _cursor_index;
        auto begin_move = end_move + delta;
        auto* destination = _elems + _cursor_index + _gap_size + delta;
        std::move(begin_move, end_move, destination);
    }
    _cursor_index += delta;
}

template <typename T>
void GapBuffer<T>::reserve(size_type new_size) {
    size_type old_size = size();
    if (old_size > new_size) return;
    auto new_elems = new T[new_size];
    size_t new_gap_size = new_size - old_size;
    if (_elems != nullptr) {
        std::move(_elems, _elems + _cursor_index, new_elems);
        std::move(_elems + _cursor_index + _gap_size,
                  _elems + _buffer_size,
                  new_elems + _cursor_index + new_gap_size);
        delete [] _elems;
    }
    _buffer_size = new_size;
    _elems = std::move(new_elems);
    _gap_size = new_gap_size;
}

template <typename T>
void GapBuffer<T>::debug() const {
    std::cout << "[";
    for (size_t i = 0; i < _buffer_size; ++i) {
        if (i == _cursor_index) {
            std::cout << "|";
        } else {
            std::cout << " ";
        }
        if (i >= _cursor_index && i < _cursor_index + _gap_size) {
            std::cout << "*";
        } else {
            std::cout << _elems[i];
        }
    }
    std::cout << (_cursor_index == _buffer_size ? "|" : " ");
    std::cout << "]" << std::endl;
}

template <typename T>
typename GapBuffer<T>::size_type GapBuffer<T>::to_external_index(size_type array_index) const {
    if (array_index < _cursor_index) {
        return array_index;
    } else if (array_index >= _cursor_index + _gap_size){
        return array_index - _cursor_index;
    } else {
        throw ("to_external_index: array_index is out of bounds!");
    }
}

template <typename T>
typename GapBuffer<T>::size_type GapBuffer<T>::to_array_index(size_type external_index) const {
    if (external_index < _cursor_index) {
        return external_index;
    } else {
        return external_index + _gap_size;
    }
}


#endif // GAPBUFFER_H
