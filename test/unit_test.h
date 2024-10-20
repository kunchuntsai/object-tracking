#ifndef UNIT_TEST_H
#define UNIT_TEST_H

#include <cassert>
#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include <chrono>
#include <iomanip>

class UnitTest {
public:
    static void addTest(const std::string& name, std::function<void()> test) {
        tests().push_back({name, test});
    }

    static void runAllTests() {
        int total_tests = tests().size();
        int passed = 0;
        int failed = 0;

        // First, print the start of all tests
        for (int i = 0; i < total_tests; ++i) {
            std::cout << "Start " << (i + 1) << ": " << tests()[i].name << std::endl;
        }

        // Then run the tests
        for (int i = 0; i < total_tests; ++i) {
            const auto& test = tests()[i];
            auto start_time = std::chrono::high_resolution_clock::now();
            
            bool test_passed = true;
            try {
                test.testFunc();
            } catch (const std::exception& e) {
                test_passed = false;
            }

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

            std::cout << (i + 1) << "/" << total_tests << " Test #" << (i + 1) << ": " << test.name 
                      << " ." << std::string(30 - test.name.length(), '.') << "   ";
            
            if (test_passed) {
                std::cout << "Passed";
                passed++;
            } else {
                std::cout << "Failed";
                failed++;
            }

            std::cout << std::fixed << std::setprecision(2) << "    " << (duration.count() / 1000.0) << " sec" << std::endl;
        }

        std::cout << "\nTest results: " << passed << " passed, " << failed << " failed" << std::endl;
    }

private:
    struct TestCase {
        std::string name;
        std::function<void()> testFunc;
    };

    static std::vector<TestCase>& tests() {
        static std::vector<TestCase> testCases;
        return testCases;
    }
};

// The rest of the file remains unchanged
#define TEST(name) \
    void name(); \
    struct name##_registrar { \
        name##_registrar() { UnitTest::addTest(#name, name); } \
    } name##_reg; \
    void name()

#define ASSERT_EQUAL(expected, actual) \
    if ((expected) != (actual)) { \
        throw std::runtime_error(std::string("Assertion failed: ") + #expected + " != " + #actual); \
    }

#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        throw std::runtime_error(std::string("Assertion failed: ") + #condition + " is not true"); \
    }

#define ASSERT_FALSE(condition) \
    if (condition) { \
        throw std::runtime_error(std::string("Assertion failed: ") + #condition + " is not false"); \
    }

#define ASSERT_THROWS(expression, exceptionType) \
    try { \
        expression; \
        throw std::runtime_error(std::string("Expected exception ") + #exceptionType + " was not thrown"); \
    } catch (const exceptionType&) { \
    } catch (...) { \
        throw std::runtime_error(std::string("Unexpected exception caught, expected ") + #exceptionType); \
    }

#endif // UNIT_TEST_H