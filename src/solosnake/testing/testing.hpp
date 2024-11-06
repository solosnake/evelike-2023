#ifndef SOLOSNAKE_TESTING_HPP
#define SOLOSNAKE_TESTING_HPP

//
// The macros in this file are intended to permit some code written to use the
// Google Test framework to compile and run Google Test framework to compile and
// run as previously.
//
// Options:
// --------
// Define TESTING_NO_TESTING to stop tests being run.
// Define TESTING_NO_AUTOMATIC_MAIN to stop this file from automatically adding
// an 'int main()' when included.

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>

// Define this to have tests throw at first failure (fail fast for debugging).
//#define TESTING_THROW_ON_FAILURE    1

namespace solosnake::testing
{
    namespace
    {
        unsigned long FAILED_TESTS_COUNT = 0;
        unsigned long PASSED_TESTS_COUNT = 0;
        unsigned long TOTAL_TESTS_COUNT  = 0;

        bool almost_equal(float x, float y, double error_factor)
        {
            if( (x == y)
                || (std::fabs(x-y) < std::numeric_limits<float>::epsilon())
                || (std::fabs(x-y) < std::abs(std::min(x,y))*std::numeric_limits<float>::epsilon()* error_factor)
                || (std::fabs(x-y) < std::numeric_limits<float>::min())
              )
                {
                    // Close enough.
                    return true;
                }
            else
            {
                std::cerr << x << " and " << y << " are dissimilar."
                          << std::endl;
                if( x != y )
                {
                    std::cerr << x << " != " << y << std::endl;
                }

                return false;
            }
        }

        bool almost_equal(double x, double y, double error_factor)
        {
            if( (x == y)
                || (std::abs(x-y) < std::abs(std::min(x,y))*std::numeric_limits<double>::epsilon()* error_factor)
                || (std::abs(x-y) < std::numeric_limits<double>::min())
              )
                {
                    // Close enough.
                    return true;
                }
            else
            {
                std::cerr << x << " and " << y << " are dissimilar."
                          << std::endl;
                if( x != y )
                {
                    std::cerr << x << " != " << y << std::endl;
                }

                return false;
            }
        }
    }// namespace

    inline void failed(char const * const what,
                       const unsigned int line,
                       char const * const group_name,
                       char const * const test_name)
    {
        std::cerr << "FAIL: Test '" << test_name << "' in '" << group_name << "' group failed. (Line "
                  << line << ", " << what << " )"
                  << std::endl;
        ++FAILED_TESTS_COUNT;
#ifdef TESTING_THROW_ON_FAILURE
        throw std::logic_error("failed.");
#endif
    }

    inline void expect_failed(char const * const what,
                              const unsigned int line,
                              char const * const group_name,
                              char const * const test_name)
    {
        return failed(what, line, group_name, test_name);
    }

    inline std::ostream & print_txt(char const * const group_name,
                                    char const * const test_name,
                                    char const * const text)
    {
        std::clog << "COMMENT (" << group_name << ", " << test_name << "):\n"
                  << text
                  << std::endl;
        return std::clog;
    }

    inline std::ostream & print_txt(char const * const  group_name,
                                    char const * const  test_name,
                                    const std::string & text)
    {
        return print_txt(group_name, test_name, text.c_str());
    }

    inline std::ostream & report_failure(char const * const group_name,
                                         char const * const test_name,
                                         char const * const comment1 = nullptr,
                                         char const * const comment2 = nullptr) noexcept
    {
        try
        {
            if (comment1 && comment2)
            {
                // Both comments
                std::cerr << "FAIL: Test '" << test_name << "' in '" << group_name << "' group failed: '";
                std::cerr << comment1 << "' '" << comment2 << "'.";
                std::cerr << std::endl;
            }
            else if (comment1 || comment2)
            {
                // Either one or other comment.
                std::cerr << "FAIL: Test '" << test_name << "' in '" << group_name << "' group failed ";
                std::cerr << " '" << (comment1 ? comment1 : comment2) << "'.";
                std::cerr << std::endl;
            }
            else
            {
                // No comments.
                std::cerr << "FAIL: Test '" << test_name << "' in '" << group_name << "' group failed.";
                std::cerr << std::endl;
            }

            ++FAILED_TESTS_COUNT;
        }
        catch (...)
        {
            // NOP
        }

        return std::cerr;
    }

