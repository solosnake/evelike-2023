#ifndef solosnake_logging_hpp
#define solosnake_logging_hpp

#include <cassert>

/// Comment this pre-processor in/out to enable/disable logging. This might also
/// have been defined externally as a compilation option.
#ifndef SS_COMPILE_WITHOUT_LOGGING
//#   define SS_COMPILE_WITHOUT_LOGGING
#endif

#define SS_LOG_ALL              (0x3u)
#define SS_LOG_WRN              (0x2u)
#define SS_LOG_ERR              (0x1u)

/// By default we log only errors.
#define SS_DEFAULT_LOGGINGLEVEL (SS_LOG_ALL)

/// How many decimals are used in logging of numbers.
#define SS_LOG_PRECISION        (9)

/// Define as non zero to enable fixed width logging.
#define SS_LOG_FIXEDWIDTH       (1)

#ifdef SS_COMPILE_WITHOUT_LOGGING

#include <iosfwd>

#define ss_log(...)                                                                                \
    {                                                                                              \
    }
#define ss_dbg(...)                                                                                \
    {                                                                                              \
    }
#define ss_wrn(...)                                                                                \
    {                                                                                              \
    }
#define ss_err(...)                                                                                \
    {                                                                                              \
    }

namespace blue
{
    static const unsigned int g_ssLogLvl = 0;
    extern std::ostream* g_clog;
    extern std::ostream* g_cerr;

    inline void open_log( const char* )
    {
    }

    inline void close_log()
    {
    }
}

#else

#include <cstdarg>
#include <cstdio>
#include <iostream>

namespace blue
{
    extern unsigned int g_ssLogLvl;
    extern std::ostream* g_clog;
    extern std::ostream* g_cerr;

    /// Flushes and closes and open streams, and opens a new log file.
    /// Both cerr and clog streams will be piped to this.
    std::ostream* open_log( const char* );

    /// Flushed and closes the log file streams and makes cerr and clog the
    /// default streams again.
    void close_log();

    // clog

    template <typename T1> inline void ss_t_log( const T1& t1 ) noexcept
    {
        assert( g_clog );
        ( *g_clog ) << "LOG: " << t1 << '\n';
    }

    template <typename T1, typename T2> inline void ss_t_log( const T1& t1, const T2& t2 ) noexcept
    {
        assert( g_clog );
        ( *g_clog ) << "LOG: " << t1 << t2 << '\n';
    }

    template <typename T1, typename T2, typename T3>
    inline void ss_t_log( const T1& t1, const T2& t2, const T3& t3 ) noexcept
    {
        assert( g_clog );
        ( *g_clog ) << "LOG: " << t1 << t2 << t3 << '\n';
    }

