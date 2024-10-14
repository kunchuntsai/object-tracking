#ifndef RINGBUFFER_HPP
#define RINGBUFFER_HPP

#include <vector>
#include <stdexcept>
#include <iostream>

class RingBuffer {
private:
    /* buffer size */
    size_t Size;
    /* index for output data */
    size_t head;
    /* index for input data */
    size_t tail;
    /* flag to indicate if buffer is full */
    bool full;
    /* vector to hold the data */
    std::vector<float> buffer;

public:
    RingBuffer(size_t size) : 
        Size(size), 
        head(0), 
        tail(0), 
        full(false),
        buffer(size, 0.0f)  // Initialize vector with size elements, all set to 0.0f
    {
        if (Size == 0) {
            throw std::runtime_error("Size is invalid");
        }
    }

    // No need for a destructor, vector manages its own memory

    void push(float value) {
        buffer[tail] = value;
        tail = (tail + 1) % Size;
        full = (head == tail);

        print();
    }

    float pop() {
        if (isEmpty()) {
            throw std::runtime_error("Buffer is empty");
        }

        float value = buffer[head];
        head = (head + 1) % Size;
        full = false;

        print();
        return value;
    }

    bool isEmpty() const {
        return !full && (head == tail);
    }

    bool isFull() const {
        return full;
    }

    void print() const {
        std::cout << "Buffer: ";
        for (const auto& value : buffer) {
            std::cout << value << ", ";
        }
        std::cout << "(h: " << head << ", t: " << tail << ", f: " << full << ")" << std::endl;
    }
};
#endif // RINGBUFFER_HPP