    inline std::ostream & report_failure(char const * const  group_name,
                                         char const * const  test_name,
                                         const std::string & comment1) noexcept
    {
        return report_failure(group_name, test_name, comment1.c_str(), nullptr);
    }

    // Forces an instance of the test to be constructed inside a
    // scope which can catch (and report) exceptions.
    template<typename T>
    class Test_instance
    {
    public:
        Test_instance(const char * group_name, const char * test_name) noexcept
        {
            try
            {
                ++TOTAL_TESTS_COUNT;
                T test_instance(group_name, test_name);
                ++PASSED_TESTS_COUNT;
            }
            catch (const std::logic_error & e)
            {
                report_failure(group_name, test_name, "std::logic_error", e.what());
            }
            catch (const std::runtime_error & e)
            {
                report_failure(group_name, test_name, "std::runtime_error", e.what());
            }
            catch (const std::exception & e)
            {
                report_failure(group_name, test_name, "std::exception", e.what());
            }
            catch (...)
            {
                report_failure(group_name, test_name, "Unknown exception.");
            }
        }
    };

    class BaseTest
    {
    protected:
        explicit BaseTest(char const * const group_name,
                          char const * const test_name) noexcept
        {
            try
            {
                std::clog << "TEST( " << group_name << ", " << test_name << " )";
                std::clog << std::endl;
            }
            catch (...)
            {
                // NOP
            }
        }

        virtual ~BaseTest() noexcept = default;
    };

}// namespace solosnake::testing


#define TESTING_CONCAT(A, B) A##B

#define TESTING_STRINGIFY(A) #A

#define TEST_CLASSNAME(GROUP_NAME, TEST_NAME) TESTING_CONCAT(GROUP_NAME, TEST_NAME)

// Declare a unique class name based on the test and group names. This class is then instanced
// and the user's test body used as its ctor body.
#define TEST_CLASS(GROUP_NAME, TEST_NAME)                                                                \
    class TEST_CLASSNAME(GROUP_NAME, TEST_NAME) final : private solosnake::testing::BaseTest             \
    {                                                                                                    \
    public:                                                                                              \
        explicit TEST_CLASSNAME(GROUP_NAME, TEST_NAME)(char const * group_name, char const * test_name); \
    }

// Use the user's TEST body as the hidden class ctor code.
#define TEST_CLASS_CTOR(GROUP_NAME, TEST_NAME) TEST_CLASSNAME(GROUP_NAME, TEST_NAME)::TEST_CLASSNAME(GROUP_NAME, TEST_NAME)(const char * group_name, const char * test_name) \
    : solosnake::testing::BaseTest(group_name, test_name)

#define TEST_INSTANCE_NAME(X, Y) TESTING_CONCAT(X, Y)

// Define TESTING_NO_TESTING to disable tests being called.
#ifdef TESTING_NO_TESTING
#define TEST(GROUP_NAME, TEST_NAME)    \
    TEST_CLASS(GROUP_NAME, TEST_NAME); \
    TEST_CLASS_CTOR(GROUP_NAME, TEST_NAME)