    template <typename T1, typename T2, typename T3, typename T4>
    inline void ss_t_log( const T1& t1, const T2& t2, const T3& t3, const T4& t4 ) noexcept
    {
        assert( g_clog );
        ( *g_clog ) << "LOG: " << t1 << t2 << t3 << t4 << '\n';
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5>
    inline void ss_t_log( const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5 ) noexcept
    {
        assert( g_clog );
        ( *g_clog ) << "LOG: " << t1 << t2 << t3 << t4 << t5 << '\n';
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    inline void ss_t_log(
        const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6 ) noexcept
    {
        assert( g_clog );
        ( *g_clog ) << "LOG: " << t1 << t2 << t3 << t4 << t5 << t6 << '\n';
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    inline void ss_t_log( const T1& t1,
                          const T2& t2,
                          const T3& t3,
                          const T4& t4,
                          const T5& t5,
                          const T6& t6,
                          const T7& t7 ) noexcept
    {
        assert( g_clog );
        ( *g_clog ) << "LOG: " << t1 << t2 << t3 << t4 << t5 << t6 << t7 << '\n';
    }

    template <typename T1,
              typename T2,
              typename T3,
              typename T4,
              typename T5,
              typename T6,
              typename T7,
              typename T8>
    inline void ss_t_log( const T1& t1,
                          const T2& t2,
                          const T3& t3,
                          const T4& t4,
                          const T5& t5,
                          const T6& t6,
                          const T7& t7,
                          const T8& t8 ) noexcept
    {
        assert( g_clog );
        ( *g_clog ) << "LOG: " << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << '\n';
    }

    // dbg

    template <typename T1> inline void ss_t_dbg( const T1& t1 ) noexcept
    {
        assert( g_clog );
        ( *g_clog ) << "DBG: " << t1 << std::endl;
        ;
    }

    template <typename T1, typename T2> inline void ss_t_dbg( const T1& t1, const T2& t2 ) noexcept
    {
        ( *g_clog ) << "DBG: " << t1 << t2 << std::endl;
        ;
    }

    template <typename T1, typename T2, typename T3>
    inline void ss_t_dbg( const T1& t1, const T2& t2, const T3& t3 ) noexcept
    {
        assert( g_clog );
        ( *g_clog ) << "DBG: " << t1 << t2 << t3 << std::endl;
        ;
    }

    template <typename T1, typename T2, typename T3, typename T4>
    inline void ss_t_dbg( const T1& t1, const T2& t2, const T3& t3, const T4& t4 ) noexcept
    {
        assert( g_clog );
        ( *g_clog ) << "DBG: " << t1 << t2 << t3 << t4 << std::endl;
        ;
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5>
    inline void ss_t_dbg( const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5 ) noexcept
    {
        assert( g_clog );
        ( *g_clog ) << "DBG: " << t1 << t2 << t3 << t4 << t5 << std::endl;
        ;
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    inline void ss_t_dbg(
        const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6 ) noexcept
    {
        assert( g_clog );
        ( *g_clog ) << "DBG: " << t1 << t2 << t3 << t4 << t5 << t6 << std::endl;
        ;
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    inline void ss_t_dbg( const T1& t1,
                          const T2& t2,
                          const T3& t3,
                          const T4& t4,
                          const T5& t5,
                          const T6& t6,
                          const T7& t7 ) noexcept
    {
        assert( g_clog );
        ( *g_clog ) << "DBG: " << t1 << t2 << t3 << t4 << t5 << t6 << t7 << std::endl;
        ;
    }

    template <typename T1,
              typename T2,
              typename T3,
              typename T4,
              typename T5,
              typename T6,
              typename T7,
              typename T8>
    inline void ss_t_dbg( const T1& t1,
                          const T2& t2,
                          const T3& t3,
                          const T4& t4,
                          const T5& t5,
                          const T6& t6,
                          const T7& t7,
                          const T8& t8 ) noexcept
    {
        assert( g_clog );
        ( *g_clog ) << "DBG: " << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << std::endl;
        ;
    }

    // wrn

    template <typename T1> inline void ss_t_wrn( const T1& t1 ) noexcept
    {
        assert( g_clog );
        ( *g_clog ) << "WRN: " << t1 << std::endl;
        ;
    }

    template <typename T1, typename T2> inline void ss_t_wrn( const T1& t1, const T2& t2 ) noexcept
    {
        assert( g_clog );
        ( *g_clog ) << "WRN: " << t1 << t2 << std::endl;
        ;
    }

    template <typename T1, typename T2, typename T3>
    inline void ss_t_wrn( const T1& t1, const T2& t2, const T3& t3 ) noexcept
    {
        ( *g_clog ) << "WRN: " << t1 << t2 << t3 << std::endl;
        ;
    }

    template <typename T1, typename T2, typename T3, typename T4>
    inline void ss_t_wrn( const T1& t1, const T2& t2, const T3& t3, const T4& t4 ) noexcept
    {
        assert( g_clog );
        ( *g_clog ) << "WRN: " << t1 << t2 << t3 << t4 << std::endl;
        ;
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5>
    inline void ss_t_wrn( const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5 ) noexcept
    {
        assert( g_clog );
        ( *g_clog ) << "WRN: " << t1 << t2 << t3 << t4 << t5 << std::endl;
        ;
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    inline void ss_t_wrn(
        const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6 ) noexcept
    {
        assert( g_clog );
        ( *g_clog ) << "WRN: " << t1 << t2 << t3 << t4 << t5 << t6 << std::endl;
        ;
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    inline void ss_t_wrn( const T1& t1,
                          const T2& t2,
                          const T3& t3,
                          const T4& t4,
                          const T5& t5,
                          const T6& t6,
                          const T7& t7 ) noexcept
    {
        assert( g_clog );
        ( *g_clog ) << "WRN: " << t1 << t2 << t3 << t4 << t5 << t6 << t7 << std::endl;
        ;
    }

    template <typename T1,
              typename T2,
              typename T3,
              typename T4,
              typename T5,
              typename T6,
              typename T7,
              typename T8>
    inline void ss_t_wrn( const T1& t1,
                          const T2& t2,
                          const T3& t3,
                          const T4& t4,
                          const T5& t5,
                          const T6& t6,
                          const T7& t7,
                          const T8& t8 ) noexcept
    {
        assert( g_clog );
        ( *g_clog ) << "WRN: " << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << std::endl;
        ;
    }

    // err

    template <typename T1> inline void ss_t_err( const T1& t1 ) noexcept
    {
        assert( g_cerr );
        ( *g_cerr ) << "ERR: " << t1 << std::endl;
        ;
    }

    template <typename T1, typename T2> inline void ss_t_err( const T1& t1, const T2& t2 ) noexcept
    {
        assert( g_cerr );
        ( *g_cerr ) << "ERR: " << t1 << t2 << std::endl;
        ;
    }

    template <typename T1, typename T2, typename T3>
    inline void ss_t_err( const T1& t1, const T2& t2, const T3& t3 ) noexcept
    {
        assert( g_cerr );
        ( *g_cerr ) << "ERR: " << t1 << t2 << t3 << std::endl;
        ;
    }

    template <typename T1, typename T2, typename T3, typename T4>
    inline void ss_t_err( const T1& t1, const T2& t2, const T3& t3, const T4& t4 ) noexcept
    {
        assert( g_cerr );
        ( *g_cerr ) << "ERR: " << t1 << t2 << t3 << t4 << std::endl;
        ;
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5>
    inline void ss_t_err( const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5 ) noexcept
    {
        assert( g_cerr );
        ( *g_cerr ) << "ERR: " << t1 << t2 << t3 << t4 << t5 << std::endl;
        ;
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    inline void ss_t_err(
        const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6 ) noexcept
    {
        assert( g_cerr );
        ( *g_cerr ) << "ERR: " << t1 << t2 << t3 << t4 << t5 << t6 << std::endl;
        ;
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    inline void ss_t_err( const T1& t1,
                          const T2& t2,
                          const T3& t3,
                          const T4& t4,
                          const T5& t5,
                          const T6& t6,
                          const T7& t7 ) noexcept
    {
        assert( g_cerr );
        ( *g_cerr ) << "ERR: " << t1 << t2 << t3 << t4 << t5 << t6 << t7 << std::endl;
        ;
    }

    template <typename T1,
              typename T2,
              typename T3,
              typename T4,
              typename T5,
              typename T6,
              typename T7,
              typename T8>
    inline void ss_t_err( const T1& t1,
                          const T2& t2,
                          const T3& t3,
                          const T4& t4,
                          const T5& t5,
                          const T6& t6,
                          const T7& t7,
                          const T8& t8 ) noexcept
    {
        assert( g_cerr );
        ( *g_cerr ) << "ERR: " << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << std::endl;
    }
}

#ifndef NDEBUG
#define ss_dbg(...)                                                                                \
    {                                                                                              \
        if (blue::g_ssLogLvl > (SS_LOG_WRN))                                                       \
        {                                                                                          \
            blue::ss_t_dbg(__VA_ARGS__);                                                           \
        }                                                                                          \
    }
#else
#define ss_dbg(...) /* GONE */
#endif

#define ss_log(...)                                                                                \
    {                                                                                              \
        if (blue::g_ssLogLvl > (SS_LOG_WRN))                                                       \
        {                                                                                          \
            blue::ss_t_log(__VA_ARGS__);                                                           \
        }                                                                                          \
    }
#define ss_wrn(...)                                                                                \
    {                                                                                              \
        if (blue::g_ssLogLvl >= (SS_LOG_WRN))                                                      \
        {                                                                                          \
            blue::ss_t_wrn(__VA_ARGS__);                                                           \
        }                                                                                          \
    }
#define ss_err(...)                                                                                \
    {                                                                                              \
        if (blue::g_ssLogLvl >= (SS_LOG_ERR))                                                      \
        {                                                                                          \
            blue::ss_t_err(__VA_ARGS__);                                                           \
        }                                                                                          \
    }

#endif // SS_COMPILE_WITHOUT_LOGGING

#endif
