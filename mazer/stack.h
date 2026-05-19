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
        _data[_last_index] = value;
        _last_index++;
        if (_last_index >= _size)
        {
            _last_index = 0;
        }
    }

    T pop()
    {
        T value = _data[_last_index - 1];
        _last_index--;
        if (_last_index < 0)
        {
            _last_index = _size;
        }
        return value;
    }
};