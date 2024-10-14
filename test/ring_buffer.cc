#include "ring_buffer.h"
#include "unit_test.h"
#include <iostream>
#include <stdexcept>

TEST(RingBufferBasicPushPop) {
    RingBuffer rb(3);
    rb.push(1.0f);
    rb.push(2.0f);
    rb.push(3.0f);
    ASSERT_EQUAL(1.0f, rb.pop());
    ASSERT_EQUAL(2.0f, rb.pop());
    ASSERT_EQUAL(3.0f, rb.pop());
}

TEST(RingBufferOverflow) {
    RingBuffer rb(2);
    rb.push(1.0f);
    rb.push(2.0f);
    rb.push(3.0f); // This should overwrite 1.0f
    ASSERT_EQUAL(2.0f, rb.pop());
    ASSERT_EQUAL(3.0f, rb.pop());
}

TEST(RingBufferEmptyFull) {
    RingBuffer rb(2);
    ASSERT_TRUE(rb.isEmpty());
    ASSERT_FALSE(rb.isFull());
    rb.push(1.0f);
    ASSERT_FALSE(rb.isEmpty());
    ASSERT_FALSE(rb.isFull());
    rb.push(2.0f);
    ASSERT_FALSE(rb.isEmpty());
    ASSERT_TRUE(rb.isFull());
    rb.pop();
    ASSERT_FALSE(rb.isEmpty());
    ASSERT_FALSE(rb.isFull());
}

TEST(RingBufferEmptyPopException) {
    RingBuffer rb(1);
    ASSERT_THROWS(rb.pop(), std::runtime_error);
}

TEST(RingBufferCircularBehavior) {
    RingBuffer rb(3);
    rb.push(1.0f);
    rb.push(2.0f);
    rb.push(3.0f);
    rb.pop();
    rb.push(4.0f);
    ASSERT_EQUAL(2.0f, rb.pop());
    ASSERT_EQUAL(3.0f, rb.pop());
    ASSERT_EQUAL(4.0f, rb.pop());
}

void runRingBufferTests() {
    UnitTest::runAllTests();
}