#else
// This is the user test declaration that gets expanded into:
// 1. A definition of a custom class (TEST_CLASS)
// 2. A declaration of an instance of the custom class
//    which inherits from BaseTest (TEST_CLASSNAME)
// 3. The user's test code is used to form the custom class
//    constructor code (TEST_CLASS_CTOR)
#define TEST(GROUP_NAME, TEST_NAME)                                                                                                                                             \
    TEST_CLASS(GROUP_NAME, TEST_NAME);                                                                                                                                          \
    namespace                                                                                                                                                                   \
    {                                                                                                                                                                           \
        solosnake::testing::Test_instance<TEST_CLASSNAME(GROUP_NAME, TEST_NAME)> TEST_INSTANCE_NAME(g_, __LINE__)(TESTING_STRINGIFY(GROUP_NAME), TESTING_STRINGIFY(TEST_NAME)); \
    }                                                                                                                                                                           \
    TEST_CLASS_CTOR(GROUP_NAME, TEST_NAME)
#endif// TESTING_NO_TESTING


// Note: The 'do..while(false) construct is to permit a terminating ; without a warning.

#define EXPECT_TRUE(B)                                                                                  \
    do                                                                                                  \
    {                                                                                                   \
        try                                                                                             \
        {                                                                                               \
            if (!(B))                                                                                   \
            {                                                                                           \
                solosnake::testing::expect_failed("EXPECT_TRUE", __LINE__, group_name, test_name);      \
            }                                                                                           \
        }                                                                                               \
        catch (...)                                                                                     \
        {                                                                                               \
            solosnake::testing::expect_failed("EXPECT_TRUE", __LINE__, group_name, test_name);          \
        }                                                                                               \
    } while (false)

#define EXPECT_FALSE(B)                                                                                 \
    do                                                                                                  \
    {                                                                                                   \
        try                                                                                             \
        {                                                                                               \
            if ((B))                                                                                    \
            {                                                                                           \
                solosnake::testing::expect_failed("EXPECT_FALSE", __LINE__, group_name, test_name);     \
            }                                                                                           \
        }                                                                                               \
        catch (...)                                                                                     \
        {                                                                                               \
            solosnake::testing::expect_failed("EXPECT_FALSE", __LINE__, group_name, test_name);         \
        }                                                                                               \
    } while (false)

#define EXPECT_EQ(A, B)                                                                                 \
    do                                                                                                  \
    {                                                                                                   \
        try                                                                                             \
        {                                                                                               \
            if (!((A) == (B)))                                                                          \
            {                                                                                           \
                solosnake::testing::expect_failed("EXPECT_EQ", __LINE__, group_name, test_name);        \
            }                                                                                           \
        }                                                                                               \
        catch (...)                                                                                     \
        {                                                                                               \
            solosnake::testing::expect_failed("EXPECT_EQ", __LINE__, group_name, test_name);            \
        }                                                                                               \
    } while (false)

#define EXPECT_NEQ(A, B)                                                                                \
    do                                                                                                  \
    {                                                                                                   \
        try                                                                                             \
        {                                                                                               \
            if ((A) == (B))                                                                             \
            {                                                                                           \
                solosnake::testing::expect_failed("EXPECT_NEQ", __LINE__, group_name, test_name);       \
            }                                                                                           \
        }                                                                                               \
        catch (...)                                                                                     \
        {                                                                                               \
            solosnake::testing::expect_failed("EXPECT_NEQ", __LINE__, group_name, test_name);           \
        }                                                                                               \
    } while (false)

#define EXPECT_FLOAT_EQ(A, B)                                                                           \
    do                                                                                                  \
    {                                                                                                   \
        try                                                                                             \
        {                                                                                               \
            if ( ! solosnake::testing::almost_equal(A,B,4.0) )                                                                  \
            {                                                                                           \
                solosnake::testing::expect_failed("EXPECT_FLOAT_EQ", __LINE__, group_name, test_name);       \
            }                                                                                           \
        }                                                                                               \
        catch (...)                                                                                     \
        {                                                                                               \
            solosnake::testing::expect_failed("EXPECT_FLOAT_EQ", __LINE__, group_name, test_name);           \
        }                                                                                               \
    } while (false)

