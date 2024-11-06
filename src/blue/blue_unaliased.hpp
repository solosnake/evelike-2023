#ifndef solosnake_unaliased_hpp
#define solosnake_unaliased_hpp

#include <algorithm>
#include <utility>

/**
* An 'alias' for restrict, where available.

http://developers.sun.com/solaris/articles/cc_restrict.html

Some Typical Uses of the restrict Qualifier
-------------------------------------------

The complexity of the specification of the restrict qualifier reflects
the fact that C has a rich set of types and a dynamic notion of the type
of an object. For example, an object does not have a fixed type, but
acquires a type when referenced. Similarly, in some of the library
functions, the extent of an array object referenced through a pointer
parameter is dynamically determined, either by another parameter or by the
contents of the array. The full specification is necessary to determine the
precise meaning of a qualifier in any context, and so must be understood by
the compiler. Fortunately, C programmers only need to understand a few
simple patterns of usage explained in the following examples.

A compiler can assume that a file-scope restrict-qualified pointer is the
sole initial means of access to an object, much as if it were the declared
name of an array. This is useful for a dynamically allocated array whose
size is not known until run time. Note in the following example how a
single block of storage is effectively subdivided into two disjoint
objects.

float * restrict a1, * restrict a2;

void init(int n)
{
float * t = malloc(2 * n * sizeof(float));
a1 = t; // a1 refers to 1st half
a2 = t + n; // a2 refers to 2nd half
}

A compiler can assume that a restrict-qualified pointer that is a function
parameter is, at the beginning of each execution of the function, the sole
means of access to an object. Note that this assumption expires with the
end of each execution. In the following example, parameters a1 and a2 can
be assumed to refer to disjoint array objects because both are restrict
-qualified. This implies that each iteration of the loop is independent of
the others, and so the loop can be aggressively optimized.

void f1(int n, float * restrict a1, const float * restrict a2)
{
int i;
for ( i = 0; i < n; i++ )
a1[i] += a2[i];
}

A compiler can assume that a restrict-qualified pointer declared with
block scope is, during each execution of the block, the sole initial means
of access to an object. An invocation of the macro shown in the following
example is equivalent to an inline version of a call to the function f1
above.

# define f2(N,A1,A2) \
{ int n = (N); \
float * restrict a1 = (A1); \
float * restrict a2 = (A2); \
int i; \
for ( i = 0; i < n; i++ ) \
a1[i] += a2[i]; \
}

The restrict qualifier can be used in the declaration of a structure
member. A compiler can assume, when an identifier is declared that provides
a means of access to an object of that structure type, that the member
provides the sole initial means of access to an object of the type
specified in the member declaration. The duration of the assumption depends
on the scope of the identifier, not on the scope of the declaration of the
structure. Thus a compiler can assume that s1.a1 and s1.a2 below are used
to refer to disjoint objects for the duration of the whole program, but
that s2.a1 and s2.a2 are used to refer to disjoint objects only for the
duration of each invocation of the f3 function.

struct t
{
int n;
float * restrict a1, * restrict a2;
};

struct t s1;

void f3(struct t s2)
{ ,,,,
}


Unions and Typedefs
-------------------

The meaning of the restrict qualifier for a union member or in a type
definition is analogous. Just as an object with a declared name can be
aliased by an unqualified pointer, so can the object associated with a
restrict-qualified pointer.

This allows the restrict qualifier to be introduced more easily into
existing programs, and also allows restrict to be used in new programs
that call functions from libraries that do not use the qualifier. In
particular, a restrict-qualified pointer can be the actual argument for
a function parameter that is unqualified. On the other hand, it is
easier for a translator to find opportunities for optimization if as
many as possible of the pointers in a program are restrict-qualified.

*/

#if defined _MSC_VER
#   define unaliased __restrict
#   define unaliased_return __declspec(restrict)
#   define unaliased_function __declspec(restrict)
#elif defined __GNUC__
#   define unaliased __restrict
#   define unaliased_return
#   define unaliased_function
#else
#   define unaliased /*restrict not available*/
#   define unaliased_return
#   define unaliased_function
#endif

#ifndef NDEBUG
#   define ASSERT_UNALIASED(P0, P1, L0, L1) assert(is_unaliased(P0, P1, L0, L1))
#else
#   define ASSERT_UNALIASED(P0, P1, L0, L1) /* ASSERT_UNALIASED removed */
#endif

inline bool is_unaliased( const void* p0, size_t len0, const void* p1, size_t len1 )
{
    const unsigned char* b0 = reinterpret_cast<const unsigned char*>( p0 );
    const unsigned char* b1 = reinterpret_cast<const unsigned char*>( p1 );
    const unsigned char* pLo = std::min<const unsigned char*>( b0, b1 );
    const unsigned char* pHi = std::max<const unsigned char*>( b0, b1 );
    return ( ( pLo + ( pLo == b0 ? len0 : len1 ) ) <= pHi );
}

template <typename T> inline bool is_unaliased( const T* p0, const T* p1, size_t len0, size_t len1 )
{
    // Calls non template check.
    return is_unaliased( p0, sizeof( T ) * len0, p1, sizeof( T ) * len1 );
}

#endif
