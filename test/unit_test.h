#ifndef UNIT_TEST_H
#define UNIT_TEST_H

#include <cassert>
#include <iostream>
#include <vector>
#include <functional>
#include <string>

class UnitTest {
public:
    static void addTest(const std::string& name, std::function<void()> test) {
        tests().push_back({name, test});
    }

    static void runAllTests() {
        int passed = 0;
        int failed = 0;

        for (const auto& test : tests()) {
            std::cout << "Running test: " << test.name << std::endl;
            try {
                test.testFunc();
                std::cout << "PASSED" << std::endl;
                passed++;
            } catch (const std::exception& e) {
                std::cout << "FAILED: " << e.what() << std::endl;
                failed++;
            }
            std::cout << std::endl;
        }

        std::cout << "Test results: " << passed << " passed, " << failed << " failed" << std::endl;
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