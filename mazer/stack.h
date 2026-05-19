#pragma once

template <typename T>
class Stack
{
private:
    int _size;
    T *_data;
    int _last_index;

public:

    Stack(int size)
    {
        _size = size;
        _data = new T[size];
        _last_index = 0;
    }

    ~Stack()
    {
        delete [] _data;
    }

    void clear()
    {
        _last_index = 0;
    }

    int numItems()
    {
        return _last_index;
    }

    void push(T value)
    {
        if (_last_index >= _size)
        {
            // Stack is full; this is a programming error.
            return;
        }
        _data[_last_index] = value;
        _last_index++;
    }

    T pop()
    {
        if (_last_index <= 0)
        {
            // Stack is empty; this is a programming error.
            return T{};
        }
        _last_index--;
        return _data[_last_index];
    }
};