#define EXPECT_THROW(F)                                                                                 \
    do                                                                                                  \
    {                                                                                                   \
        try                                                                                             \
        {                                                                                               \
            F;                                                                                          \
            solosnake::testing::expect_failed("EXPECT_THROW", __LINE__, group_name, test_name);         \
        }                                                                                               \
        catch (...)                                                                                     \
        {                                                                                               \
        }                                                                                               \
    } while (false)

#define EXPECT_NO_THROW(F)                                                                              \
    do                                                                                                  \
    {                                                                                                   \
        try                                                                                             \
        {                                                                                               \
            F;                                                                                          \
        }                                                                                               \
        catch (...)                                                                                     \
        {                                                                                               \
            solosnake::testing::expect_failed("EXPECT_NO_THROW", __LINE__, group_name, test_name);      \
        }                                                                                               \
    } while (false)

#define ADD_FAILURE(TXT) solosnake::testing::report_failure(group_name, test_name, TXT)

#define TEST_PRINT(TXT)  solosnake::testing::print_txt(group_name, test_name, TXT)

#define ASSERT_TRUE(B)                                                                                  \
    do {                                                                                                \
        if(!(B))                                                                                        \
        {                                                                                               \
            solosnake::testing::expect_failed("ASSERT_TRUE",  __LINE__, "", __FUNCTION__);              \
        }                                                                                               \
    } while(false)

#define ASSERT_FALSE(B)                                                                                 \
    do {                                                                                                \
        if((B))                                                                                         \
        {                                                                                               \
            solosnake::testing::expect_failed("ASSERT_FALSE",  __LINE__, "", __FUNCTION__);             \
        }                                                                                               \
    } while(false)

#define ASSERT_EQ(A,B)                                                                                  \
    do {                                                                                                \
        if((A) == (B))                                                                                  \
        {                                                                                               \
            solosnake::testing::expect_failed("ASSERT_EQ",  __LINE__, "", __FUNCTION__);                \
        }                                                                                               \
    } while(false)

#define TESTING_MAIN_CONTENTS                                                                           \
    if (solosnake::testing::FAILED_TESTS_COUNT > 0u)                                                    \
    {                                                                                                   \
        std::clog << "FAIL: " << solosnake::testing::FAILED_TESTS_COUNT << " of ";                      \
        std::clog << solosnake::testing::TOTAL_TESTS_COUNT << " test(s) failed.";                       \
        std::clog << std::endl;                                                                         \
        return EXIT_FAILURE;                                                                            \
    }                                                                                                   \
    else                                                                                                \
    {                                                                                                   \
        std::clog << "PASS: " << solosnake::testing::PASSED_TESTS_COUNT << " of ";                      \
        std::clog << solosnake::testing::TOTAL_TESTS_COUNT << " test(s) passed.";                       \
        std::clog << std::endl;                                                                         \
        return EXIT_SUCCESS;                                                                            \
    }

// We instance main here. This makes test files self contained executables.
// Define TESTING_NO_AUTOMATIC_MAIN to prevent this.

#ifdef TESTING_NO_AUTOMATIC_MAIN
    // User will define their own `main`.
#else
    // We will insert a `main` for them. This can cause linker problems.
    int main(int, char **)
    {
        if (solosnake::testing::FAILED_TESTS_COUNT > 0u)
        {
            std::clog << "FAIL: " << solosnake::testing::FAILED_TESTS_COUNT << " of ";
            std::clog << solosnake::testing::TOTAL_TESTS_COUNT << " test(s) failed.";
            std::clog << std::endl;
            return EXIT_FAILURE;
        }
        else
        {
            std::clog << "PASS: " << solosnake::testing::PASSED_TESTS_COUNT << " of ";
            std::clog << solosnake::testing::TOTAL_TESTS_COUNT << " test(s) passed.";
            std::clog << std::endl;
            return EXIT_SUCCESS;
        }
    }
#endif// TESTING_NO_AUTOMATIC_MAIN

#endif// SOLOSNAKE_TESTING_HPP
