#ifndef GAPBUFFER_H
#define GAPBUFFER_H
#include <iostream> // cout for debug
#include <algorithm> // std::copy, std::move
#include <sstream> // stringstreams
#include <memory> // for unique_ptr

const int kDefaultSize = 10;

class GapBuffer {
public:
    using value_type = char;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = char*;
    using const_pointer = const char*;

    // Part 1
    GapBuffer();
    GapBuffer(size_type count, const value_type& val = value_type());
    ~GapBuffer();
    void insert_at_cursor(const_reference element);
    void delete_at_cursor();
    reference get_at_cursor();
    reference at(size_type pos);

    // Part 2 - you need to declare these yourself
    const_reference at(size_type pos) const;
    const_reference get_at_cursor() const;
    size_type size() const;
    size_type cursor_index() const;
    bool empty() const;

    // Part 3 - you need to declare these yourself
    // member operators
    reference operator[](size_type pos);
    const_reference operator[](size_type pos) const;

    /* Implemented for you */
    void move_cursor(int num);
    void reserve(size_type new_size);
    void debug() const;

private:
    pointer _elems; // uses array_index
    size_type _buffer_size; // uses array_index
    size_type _cursor_index; // uses array_index
    size_type _gap_size;
    size_type to_array_index(size_type logic_index) const;
    void move_to_left_of_buffer(size_type num);
};

GapBuffer::GapBuffer() :
    _elems(nullptr),
    _buffer_size(0),
    _cursor_index(0),
    _gap_size(0) {}

GapBuffer::GapBuffer(size_type count, const value_type& val) : GapBuffer() {
    reserve(count * 2);
    for (size_type i = 0; i < count; ++i) {
        insert_at_cursor(val);
    }
}

void GapBuffer::insert_at_cursor(const_reference element) {
    if (_gap_size == 0) {
        reserve(_buffer_size == 0 ? 1 : _buffer_size * 2);
    }
    _elems[_cursor_index++] = std::move(element);
    --_gap_size;
}

void GapBuffer::delete_at_cursor() {
    if (_cursor_index == 0) {
        return;
    }
    ++_gap_size;
    --_cursor_index;
}

typename GapBuffer::reference GapBuffer::get_at_cursor() {
    return at(_cursor_index - 1);
}

typename GapBuffer::const_reference GapBuffer::get_at_cursor() const {
    return const_cast<GapBuffer*>(this)->get_at_cursor();
}

typename GapBuffer::reference GapBuffer::at(size_type pos) {
    pos = to_array_index(pos);
    if (pos >= _buffer_size) {
        throw std::out_of_range("GapBuffer::at: index out of bounds");
    }
    return _elems[pos];
}

typename GapBuffer::const_reference GapBuffer::at(GapBuffer::size_type pos) const {
    return const_cast<GapBuffer*>(this)->at(pos);
}

typename GapBuffer::size_type GapBuffer::size() const {
    return _buffer_size - _gap_size; // only here so it compiles
}

GapBuffer::size_type GapBuffer::cursor_index() const {
    return _cursor_index;
}

bool GapBuffer::empty() const {
    return _buffer_size == _gap_size;
}

GapBuffer::reference GapBuffer::operator[](GapBuffer::size_type pos) {
    return at(pos);
}

GapBuffer::const_reference GapBuffer::operator[](GapBuffer::size_type pos) const {
    return at(pos);
}

// Implement Part 3 functions here
// Members and non-member non-friends.

std::ostream& operator<<(std::ostream& os, const GapBuffer& buf) {
    os << "{";
    GapBuffer::size_type buf_size = buf.size();
    GapBuffer::size_type cursor_index = buf.cursor_index();
    for (GapBuffer::size_type i = 0; i < buf_size; i++) {
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

bool operator==(const GapBuffer& left, const GapBuffer& right) {
    GapBuffer::size_type leftSize = left.size();
    GapBuffer::size_type rightSize = right.size();
    if (leftSize != rightSize) {
        return false;
    }

    for (GapBuffer::size_type i = 0; i < leftSize; ++i) {
        if (left.at(i) != right.at(i)) {
            return false;
        }
    }

    return true;
}

bool operator!=(const GapBuffer& left, const GapBuffer& right) {
    return !(left == right);
}

bool operator<(const GapBuffer& left, const GapBuffer& right) {
    GapBuffer::size_type leftSize = left.size();
    GapBuffer::size_type rightSize = right.size();
    GapBuffer::size_type minSize = std::min(leftSize, rightSize);

    for (GapBuffer::size_type i = 0; i < minSize; ++i) {
        GapBuffer::const_reference l = left.at(i);
        GapBuffer::const_reference r = right.at(i);
        if (l != r) {
            return l < r;
        }
    }

    return leftSize < rightSize;
}

bool operator>(const GapBuffer& left, const GapBuffer& right) {
    return left.size() != right.size() && !(left < right);
}

bool operator<=(const GapBuffer& left, const GapBuffer& right) {
    return !(left > right);
}

bool operator>=(const GapBuffer& left, const GapBuffer& right) {
    return !(left < right);
}





/* We've implemented these for you, do not edit! */
void GapBuffer::move_cursor(int delta) {
    int new_index = _cursor_index + delta;
    if (new_index < 0 || new_index > static_cast<int>(_buffer_size)) {
        throw std::out_of_range("move_cursor: delta moves cursor out of bounds");
    }
    if (delta > 0 && _cursor_index == size()) {
        throw std::out_of_range("GapBuffer::move_cursor index out of bounds");
    }
    if (delta > 0) {
        pointer begin_move = _elems + _cursor_index + _gap_size;
        pointer end_move = begin_move + delta;
        pointer destination = _elems + _cursor_index;
        std::move(begin_move, end_move, destination);
    } else {
        pointer end_move = _elems + _cursor_index;
        pointer begin_move = end_move + delta;
        pointer destination = _elems + _cursor_index + _gap_size + delta;
        std::move(begin_move, end_move, destination);
    }
    _cursor_index += delta;
}

void GapBuffer::reserve(size_type new_size) {
    size_type oldSize = size();
    if (_buffer_size >= new_size) return;
    pointer new_elems = new char[new_size];
    if (_elems != nullptr) {
        //   0 1 2 3 4 5 6 7 8 9
        // [ a|* * * * * * * * b ]
        // [ a|* * b ]
        std::move(_elems, // 0
                  _elems + _cursor_index, // 1
                  new_elems); // new 0

        std::move(_elems+ _cursor_index, // 1
                  _elems + _buffer_size, // 4
                  new_elems + _cursor_index + new_size - _buffer_size); // new 7

        delete[] _elems;
    }
    _buffer_size = new_size;
    _elems = std::move(new_elems);
    _gap_size = new_size - oldSize;
}

void GapBuffer::debug() const {
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

typename GapBuffer::size_type GapBuffer::to_array_index(size_type logic_index) const {
    if (logic_index >= size()) {
        throw std::out_of_range("GapBuffer::to_array_index: index out of bounds");
    }
    if (logic_index < _cursor_index) {
        return logic_index;
    } else {
        return logic_index + _gap_size;
    }
}

GapBuffer::~GapBuffer() {
    if (_elems != nullptr) {
        delete[] _elems;
    }
}

#endif // GAPBUFFER_H
