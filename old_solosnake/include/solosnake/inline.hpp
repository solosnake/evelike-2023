#ifndef solosnake_inline_hpp
#define solosnake_inline_hpp

#if defined(_MSC_VER)
#define SS_INLINE __forceinline
#elif defined(__GNUC__) && __GNUC__ > 3
#define SS_INLINE inline __attribute__((__always_inline__))
#else
#define SS_INLINE inline
#endif

#endif
