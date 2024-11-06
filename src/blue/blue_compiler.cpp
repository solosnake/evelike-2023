#include <algorithm>
#include <cassert>
#include <cstring>
#include <fstream>
#include <memory>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include "solosnake/blue/blue_compiler.hpp"
#include "solosnake/blue/blue_compilation_result.hpp"
#include "solosnake/blue/blue_instruction.hpp"
#include "solosnake/blue/blue_instructions.hpp"
#include "solosnake/blue/blue_logging.hpp"
#include "solosnake/blue/blue_throw.hpp"

#ifndef NDEBUG
#   include <iostream>
#   include <iomanip>
#endif

using namespace blue;

// Enable to print annoying list of opcode hex values ;)
// #define BLUE_PRINT_OPCODES_FROM_CTOR

namespace
{
#   ifdef _MSC_VER
#       define SNPRINTF             _snprintf
#   else
#       define SNPRINTF             snprintf
#   endif

#   define TRAILING_COMMENT         "(?:(?:[ \\t]*)|(?:[ \\t]*//.*))?"
#   define LEADING_COMMENT          "(?:[ \\t]*//.*))?"
#   define SPACE                    "(?:[ \\t]*)"
#   define TEXT_WITH_NEWLINE        "\"([\\S ]*?)\""
#   define FUNCTION_NAME            "(\\w+)"
#   define SPACES                   "(?:[ \\t]+)"
#   define LEADING_SPACE            SPACE
#   define VALUE                    "(\\d+)"
#   define INTEGER                  "([\\+\\-]?\\d+)"
#   define INDIRECTION              "(?:\\[" SPACE VALUE SPACE "\\])"
#   define INDIRECTION2             "(?:\\[" SPACE "\\[" SPACE VALUE SPACE "\\]" SPACE "\\])"
#   define INTEGER_OR_INDIRECTION     "(" INTEGER "|" INDIRECTION ")"
#   define VALUE_OR_INDIRECTION     "(" VALUE "|" INDIRECTION ")"
#   define DESTINATION              "(" INDIRECTION "|" INDIRECTION2 ")"

    constexpr auto WINDOWS_PENULTIMATE_LINE_END_CHAR = '\r';
    constexpr auto BLUE_REGEX_COUNT                  = 22;
    constexpr auto MAX_FUNCTION_NAME_LENGTH          = 31;

    //-------------------------------------------------------------------------

    /// Reads a line into std::string, stripping the windows line ending if
    /// it exists.
    std::istream& read_and_strip_line_endings( std::istream& is, std::string& str )
    {
        getline( is, str );
        const size_t n = str.size();
        if( n > 0 && str[n - 1] == WINDOWS_PENULTIMATE_LINE_END_CHAR )
        {
            str.resize( str.size() - 1 );
        }

        return is;
    }

    //-------------------------------------------------------------------------
    static constexpr char g_regCommentLine[] = {
        TRAILING_COMMENT
    };

    static constexpr char g_regValue[] = {
        LEADING_SPACE
        INTEGER
        TRAILING_COMMENT
    };

    static constexpr char g_regAsm[] = {
        LEADING_SPACE
        "0x([0-9ABCDEFabcdef]{4})[ \\t]+"
        "0x([0-9ABCDEFabcdef]{4})[ \\t]+"
        "0x([0-9ABCDEFabcdef]{4})[ \\t]+"
        "0x([0-9ABCDEFabcdef]{4})"
        TRAILING_COMMENT
    };

    /// X OP= Y
    static constexpr char g_regMathOp1[] = {
        LEADING_SPACE
        INDIRECTION
        SPACE
        "([\\+\\*\\^\\|\\-/~&=!<>%])="
        SPACE
        INTEGER_OR_INDIRECTION
        TRAILING_COMMENT
    };

    /// X = Y OP Z
    static constexpr char g_regMathOp2[] = {
        LEADING_SPACE
        DESTINATION
        SPACE
        "="
        SPACE
        INTEGER_OR_INDIRECTION
        SPACE
        "([\\+\\*\\^\\|\\-/~&=!<>%]+)"
        SPACE
        INTEGER_OR_INDIRECTION
        TRAILING_COMMENT
    };

    // if_1_op_goto
    static constexpr char g_reg1OpIfGoto[] = {
        LEADING_SPACE
        "if"
        SPACE
        "([!]?)"
        SPACE
        INTEGER_OR_INDIRECTION
        SPACE
        "goto"
        SPACE
        VALUE_OR_INDIRECTION
        TRAILING_COMMENT
    };

    // if_2_op_goto
    static constexpr char g_reg2OpIfGoto[] = {
        LEADING_SPACE
        "if"
        SPACES
        INTEGER_OR_INDIRECTION
        SPACE
        "([=!<>]{1,2})"
        SPACE
        INTEGER_OR_INDIRECTION
        SPACES
        "goto"
        SPACES
        VALUE_OR_INDIRECTION
        TRAILING_COMMENT
    };

    // Remember  .[{()\*+?|^$
    static constexpr char g_regAssert1[] = {
        LEADING_SPACE
        "assert"
        SPACE
        "\\("
        SPACE
        INTEGER_OR_INDIRECTION
        SPACE
        "([=!<>&\\|]{1,2})"
        SPACE
        INTEGER_OR_INDIRECTION
        SPACE
        "\\)"
        TRAILING_COMMENT
    };

    static constexpr char g_regAssert2[] = {
        LEADING_SPACE
        "assert"
        SPACE
        "\\("
        SPACE
        "([!]?)"
        SPACE
        INTEGER_OR_INDIRECTION
        SPACE
        "\\)"
        TRAILING_COMMENT
    };

    static constexpr char g_regAssign[] = {
        LEADING_SPACE
        DESTINATION
        SPACE
        "="
        SPACE
        INTEGER_OR_INDIRECTION
        TRAILING_COMMENT
    };

    static constexpr char g_regGoto[] = {
        LEADING_SPACE
        "goto"
        SPACES
        VALUE_OR_INDIRECTION
        TRAILING_COMMENT
    };

    static constexpr char g_regFn_0_0[] = {
        LEADING_SPACE
        FUNCTION_NAME
        SPACE
        "\\("
        SPACE
        "\\)"
        TRAILING_COMMENT
    };

    static constexpr char g_regFn_0_1[] = {
        LEADING_SPACE
        FUNCTION_NAME
        SPACE
        "\\("
        SPACE
        INTEGER_OR_INDIRECTION
        SPACE
        "\\)"
        TRAILING_COMMENT
    };

    static constexpr char g_regFn_0_1i[] = {
        LEADING_SPACE
        FUNCTION_NAME
        SPACE
        "\\("
        SPACE
        INDIRECTION
        SPACE
        "\\)"
        TRAILING_COMMENT
    };

    static constexpr char g_regFn_0_2[] = {
        LEADING_SPACE
        FUNCTION_NAME
        SPACE
        "\\("
        SPACE
        INTEGER_OR_INDIRECTION
        SPACE
        ","
        SPACE
        INTEGER_OR_INDIRECTION
        SPACE
        "\\)"
        TRAILING_COMMENT
    };

    static constexpr char g_regFn_0_2i[] = {
        LEADING_SPACE
        FUNCTION_NAME
        SPACE
        "\\("
        SPACE
        INDIRECTION
        SPACE
        ","
        SPACE
        INDIRECTION
        SPACE
        "\\)"
        TRAILING_COMMENT
    };

    static constexpr char g_regFn_0_3[] = {
        LEADING_SPACE
        FUNCTION_NAME
        SPACE
        "\\("
        SPACE
        INTEGER_OR_INDIRECTION
        SPACE
        ","
        SPACE
        INTEGER_OR_INDIRECTION
        SPACE
        ","
        SPACE
        INTEGER_OR_INDIRECTION
        SPACE
        "\\)"
        TRAILING_COMMENT
    };

    static constexpr char g_regFn_0_1i2[] = {
        LEADING_SPACE
        FUNCTION_NAME
        SPACE
        "\\("
        SPACE
        INDIRECTION
        SPACE
        ","
        SPACE
        INTEGER_OR_INDIRECTION
        SPACE
        ","
        SPACE
        INTEGER_OR_INDIRECTION
        SPACE
        "\\)"
        TRAILING_COMMENT
    };

    static constexpr char g_regFn_1_0[] = {
        LEADING_SPACE
        DESTINATION
        SPACE
        "="
        SPACE
        FUNCTION_NAME
        SPACE
        "\\("
        SPACE
        "\\)"
        TRAILING_COMMENT
    };

    static constexpr char g_regFn_1_1[] = {
        LEADING_SPACE
        DESTINATION
        SPACE
        "="
        SPACE
        FUNCTION_NAME
        SPACE
        "\\("
        SPACE
        INTEGER_OR_INDIRECTION
        SPACE
        "\\)"
        TRAILING_COMMENT
    };

    /// Specialised version of g_regFn_1_1.
    static constexpr char g_regFn_1_1i[] = {
        LEADING_SPACE
        DESTINATION
        SPACE
        "="
        SPACE
        FUNCTION_NAME
        SPACE
        "\\("
        SPACE
        INDIRECTION
        SPACE
        "\\)"
        TRAILING_COMMENT
    };

    static constexpr char g_regFn_1_2[] = {
        LEADING_SPACE
        DESTINATION
        SPACE
        "="
        SPACE
        FUNCTION_NAME
        SPACE
        "\\("
        SPACE
        INTEGER_OR_INDIRECTION
        SPACE
        ","
        SPACE
        INTEGER_OR_INDIRECTION
        SPACE
        "\\)"
        TRAILING_COMMENT
    };

    static constexpr char g_regFn_1_2i[] = {
        LEADING_SPACE
        DESTINATION
        SPACE
        "="
        SPACE
        FUNCTION_NAME
        SPACE
        "\\("
        SPACE
        INDIRECTION
        SPACE
        ","
        SPACE
        INDIRECTION
        SPACE
        "\\)"
        TRAILING_COMMENT
    };

    static constexpr char g_regPrintText[] = {
        LEADING_SPACE
        "print"
        SPACE
        "\\("
        SPACE
        "\"([\\S ]{0,6})\""
        SPACE
        "\\)"
        TRAILING_COMMENT
    };

    static constexpr char g_regFn_0_1_1i[] = {
        LEADING_SPACE
        FUNCTION_NAME
        SPACE
        "\\("
        SPACE
        INTEGER_OR_INDIRECTION
        SPACE
        ","
        SPACE
        INDIRECTION
        SPACE
        "\\)"
        TRAILING_COMMENT
    };

    //-------------------------------------------------------------------------

    enum ArgPos : std::uint16_t
    {
        Value0, Value1, Value2, ValueOne, ValueZero
    };

    struct Match_values
    {
        unsigned int   flags;
        unsigned int   args[5];

        Match_values() : flags( 0u )
        {
            args[ValueOne]  = 1u;
            args[ValueZero] = 0u;
        }
    };

    struct Match_result
    {
        char            functionName[ MAX_FUNCTION_NAME_LENGTH + 1 ];
        Match_values    values;

        Match_result()
        {
            functionName[0] = 0;
        }

        void copy_function_name( const char* txt, size_t len )
        {
            assert( len <= MAX_FUNCTION_NAME_LENGTH );
            len = std::min<size_t>( len, MAX_FUNCTION_NAME_LENGTH );
            memcpy( functionName, txt, len );
            functionName[len] = 0;
        }
    };

    class Syntax
    {
    public:

        /// @param c Compile text.
        /// @param d Decompile text.
        Syntax( const char* c, const char* d, std::uint16_t opcode );

        Syntax( const char* c, const char* d, std::uint16_t opcode, ArgPos placeArg0 );

        Syntax( const char* c, const char* d, std::uint16_t opcode, ArgPos placeArg0, ArgPos placeArg1 );

        Syntax( const char* c, const char* d, std::uint16_t opcode, ArgPos placeArg0, ArgPos placeArg1, ArgPos placeArg2 );

        Instruction compile( const Match_values& values ) const;

        /// This std::string is used to uniquely identify this Syntax within a set within a Matcher.
        /// It can be the empty std::string if there is only one Syntax in the Matcher.
        const char* text() const            { return text_; }

        const char* decompile_text() const   { return decompile_text_; }

        std::uint16_t opcode() const             { return opcode_; }

    private:

        const char*     text_;
        const char*     decompile_text_;
        std::uint16_t   opcode_;
        std::uint16_t   arg_places_[3];
        uint32_t        args_count_;
    };

    Syntax::Syntax( const char* c, const char* d, std::uint16_t opcode )
        : text_( c )
        , decompile_text_( d )
        , opcode_( opcode )
        , args_count_( 0u )
    {
        arg_places_[0] = 0;
        arg_places_[1] = 0;
        arg_places_[2] = 0;
        assert( c == nullptr || strlen( c ) < MAX_FUNCTION_NAME_LENGTH );
    }

    Syntax::Syntax( const char* c, const char* d, std::uint16_t opcode,
                    ArgPos placeArg0 )
        : text_( c )
        , decompile_text_( d )
        , opcode_( opcode )
        , args_count_( 1u )
    {
        assert( c == nullptr || strlen( c ) < MAX_FUNCTION_NAME_LENGTH );
        arg_places_[0] = placeArg0;
        arg_places_[1] = 0;
        arg_places_[2] = 0;
    }

    Syntax::Syntax( const char* c, const char* d, std::uint16_t opcode,
                    ArgPos placeArg0, ArgPos placeArg1 )
        : text_( c )
        , decompile_text_( d )
        , opcode_( opcode )
        , args_count_( 2u )
    {
        assert( c == nullptr || strlen( c ) < MAX_FUNCTION_NAME_LENGTH );
        arg_places_[0] = placeArg0;
        arg_places_[1] = placeArg1;
        arg_places_[2] = 0;
    }

    Syntax::Syntax( const char* c, const char* d, std::uint16_t opcode,
                    ArgPos placeArg0, ArgPos placeArg1, ArgPos placeArg2 )
        : text_( c )
        , decompile_text_( d )
        , opcode_( opcode )
        , args_count_( 3u )
    {
        assert( c == nullptr || strlen( c ) < MAX_FUNCTION_NAME_LENGTH );
        arg_places_[0] = placeArg0;
        arg_places_[1] = placeArg1;
        arg_places_[2] = placeArg2;
    }

    Instruction Syntax::compile( const Match_values& values ) const
    {
        if( 0 == args_count_ )
        {
            return Instruction::from_asm( opcode_ | static_cast<std::uint16_t>( values.flags ) );
        }
        else if( 1u == args_count_ )
        {
            return Instruction::from_asm( opcode_ | static_cast<std::uint16_t>( values.flags ),
                                          static_cast<std::uint16_t>( values.args[arg_places_[0]] ) );
        }
        else if( 2u == args_count_ )
        {
            return Instruction::from_asm( opcode_ | static_cast<std::uint16_t>( values.flags ),
                                          static_cast<std::uint16_t>( values.args[arg_places_[0]] ),
                                          static_cast<std::uint16_t>( values.args[arg_places_[1]] ) );
        }

        return Instruction::from_asm( opcode_ | static_cast<std::uint16_t>( values.flags ),
                                      static_cast<std::uint16_t>( values.args[arg_places_[0]] ),
                                      static_cast<std::uint16_t>( values.args[arg_places_[1]] ),
                                      static_cast<std::uint16_t>( values.args[arg_places_[2]] ) );
    }

    //-------------------------------------------------------------------------

    struct sort_by_string
    {
        bool operator()( const char* str1, const char* str2 ) const
        {
            // A value greater than zero indicates that the first character that
            // does not match has a greater value in str1 than in str2;
            if( str1 != nullptr && str2 != nullptr )
            {
                return strcmp( str1, str2 ) < 0;
            }
            else
            {
                return str1 < str2;
            }
        }
    };

    inline void replace( std::string& str, const std::string_view& from, const std::string& to )
    {
        size_t start_pos = str.find( from );
        if( start_pos != std::string::npos )
        {
            str.replace( start_pos, from.length(), to );
        }
    }

#ifndef NDEBUG
    void print( const std::smatch& what )
    {
        std::cout << what.size() << " matches:\n";

        for( unsigned int i = 0u; i < what.size(); ++i )
        {
            std::cout << "$" << i << " = \"" << what[i] << "\"\n";
        }
    }
#else
    void print( const std::smatch& )
    {
    }
#endif

    template<typename T>
    T extract( int n, const std::smatch& what )
    {
        assert( ( what[n].matched || what[1 + n].matched ) );
        return static_cast<T>( atoi( static_cast<const char*>( &*what[ what[n].matched ? n : 1u + n ].first ) ) );
    }

    /// Performs several common tasks: extracts the value from the match,
    /// where the value is either located at n or n+1. By
    /// convention, if the first one is the match, the result is a value,
    /// and if the second matches then the result is an indirection.
    /// Returns true if the match was at the 'n' position.
    bool match(
        unsigned int& v,
        unsigned int& flags,
        unsigned int byValueFlag,
        int n,
        const std::smatch& what )
    {
        assert( ( what[n].matched || what[1u + n].matched ) );

        v = extract<unsigned int>( n, what );

        if( what[n].matched )
        {
            // First matched, which is a value. Add value flag.
            flags |= byValueFlag;
        }

        return what[n].matched;
    }

    //-------------------------------------------------------------------------

    /// Associated with a single regex, when a line of code matches the regex
    /// it calls a virtual member which must extracts a Match_result from the
    /// regex's match. This Match_result is used to find a Syntax which has
    /// the same function name, and this Syntax builds an instruction from the
    /// Match_result.
    class Matcher
    {
    public:

        Matcher( const char* r, const unsigned int min_matches );

        virtual ~Matcher();

        bool compiles( const std::string_view& line, Instruction& ins ) const;

        bool decompiles( const Instruction& x, std::string& s ) const;

        void add_syntax( const Syntax& s );

        void append_possible_commands( const char* s, size_t len, std::vector<const char*>& possibles ) const;

        void append_opcodes( std::vector<std::uint16_t>& codes ) const;

    private:

        virtual void make_match_result( const std::smatch& what, Match_result& result ) const = 0;

    private:

        std::regex                                      regex_;
        unsigned int                                    min_matches_;
        std::map<const char*, Syntax, sort_by_string>   syntaxes_;
        std::map<std::uint16_t, const char*>            decompilers_;

        static constexpr std::string_view arg0_ = "$A0";
        static constexpr std::string_view arg1_ = "$A1";
        static constexpr std::string_view arg2_ = "$A2";
        static constexpr std::string_view op2_  = "$OP2";
        static constexpr std::string_view text_ = "$TEXT";
    };

    Matcher::Matcher( const char* r, const unsigned int min_matches )
        : regex_( r )
        , min_matches_( min_matches )
    {
    }

    Matcher::~Matcher()
    {
    }

    bool Matcher::compiles( const std::string_view& line, Instruction& ins ) const
    {
        bool compiled = false;

        std::smatch what;

        const std::string str_line(line);

        if( std::regex_match( str_line, what, regex_ ) )
        {
            if( what.size() >= min_matches_ )
            {
                Match_result result;
                this->make_match_result( what, result );
                auto i = syntaxes_.find( result.functionName );
                if( i != syntaxes_.cend() )
                {
                    ins = i->second.compile( result.values );
                    compiled = true;
                }
            }
        }

        return compiled;
    }

    bool Matcher::decompiles( const Instruction& x, std::string& s ) const
    {
        bool decompiled = false;

        auto d = decompilers_.find( x.cmd() );

        if( d != decompilers_.end() )
        {
            char ARG0[16] = {};
            char ARG1[16] = {};
            char ARG2[16] = {};

            assert( d->second );
            std::string xstr = d->second;

            ss_dbg( xstr );

            union
            {
                std::uint16_t        args_;
                char            chars_[3];
            } args_and_chars2;

            args_and_chars2.chars_[2] = 0;
            args_and_chars2.args_ = x.arg( 2u );

            if( x.flags() & BLUE_VALUE_ARG0 )
            {
                SNPRINTF( ARG0, 16, "%d", static_cast<int16_t>( x.arg( 0u ) ) );
            }
            else
            {
                SNPRINTF( ARG0, 16, "[%u]", x.arg( 0u ) );
            }

            if( x.flags() & BLUE_VALUE_ARG1 )
            {
                SNPRINTF( ARG1, 16, "%d", static_cast<int16_t>( x.arg( 1u ) ) );
            }
            else
            {
                SNPRINTF( ARG1, 16, "[%u]", x.arg( 1u ) );
            }

            if( x.flags() & BLUE_VALUE_ARG2 )
            {
                SNPRINTF( ARG2, 16, "%d", static_cast<int16_t>( x.arg( 2u ) ) );
            }
            else
            {
                SNPRINTF( ARG2, 16, "[%u]", x.arg( 2u ) );
            }

            ss_dbg( xstr );

            replace( xstr, Matcher::arg0_, ARG0 );
            ss_dbg( xstr );

            replace( xstr, Matcher::arg1_, ARG1 );
            ss_dbg( xstr );

            replace( xstr, Matcher::arg2_, ARG2 );
            ss_dbg( xstr );

            replace( xstr, Matcher::op2_,  args_and_chars2.chars_ );
            ss_dbg( xstr );

            replace( xstr, Matcher::text_, x.args_as_string() );
            ss_dbg( xstr );

            s = move( xstr );
            decompiled = true;
        }

        return decompiled;
    }

    void Matcher::add_syntax( const Syntax& s )
    {
        assert( syntaxes_.find( s.text() ) == syntaxes_.cend() );

        syntaxes_.insert( std::map<const char*, Syntax, sort_by_string>::value_type( s.text(), s ) );

        if( s.decompile_text() )
        {
            decompilers_.insert( std::map<std::uint16_t, const char*>::value_type( s.opcode(), s.decompile_text() ) );
        }

        assert( syntaxes_.find( s.text() ) != syntaxes_.cend() );
    }

    void Matcher::append_possible_commands( const char* s, size_t len, std::vector<const char*>& possibles ) const
    {
        for_each( syntaxes_.cbegin(),
                  syntaxes_.cend(),
                  [&]( const std::map<const char*, Syntax, sort_by_string>::value_type & v )
        {
            if( 0 == strncmp( s, v.second.text(), len ) )
            {
                possibles.push_back( v.second.text() );
            }
        } );
    }

    void Matcher::append_opcodes( std::vector<std::uint16_t>& codes ) const
    {
        for_each( syntaxes_.cbegin(),
                  syntaxes_.cend(),
                  [&]( const std::map<const char*, Syntax, sort_by_string>::value_type & v )
        {
            codes.push_back( v.second.opcode() );
        } );
    }

    //-------------------------------------------------------------------------

    class Match_comment_line : public Matcher
    {
    public:

        Match_comment_line();

        void make_match_result( const std::smatch&, Match_result& ) const override
        {
        }
    };

    class Match_value : public Matcher
    {
    public:

        Match_value();

        void make_match_result( const std::smatch& what, Match_result& mr ) const override
        {
            mr.values.flags = BLUE_VALUE_ARG0;
            mr.values.args[0] = static_cast<std::uint16_t>( extract<int16_t>( 1, what ) );
        }
    };

    class Match_math_op1 : public Matcher
    {
    public:

        Match_math_op1();

        void make_match_result( const std::smatch& what, Match_result& mr ) const override
        {
            unsigned int dstValue;
            unsigned int opValue;

            // BLUE_VALUE_ARG0 here means one level of indirection.
            unsigned int flags = BLUE_VALUE_ARG0;

            dstValue = extract<std::uint16_t>( 1, what );
            match( opValue, flags, BLUE_VALUE_ARG2, 4, what );

            mr.functionName[0] = *what[2].first;
            mr.functionName[1] = 0;
            mr.values.flags    = flags;
            mr.values.args[0]  = dstValue;
            mr.values.args[1]  = dstValue;
            mr.values.args[2]  = opValue;
        }
    };

    class Match_math_op2 : public Matcher
    {
    public:

        Match_math_op2();

        void make_match_result( const std::smatch& what, Match_result& mr ) const override
        {
            assert( what[7].matched );

            // Check operator length is 1 or 2 (<, <= etc)
            assert( what[7].length() == 1 || what[7].length() == 2 );

            unsigned int dstValue = 0u;
            unsigned int op1Value = 0u;
            unsigned int op2Value = 0u;
            unsigned int flags    = 0u;

            match( dstValue, flags, BLUE_VALUE_ARG0, 2, what );
            match( op1Value, flags, BLUE_VALUE_ARG1, 5, what );
            match( op2Value, flags, BLUE_VALUE_ARG2, 9, what );

            mr.copy_function_name( &*what[7].first, what[7].length() );
            mr.values.flags   = flags;
            mr.values.args[0] = dstValue;
            mr.values.args[1] = op1Value;
            mr.values.args[2] = op2Value;
        }
    };

    class Match_ifgoto_1op : public Matcher
    {
    public:

        Match_ifgoto_1op();

        void make_match_result( const std::smatch& what, Match_result& mr ) const override
        {
            unsigned int dstValue = 0u;
            unsigned int op1Value = 0u;
            unsigned int flags    = 0u;
            const bool invert     = what[1].matched && what[1].length() == 1;
            const bool opIsValue  = match( op1Value, flags, BLUE_VALUE_ARG0, 3, what );

            // Use short cut logic here if the operand is a raw value.
            if( opIsValue )
            {
                match( dstValue, flags, BLUE_VALUE_ARG0, 6, what );

                mr.values.args[1] = 0;
                mr.values.args[2] = 0;
                mr.values.flags   = flags;

                // If 3 was the match then the operand is a raw integer and
                // we can already deduce the result as its const.
                if( invert )
                {
                    if( ! op1Value )
                    {
                        mr.functionName[0] = 'g';
                        mr.functionName[1] = 'o';
                        mr.functionName[2] = 't';
                        mr.functionName[3] = 'o';
                        mr.functionName[4] = 0;
                        mr.values.args[0] = dstValue;
                    }
                    else
                    {
                        mr.functionName[0] = 0;
                        mr.values.args[0] = 0;
                    }
                }
                else
                {
                    if( op1Value )
                    {
                        mr.functionName[0] = 'g';
                        mr.functionName[1] = 'o';
                        mr.functionName[2] = 't';
                        mr.functionName[3] = 'o';
                        mr.functionName[4] = 0;
                        mr.values.args[0] = dstValue;
                    }
                    else
                    {
                        mr.functionName[0] = 0;
                        mr.values.args[0] = 0;
                    }
                }
            }
            else
            {
                match( dstValue, flags, BLUE_VALUE_ARG2, 6, what );

                if( invert )
                {
                    // if [X] == 0 goto Y // BLUE_ASM_JMP_EQ
                    mr.functionName[0] = 'i';
                    mr.functionName[1] = 'f';
                    mr.functionName[2] = ' ';
                    mr.functionName[3] = '!';
                    mr.functionName[4] = 0;
                    mr.values.flags   = flags | BLUE_VALUE_ARG1;
                    mr.values.args[0] = op1Value;
                    mr.values.args[1] = 0;
                    mr.values.args[2] = dstValue;
                }
                else
                {
                    // if [X] != 0 goto Y // BLUE_ASM_JMP_NEQ
                    mr.functionName[0] = 'i';
                    mr.functionName[1] = 'f';
                    mr.functionName[2] = 0;
                    mr.values.flags   = flags | BLUE_VALUE_ARG1;
                    mr.values.args[0] = op1Value;
                    mr.values.args[1] = 0;
                    mr.values.args[2] = dstValue;
                }
            }
        }
    };

    class Match_ifgoto_2op : public Matcher
    {
    public:

        Match_ifgoto_2op();

        void make_match_result( const std::smatch& what, Match_result& mr ) const override
        {
            const size_t operatorLength = what[4].length();
            assert( operatorLength == 1u || operatorLength == 2u );

            unsigned int dstValue = 0u;
            unsigned int op1Value = 0u;
            unsigned int op2Value = 0u;
            unsigned int flags    = 0u;

            match( op1Value, flags, BLUE_VALUE_ARG0, 2, what );
            match( op2Value, flags, BLUE_VALUE_ARG1, 6, what );
            match( dstValue, flags, BLUE_VALUE_ARG2, 9, what );

            mr.functionName[0] = 'i';
            mr.functionName[1] = 'f';
            mr.functionName[2] = ' ';
            mr.functionName[3] = *what[4].first;
            mr.functionName[4] = ( operatorLength == 2 ) ? *( what[4].first + 1 ) : 0;
            mr.functionName[5] = 0;
            mr.values.flags   = flags;
            mr.values.args[0] = op1Value;
            mr.values.args[1] = op2Value;
            mr.values.args[2] = dstValue;
        }
    };

    class Match_assert1 : public Matcher
    {
    public:

        Match_assert1();

        void make_match_result( const std::smatch& what, Match_result& mr ) const override
        {
            const size_t operatorLength = what[4].length();

            assert( operatorLength == 1u || operatorLength == 2u );

            unsigned int op1Value = 0u;
            unsigned int op2Value = 0u;
            unsigned int flags    = 0u;

            match( op1Value, flags, BLUE_VALUE_ARG0, 2, what );
            match( op2Value, flags, BLUE_VALUE_ARG1, 6, what );

            const char* opText = &*what[4].first;
            std::uint16_t input = static_cast<uint8_t>( opText[0] );

            if( operatorLength > 1 )
            {
                input |= std::uint16_t( static_cast<uint8_t>( opText[1] ) ) << 8;
            }

            mr.functionName[0] = 'a';
            mr.functionName[1] = 's';
            mr.functionName[2] = 's';
            mr.functionName[3] = 'e';
            mr.functionName[4] = 'r';
            mr.functionName[5] = 't';
            mr.functionName[6] = 0;
            mr.values.flags   = flags;
            mr.values.args[0] = op1Value;
            mr.values.args[1] = op2Value;
            mr.values.args[2] = input;
        }
    };

    class Match_assert2 : public Matcher
    {
    public:

        Match_assert2();

        void make_match_result( const std::smatch& what, Match_result& mr ) const override
        {
            const bool invert = what[1].matched && what[1].length() == 1;
            unsigned int op1Value;
            unsigned int flags = BLUE_VALUE_ARG1;
            match( op1Value, flags, BLUE_VALUE_ARG0, 3, what );

            std::uint16_t input = static_cast<uint8_t>( '!' );
            input |= std::uint16_t( static_cast<uint8_t>( '=' ) ) << 8;

            if( invert )
            {
                input |= static_cast<uint8_t>( '=' );
            }

            mr.functionName[0] = 'a';
            mr.functionName[1] = 's';
            mr.functionName[2] = 's';
            mr.functionName[3] = 'e';
            mr.functionName[4] = 'r';
            mr.functionName[5] = 't';
            mr.functionName[6] = 0;
            mr.values.flags    = flags;
            mr.values.args[0]  = op1Value;
            mr.values.args[1]  = 0;
            mr.values.args[2]  = input;
        }
    };

    class Match_print_text : public Matcher
    {
    public:

        Match_print_text();

        void make_match_result( const std::smatch& what, Match_result& mr ) const override
        {
            union
            {
                std::uint16_t        args_[3];
                char            chars_[6];
            } args_and_chars;

            // The regex means this assert should never be false: it wont match.
            assert( what[1].length() <= BLUE_INSTRUCTION_TEXT_MAX_LENGTH );

            args_and_chars.args_[0] = args_and_chars.args_[1] = args_and_chars.args_[2] = 0;

            for( int i = 0; i < std::min<int>(6, what[1].length()); ++i )
            {
                args_and_chars.chars_[i] = what[1].first[i];
            }

            mr.functionName[0] = 'p';
            mr.functionName[1] = 'r';
            mr.functionName[2] = 'i';
            mr.functionName[3] = 'n';
            mr.functionName[4] = 't';
            mr.functionName[5] = 0;
            mr.values.args[0]  = args_and_chars.args_[0];
            mr.values.args[1]  = args_and_chars.args_[1];
            mr.values.args[2]  = args_and_chars.args_[2];
        }
    };

    class Match_assign : public Matcher
    {
    public:

        Match_assign();

        void make_match_result( const std::smatch& what, Match_result& mr ) const override
        {
            unsigned int from  = 0u;
            unsigned int to    = 0u;
            unsigned int flags = 0u;
            match( to,   flags, BLUE_VALUE_ARG1, 2u, what );
            match( from, flags, BLUE_VALUE_ARG0, 5u, what );
            mr.values.flags = flags;
            mr.values.args[0] = from;
            mr.values.args[1] = to;
        }
    };

    class Match_asm : public Matcher
    {
    public:

        Match_asm();

        void make_match_result( const std::smatch& what, Match_result& mr ) const override
        {
            assert( what[1].matched && what[2].matched && what[3].matched && what[4].matched );

            // Match against base 16 (hex).
            std::uint16_t fxxx = static_cast<std::uint16_t>( strtoul( static_cast<const char*>( &*what[1].first ), nullptr, 16 ) );
            std::uint16_t aaaa = static_cast<std::uint16_t>( strtoul( static_cast<const char*>( &*what[2].first ), nullptr, 16 ) );
            std::uint16_t bbbb = static_cast<std::uint16_t>( strtoul( static_cast<const char*>( &*what[3].first ), nullptr, 16 ) );
            std::uint16_t cccc = static_cast<std::uint16_t>( strtoul( static_cast<const char*>( &*what[4].first ), nullptr, 16 ) );

            mr.values.flags    = fxxx;
            mr.values.args[0]  = aaaa;
            mr.values.args[1]  = bbbb;
            mr.values.args[2]  = cccc;
        }
    };

    class Match_goto : public Matcher
    {
    public:

        Match_goto();

        void make_match_result( const std::smatch& what, Match_result& mr ) const override
        {
            unsigned int whichLine = 0u;
            unsigned int flags     = 0u;
            match( whichLine, flags, BLUE_VALUE_ARG0, 2u, what );
            mr.functionName[0] = 'g';
            mr.functionName[1] = 'o';
            mr.functionName[2] = 't';
            mr.functionName[3] = 'o';
            mr.functionName[4] = 0;
            mr.values.flags    = flags;
            mr.values.args[0]  = whichLine;
        }
    };

    /// function()
    class Match_0_0 : public Matcher
    {
    public:

        Match_0_0();

        void make_match_result( const std::smatch& what, Match_result& mr ) const override
        {
            assert( what[1].matched );
            if( what[1].length() > 0 && what[1].length() <= MAX_FUNCTION_NAME_LENGTH )
            {
                mr.copy_function_name( &*what[1].first, what[1].length() );
            }
        }
    };

    /// function( X )
    class Match_0_1 : public Matcher
    {
    public:

        Match_0_1();

        void make_match_result( const std::smatch& what, Match_result& mr ) const override
        {
            assert( what[1].matched );
            if( what[1].length() > 0 && what[1].length() <= MAX_FUNCTION_NAME_LENGTH )
            {
                unsigned int param = 0u;
                unsigned int flags = 0u;
                match( param, flags, BLUE_VALUE_ARG0, 3u, what );
                mr.copy_function_name( &*what[1].first, what[1].length() );
                // We set the by-value on arg1 here to make "turn" work correctly.
                mr.values.flags = flags | BLUE_VALUE_ARG1;
                mr.values.args[0] = param;
            }
        }
    };

    /// function( [X] )
    class Match_0_1i : public Matcher
    {
    public:

        Match_0_1i();

        void make_match_result( const std::smatch& what, Match_result& mr ) const override
        {
            assert( what[1].matched );
            if( what[1].length() > 0 && what[1].length() <= MAX_FUNCTION_NAME_LENGTH )
            {
                unsigned int param = 0u;
                unsigned int flags = 0u;
                match( param, flags, BLUE_VALUE_ARG0, 2, what );
                mr.copy_function_name( &*what[1].first, what[1].length() );
                mr.values.args[0] = param;
            }
        }
    };

    /// function( X, Y )
    class Match_0_2 : public Matcher
    {
    public:

        Match_0_2();

        void make_match_result( const std::smatch& what, Match_result& mr ) const override
        {
            assert( what[1].matched );
            if( what[1].length() > 0 && what[1].length() <= MAX_FUNCTION_NAME_LENGTH )
            {
                unsigned int params[3];
                unsigned int flags = 0;

                match( params[0], flags, BLUE_VALUE_ARG0, 3, what );
                match( params[1], flags, BLUE_VALUE_ARG1, 6, what );

                mr.copy_function_name( &*what[1].first, what[1].length() );
                mr.values.flags   = flags;
                mr.values.args[0] = params[0];
                mr.values.args[1] = params[1];
            }
        }
    };

    /// function( [X], [Y] )
    class Match_0_2i : public Matcher
    {
    public:

        Match_0_2i();

        void make_match_result( const std::smatch& what, Match_result& mr ) const override
        {
            assert( what[1].matched );
            if( what[1].length() > 0 && what[1].length() <= MAX_FUNCTION_NAME_LENGTH )
            {
                unsigned int params[3];
                unsigned int flags = 0;

                match( params[0], flags, BLUE_VALUE_ARG0, 3, what );
                match( params[1], flags, BLUE_VALUE_ARG1, 6, what );

                mr.copy_function_name( &*what[1].first, what[1].length() );
                mr.values.flags   = flags;
                mr.values.args[0] = params[0];
                mr.values.args[1] = params[1];
            }
        }
    };

    /// function( X, Y, Z )
    class Match_0_3 : public Matcher
    {
    public:

        Match_0_3();

        void make_match_result( const std::smatch& what, Match_result& mr ) const override
        {
            assert( what[1].matched );
            if( what[1].length() > 0 && what[1].length() <= MAX_FUNCTION_NAME_LENGTH )
            {
                unsigned int params[3];
                unsigned int flags = 0;

                match( params[0], flags, BLUE_VALUE_ARG0, 3, what );
                match( params[1], flags, BLUE_VALUE_ARG1, 6, what );
                match( params[2], flags, BLUE_VALUE_ARG2, 9, what );

                mr.copy_function_name( &*what[1].first, what[1].length() );
                mr.values.flags   = flags;
                mr.values.args[0] = params[0];
                mr.values.args[1] = params[1];
                mr.values.args[2] = params[2];
            }
        }
    };

    /// function( [X], Y, Z )
    class Match_0_1i2 : public Matcher
    {
    public:

        Match_0_1i2();

        void make_match_result( const std::smatch& what, Match_result& mr ) const override
        {
            assert( what[1].matched );
            if( what[1].length() > 0 && what[1].length() <= MAX_FUNCTION_NAME_LENGTH )
            {
                unsigned int params[3];
                unsigned int flags = 0;

                match( params[0], flags, 0,               2, what );
                match( params[1], flags, BLUE_VALUE_ARG1, 3, what );
                match( params[2], flags, BLUE_VALUE_ARG2, 7, what );

                mr.copy_function_name( &*what[1].first, what[1].length() );
                mr.values.flags   = flags;
                mr.values.args[0] = params[0];
                mr.values.args[1] = params[1];
                mr.values.args[2] = params[2];
            }
        }
    };

    /// X = function()
    class Match_1_0 : public Matcher
    {
    public:

        Match_1_0();

        void make_match_result( const std::smatch& what, Match_result& mr ) const override
        {
            if( what[4].length() > 0 && what[4].length() <= MAX_FUNCTION_NAME_LENGTH )
            {
                unsigned int result = 0u;
                unsigned int flags  = 0u;
                match( result, flags, BLUE_VALUE_ARG0, 2u, what );

                mr.copy_function_name( &*what[4].first, what[4].length() );
                mr.values.flags   = flags;
                mr.values.args[0] = result;
            }
        }
    };

    /// X = function( Y )
    /// X = arg0
    /// Y = arg1
    class Match_1_1 : public Matcher
    {
    public:

        Match_1_1();

        void make_match_result( const std::smatch& what, Match_result& mr ) const override
        {
            assert( what[4].matched );
            if( what[4].length() > 0 && what[4].length() <= MAX_FUNCTION_NAME_LENGTH )
            {
                unsigned int arg1  = 0u;;
                unsigned int arg0  = 0u;;
                unsigned int flags = 0u;
                match( arg0, flags, BLUE_VALUE_ARG0, 2u, what );
                match( arg1, flags, BLUE_VALUE_ARG1, 6u, what );

                mr.copy_function_name( &*what[4].first, what[4].length() );
                mr.values.flags   = flags;
                mr.values.args[0] = arg0;
                mr.values.args[1] = arg1;
            }
        }
    };

    /// X = function( [Y] )
    /// X = arg0, Y = arg1. Y is always captures as an indirection.
    class Match_1_1i : public Matcher
    {
    public:

        Match_1_1i();

        void make_match_result( const std::smatch& what, Match_result& mr ) const override
        {
            assert( what[4].matched );
            if( what[4].length() > 0 && what[4].length() <= MAX_FUNCTION_NAME_LENGTH )
            {
                unsigned int arg1  = 0u;
                unsigned int arg0  = 0u;
                unsigned int flags = 0u;
                match( arg0, flags, BLUE_VALUE_ARG0, 2u, what );

                // Pass in zero here - the regex ensures we always match by
                // indirection. Values are impossible here.
                match( arg1, flags, 0, 5u, what );

                mr.copy_function_name( &*what[4].first, what[4].length() );
                mr.values.flags   = flags;
                mr.values.args[0] = arg0;
                mr.values.args[1] = arg1;
            }
        }
    };

    /// X = function( Y, Z )
    class Match_1_2 : public Matcher
    {
    public:

        Match_1_2();

        void make_match_result( const std::smatch& what, Match_result& mr ) const override
        {
            assert( what[4].matched );
            if( what[4].length() > 0 && what[4].length() <= MAX_FUNCTION_NAME_LENGTH )
            {
                unsigned int arg0  = 0u;
                unsigned int arg1  = 0u;
                unsigned int arg2  = 0u;
                unsigned int flags = 0u;
                match( arg0, flags, BLUE_VALUE_ARG0, 2, what );
                match( arg1, flags, BLUE_VALUE_ARG1, 5, what );
                match( arg2, flags, BLUE_VALUE_ARG2, 8, what );

                mr.copy_function_name( &*what[4].first, what[4].length() );
                mr.values.flags   = flags;
                mr.values.args[0] = arg0;
                mr.values.args[1] = arg1;
                mr.values.args[2] = arg2;
            }
        }
    };

    /// X = function( [Y], [Z] )
    class Match_1_2i : public Matcher
    {
    public:

        Match_1_2i();

        void make_match_result( const std::smatch& what, Match_result& mr ) const override
        {
            assert( what[4].matched );
            if( what[4].length() > 0 && what[4].length() <= MAX_FUNCTION_NAME_LENGTH )
            {
                unsigned int arg0  = 0u;
                unsigned int arg1  = 0u;
                unsigned int arg2  = 0u;
                unsigned int flags = 0u;
                match( arg0, flags, BLUE_VALUE_ARG0, 2u, what );

                // Pass in zero here - the regex ensures we always match by
                // indirection. Values are impossible here.
                match( arg1, flags, 0, 5u, what );
                match( arg2, flags, 0, 6u, what );

                mr.copy_function_name( &*what[4].first, what[4].length() );
                mr.values.flags   = flags;
                mr.values.args[0] = arg0;
                mr.values.args[1] = arg1;
                mr.values.args[2] = arg2;
            }
        }
    };

    /// function( X, [Y] )
    class Match_0_1_1i : public Matcher
    {
    public:

        Match_0_1_1i();

        void make_match_result( const std::smatch& what, Match_result& mr ) const override
        {
            assert( what[1].matched );
            if( what[1].length() > 0 && what[1].length() <= MAX_FUNCTION_NAME_LENGTH )
            {
                unsigned int params[3];
                unsigned int flags = 0;

                match( params[0], flags, BLUE_VALUE_ARG0, 2, what );
                match( params[1], flags, 0, 5, what );

                mr.copy_function_name( &*what[1].first, what[1].length() );
                mr.values.flags   = flags;
                mr.values.args[0] = params[0];
                mr.values.args[1] = params[1];
            }
        }
    };

    //-------------------------------------------------------------------------

    Match_value::Match_value() : Matcher( g_regValue, 2 )
    {
        add_syntax( Syntax( "",                           nullptr,                                    BLUE_ASM_NOP,                   Value0 ) );
    }

    Match_print_text::Match_print_text() : Matcher( g_regPrintText, 2 )
    {
        add_syntax( Syntax( "print",                      "print( \"$TEXT\" )",                       BLUE_ASM_PRINT_TXT,             Value0, Value1, Value2 ) );
    }

    Match_comment_line::Match_comment_line() : Matcher( g_regCommentLine, 0 )
    {
        add_syntax( Syntax( "",                           nullptr,                                    BLUE_ASM_NOP ) );
    }

    Match_assign::Match_assign() : Matcher( g_regAssign, 7 )
    {
        add_syntax( Syntax( "",                           "[$A1] = $A0",                              BLUE_ASM_COPY,                  Value0, Value1 ) );
    }

    Match_asm::Match_asm() : Matcher( g_regAsm, 5 )
    {
        add_syntax( Syntax( "",                           nullptr,                                    BLUE_ASM_NOP,                   Value0, Value1, Value2 ) );
    }

    Match_assert1::Match_assert1() : Matcher( g_regAssert1, 7 )
    {
        add_syntax( Syntax( "assert",                     "assert( $A0 $OP2 $A1 )",                   BLUE_ASM_ASSERT,                Value0, Value1, Value2 ) );
    }

    Match_assert2::Match_assert2() : Matcher( g_regAssert2, 4 )
    {
        add_syntax( Syntax( "assert",                     "assert( $A0 $OP2 $A1 )",                   BLUE_ASM_ASSERT,                Value0, Value1, Value2 ) );
    }

    Match_ifgoto_1op::Match_ifgoto_1op() : Matcher( g_reg1OpIfGoto, 7 )
    {
        add_syntax( Syntax( "goto",                       "goto $A1",                                 BLUE_ASM_JMP,                   Value0, Value1, Value2 ) );
        add_syntax( Syntax( "",                           nullptr,                                    BLUE_ASM_NOP ) );
        add_syntax( Syntax( "if",                         "if $A0 != $A1 goto $A2",                   BLUE_ASM_JMP_NEQ,               Value0, Value1, Value2 ) );
        add_syntax( Syntax( "if !",                       "if $A0 == $A1 goto $A2",                   BLUE_ASM_JMP_EQ,                Value0, Value1, Value2 ) );
    }

    Match_ifgoto_2op::Match_ifgoto_2op() : Matcher( g_reg2OpIfGoto, 0 )
    {
        add_syntax( Syntax( "if <",                       "if $A0 < $A1 goto $A2",                    BLUE_ASM_JMP_LT,                Value0, Value1, Value2 ) );
        add_syntax( Syntax( "if >",                       "if $A0 > $A1 goto $A2",                    BLUE_ASM_JMP_GT,                Value0, Value1, Value2 ) );
        add_syntax( Syntax( "if >=",                      "if $A0 >= $A1 goto $A2",                   BLUE_ASM_JMP_LTE,               Value0, Value1, Value2 ) );
        add_syntax( Syntax( "if <=",                      "if $A0 <= $A1 goto $A2",                   BLUE_ASM_JMP_GTE,               Value0, Value1, Value2 ) );
        add_syntax( Syntax( "if ==",                      "if $A0 == $A1 goto $A2",                   BLUE_ASM_JMP_EQ,                Value0, Value1, Value2 ) );
        add_syntax( Syntax( "if !=",                      "if $A0 != $A1 goto $A2",                   BLUE_ASM_JMP_NEQ,               Value0, Value1, Value2 ) );
    }

    Match_goto::Match_goto() : Matcher( g_regGoto, 2 )
    {
        add_syntax( Syntax( "goto",                       "goto $A0",                                 BLUE_ASM_JMP,                   Value0 ) );
    }

    Match_math_op1::Match_math_op1() : Matcher( g_regMathOp1, 6 )
    {
        add_syntax( Syntax( "+",                          "[$A0] = $A1 + $A2",                        BLUE_ASM_ADD,                   Value0, Value1, Value2 ) );
        add_syntax( Syntax( "-",                          "[$A0] = $A1 - $A2",                        BLUE_ASM_SUB,                   Value0, Value1, Value2 ) );
        add_syntax( Syntax( "*",                          "[$A0] = $A1 * $A2",                        BLUE_ASM_MUL,                   Value0, Value1, Value2 ) );
        add_syntax( Syntax( "/",                          "[$A0] = $A1 / $A2",                        BLUE_ASM_DIV,                   Value0, Value1, Value2 ) );
        add_syntax( Syntax( "%",                          "[$A0] = $A1 % $A2",                        BLUE_ASM_MODULO,                Value0, Value1, Value2 ) );
        add_syntax( Syntax( "|",                          "[$A0] = $A1 | $A2",                        BLUE_ASM_OR,                    Value0, Value1, Value2 ) );
        add_syntax( Syntax( "&",                          "[$A0] = $A1 & $A2",                        BLUE_ASM_AND,                   Value0, Value1, Value2 ) );
        add_syntax( Syntax( "^",                          "[$A0] = $A1 ^ $A2",                        BLUE_ASM_XOR,                   Value0, Value1, Value2 ) );
    }

    Match_math_op2::Match_math_op2() : Matcher( g_regMathOp2, 11 )
    {
        add_syntax( Syntax( "+",                          "[$A0] = $A1 + $A2",                        BLUE_ASM_ADD,                   Value0, Value1, Value2 ) );
        add_syntax( Syntax( "-",                          "[$A0] = $A1 - $A2",                        BLUE_ASM_SUB,                   Value0, Value1, Value2 ) );
        add_syntax( Syntax( "*",                          "[$A0] = $A1 * $A2",                        BLUE_ASM_MUL,                   Value0, Value1, Value2 ) );
        add_syntax( Syntax( "/",                          "[$A0] = $A1 / $A2",                        BLUE_ASM_DIV,                   Value0, Value1, Value2 ) );
        add_syntax( Syntax( "%",                          "[$A0] = $A1 % $A2",                        BLUE_ASM_MODULO,                Value0, Value1, Value2 ) );
        add_syntax( Syntax( "|",                          "[$A0] = $A1 | $A2",                        BLUE_ASM_OR,                    Value0, Value1, Value2 ) );
        add_syntax( Syntax( "&",                          "[$A0] = $A1 & $A2",                        BLUE_ASM_AND,                   Value0, Value1, Value2 ) );
        add_syntax( Syntax( "^",                          "[$A0] = $A1 ^ $A2",                        BLUE_ASM_XOR,                   Value0, Value1, Value2 ) );
        add_syntax( Syntax( ">",                          "[$A0] = $A1 > $A2",                        BLUE_ASM_BOOL_GT,               Value0, Value1, Value2 ) );
        add_syntax( Syntax( "<",                          "[$A0] = $A1 < $A2",                        BLUE_ASM_BOOL_LT,               Value0, Value1, Value2 ) );
        add_syntax( Syntax( ">=",                         "[$A0] = $A1 >= $A2",                       BLUE_ASM_BOOL_GTE,              Value0, Value1, Value2 ) );
        add_syntax( Syntax( "<=",                         "[$A0] = $A1 <= $A2",                       BLUE_ASM_BOOL_LTE,              Value0, Value1, Value2 ) );
        add_syntax( Syntax( ">>",                         "[$A0] = $A1 >> $A2",                       BLUE_ASM_RIGHT_SHIFT,           Value0, Value1, Value2 ) );
        add_syntax( Syntax( "<<",                         "[$A0] = $A1 << $A2",                       BLUE_ASM_LEFT_SHIFT,            Value0, Value1, Value2 ) );
        add_syntax( Syntax( "||",                         "[$A0] = $A1 || $A2",                       BLUE_ASM_BOOL_OR,               Value0, Value1, Value2 ) );
        add_syntax( Syntax( "&&",                         "[$A0] = $A1 && $A2",                       BLUE_ASM_BOOL_AND,              Value0, Value1, Value2 ) );
        add_syntax( Syntax( "==",                         "[$A0] = $A1 == $A2",                       BLUE_ASM_EQ,                    Value0, Value1, Value2 ) );
        add_syntax( Syntax( "!=",                         "[$A0] = $A1 != $A2",                       BLUE_ASM_NEQ,                   Value0, Value1, Value2 ) );
    }

    Match_0_0::Match_0_0() : Matcher( g_regFn_0_0, 2 )
    {
        add_syntax( Syntax( "clear_msgs",                 "clear_msgs()",                             BLUE_ASM_MSG_CLEAR_MSGS ) );
        add_syntax( Syntax( "sense",                      "sense()",                                  BLUE_ASM_SENSE_MAX ) );
        add_syntax( Syntax( "cancel_sales",               "cancel_sales()",                           BLUE_ASM_CANCEL_SALES ) );
    }

    Match_0_1::Match_0_1() : Matcher( g_regFn_0_1, 3 )
    {
        add_syntax( Syntax( "advance",                    "advance( $A0 )",                           BLUE_ASM_ADVANCE,               Value0 ) );
        add_syntax( Syntax( "print",                      "print( $A0 )",                             BLUE_ASM_PRINT_VAL,             Value0 ) );
        add_syntax( Syntax( "sense",                      "sense( $A0 )",                             BLUE_ASM_SENSE,                 Value0 ) );
        add_syntax( Syntax( "synch",                      "synch( $A0 )",                             BLUE_ASM_SYNCH,                 Value0 ) );
        add_syntax( Syntax( "selfdestruct",               "selfdestruct( $A0 )",                      BLUE_ASM_SELF_DESTRUCT,         Value0 ) );
        add_syntax( Syntax( "turn_cw",                    "turn( $A1, $A0 )",                         BLUE_ASM_TURN,                  Value0, ValueOne ) );
        add_syntax( Syntax( "turn_ccw",                   "turn( $A1, $A0 )",                         BLUE_ASM_TURN,                  Value0, ValueZero ) );
        add_syntax( Syntax( "move_msg_to_msgbuf",         "move_msg_to_msgbuf( $A0 )",                BLUE_ASM_MOVE_MSG_MSGBUF,       Value0 ) );
        add_syntax( Syntax( "lock_cargohold",             "lock_cargohold( $A0 )",                    BLUE_ASM_LOCK_CARGO,            Value0 ) );
        add_syntax( Syntax( "set_error_code",             "set_error_code( $A0 )",                    BLUE_ASM_SET_ERROR_CODE,        Value0 ) );
    }

    /// These commands require a location as a param.
    Match_0_1i::Match_0_1i() : Matcher( g_regFn_0_1i, 3 )
    {
        add_syntax( Syntax( "navigate_to",                "navigate_to( $A0 )",                       BLUE_ASM_NAVIGATE_TO,           Value0 ) );
        add_syntax( Syntax( "aim_at",                     "aim_at( $A0 )",                            BLUE_ASM_AIM_AT,                Value0 ) );
        add_syntax( Syntax( "fire_at",                    "fire_at( $A0 )",                           BLUE_ASM_FIRE_AT,               Value0 ) );
    }

    Match_0_2::Match_0_2() : Matcher( g_regFn_0_2, 8 )
    {
        add_syntax( Syntax( "turn",                       "turn( $A1, $A0 )",                         BLUE_ASM_TURN,                  Value1, Value0 ) );
        add_syntax( Syntax( "store_and_goto",             "store_and_goto( $A0, $A1 )",               BLUE_ASM_STORE_JMP,             Value0, Value1 ) );
        add_syntax( Syntax( "refine",                     "refine( $A0, $A1 )",                       BLUE_ASM_REFINE,                Value0, Value1 ) );
        add_syntax( Syntax( "set_navigation_weighting",   "set_navigation_weighting( $A0, $A1 )",     BLUE_ASM_SET_ASTAR_WEIGHT,      Value0, Value1 ) );
        add_syntax( Syntax( "try_buy",                    "try_buy( $A0, $A1 )",                      BLUE_ASM_TRY_BUY,               Value0, Value1 ) );
    }

    /// These commands require 2 locations as params.
    Match_0_2i::Match_0_2i() : Matcher( g_regFn_0_2i, 8 )
    {
        add_syntax( Syntax( "plot_route_between",         "plot_route_between( $A0, $A1 )",           BLUE_ASM_PLOT_ROUTE_BETWEEN,    Value0, Value1 ) );
    }

    Match_0_3::Match_0_3() : Matcher( g_regFn_0_3, 9 )
    {
        add_syntax( Syntax( "build",                      "build( $A0, $A1, $A2 )",                   BLUE_ASM_BUILD,                 Value0, Value1, Value2 ) );
        add_syntax( Syntax( "broadcast",                  "broadcast( $A0, $A1, $A2 )",               BLUE_ASM_BROADCAST,             Value0, Value1, Value2 ) );
        add_syntax( Syntax( "copy_code_from_msgbuf",      "copy_code_from_msgbuf( $A0, $A1, $A2 )",   BLUE_ASM_COPY_MSGBUF_CODE,      Value0, Value1, Value2 ) );
        add_syntax( Syntax( "copy_code",                  "copy_code( $A0, $A1, $A2 )",               BLUE_ASM_COPY_CODE,             Value0, Value1, Value2 ) );
        add_syntax( Syntax( "swap_code",                  "swap_code( $A0, $A1, $A2 )",               BLUE_ASM_SWAP_CODE,             Value0, Value1, Value2 ) );
        add_syntax( Syntax( "copy_route",                 "copy_route( $A0, $A1, $A2 )",              BLUE_ASM_COPY_ROUTE,            Value0, Value1, Value2 ) );
        add_syntax( Syntax( "for_sale",                   "for_sale( $A0, $A1, $A2 )",                BLUE_ASM_FOR_SALE,              Value0, Value1, Value2 ) );
    }

    /// These commands require a location as a param.
    Match_0_1i2::Match_0_1i2() : Matcher( g_regFn_0_1i2, 9 )
    {
        add_syntax( Syntax( "transmit",                   "transmit( $A0, $A1, $A2 )",                BLUE_ASM_TRANSMIT,              Value0, Value1, Value2 ) );
        add_syntax( Syntax( "give_cargo_to",              "give_cargo_to( $A0, $A1, $A2 )",           BLUE_ASM_GIVE_CARGO_TO,         Value0, Value1, Value2 ) );
        add_syntax( Syntax( "take_cargo_from",            "take_cargo_from( $A0, $A1, $A2 )",         BLUE_ASM_TAKE_CARGO_FROM,       Value0, Value1, Value2 ) );
    }

    Match_1_0::Match_1_0() : Matcher( g_regFn_1_0, 5 )
    {
        add_syntax( Syntax( "get_line_number",            "[$A0] = get_line_number()",                BLUE_ASM_LINENUMBER,            Value0 ) );
        add_syntax( Syntax( "get_line_count",             "[$A0] = get_line_count()",                 BLUE_ASM_LINE_COUNT,            Value0 ) );
        add_syntax( Syntax( "get_msgbuf_line_count",      "[$A0] = get_msgbuf_line_count()",          BLUE_ASM_MSGBUF_LINE_COUNT,     Value0 ) );
        add_syntax( Syntax( "get_cap",                    "[$A0] = get_cap()",                        BLUE_ASM_CAP,                   Value0 ) );
        add_syntax( Syntax( "get_max_cap",                "[$A0] = get_max_cap()",                    BLUE_ASM_MAX_CAP,               Value0 ) );
        add_syntax( Syntax( "get_max_msgs",               "[$A0] = get_max_msgs()",                   BLUE_ASM_GET_MAX_MSGS,          Value0 ) );
        add_syntax( Syntax( "get_msgs_count",             "[$A0] = get_msgs_count()",                 BLUE_ASM_GET_MSGS_COUNT,        Value0 ) );
        add_syntax( Syntax( "get_sense_range",            "[$A0] = get_sense_range()",                BLUE_ASM_MAX_SENSE_RANGE,       Value0 ) );
        add_syntax( Syntax( "get_transmit_range",         "[$A0] = get_transmit_range()",             BLUE_ASM_MAX_TRANSMIT_RANGE,    Value0 ) );
        add_syntax( Syntax( "get_broadcast_range",        "[$A0] = get_broadcast_range()",            BLUE_ASM_MAX_BROADCAST_RANGE,   Value0 ) );
        add_syntax( Syntax( "get_blueprint_count",        "[$A0] = get_blueprint_count()",            BLUE_ASM_GET_BLUEPRINT_COUNT,   Value0 ) );
        add_syntax( Syntax( "get_sense_result_count",     "[$A0] = get_sense_result_count()",         BLUE_ASM_SENSE_RESULT_COUNT,    Value0 ) );
        add_syntax( Syntax( "get_location",               "[$A0] = get_location()",                   BLUE_ASM_GET_LOCATION,          Value0 ) );
        add_syntax( Syntax( "get_random",                 "[$A0] = get_random()",                     BLUE_ASM_RAND,                  Value0 ) );
        add_syntax( Syntax( "get_cargohold_freespace",    "[$A0] = get_cargohold_freespace()",        BLUE_ASM_CARGOHOLD_FREE_SPACE,  Value0 ) );
        add_syntax( Syntax( "get_cargohold_spaceused",    "[$A0] = get_cargohold_spaceused()",        BLUE_ASM_CARGOHOLD_SPACE_USED,  Value0 ) );
        add_syntax( Syntax( "get_cargohold_volume",       "[$A0] = get_cargohold_volume()",           BLUE_ASM_CARGOHOLD_VOLUME,      Value0 ) );
        add_syntax( Syntax( "is_cargohold_locked",        "[$A0] = is_cargohold_locked()",            BLUE_ASM_IS_CARGO_LOCKED,       Value0 ) );
        add_syntax( Syntax( "get_route_length",           "[$A0] = get_route_length()",               BLUE_ASM_GET_ROUTE_LENGTH,      Value0 ) );
        add_syntax( Syntax( "get_error_code",             "[$A0] = get_error_code()",                 BLUE_ASM_GET_ERROR_CODE,        Value0 ) );
        add_syntax( Syntax( "get_credits_balance",        "[$A0] = get_credits_balance()",            BLUE_ASM_GET_CREDITS_BALANCE,   Value0 ) );
    }

    Match_1_1::Match_1_1() : Matcher( g_regFn_1_1, 8 )
    {
        add_syntax( Syntax( "abs",                        "[$A0] = abs( $A1 )",                       BLUE_ASM_ABS,                   Value0, Value1 ) );
        add_syntax( Syntax( "advance",                    "[$A0] = advance( $A1 )",                   BLUE_ASM_ADVANCE_WITH_STORE,    Value1, Value0 ) );
        add_syntax( Syntax( "can_build",                  "[$A0] = can_build( $A1 )",                 BLUE_ASM_CAN_BUILD,             Value0, Value1 ) );
        add_syntax( Syntax( "get_sense_result_type",      "[$A0] = get_sense_result_type( $A1 )",     BLUE_ASM_SENSE_RESULT_TYPE,     Value0, Value1 ) );
        add_syntax( Syntax( "get_sense_result_location",  "[$A0] = get_sense_result_location( $A1 )", BLUE_ASM_SENSE_RESULT_LOCATION, Value0, Value1 ) );
        add_syntax( Syntax( "get_cargohold_count_of",     "[$A0] = get_cargohold_count_of( $A1 )",    BLUE_ASM_CARGOHOLD_UNITS_OF,    Value0, Value1 ) );
        add_syntax( Syntax( "get_cargohold_volume_of",    "[$A0] = get_cargohold_volume_of( $A1 )",   BLUE_ASM_CARGOHOLD_VOL_OF,      Value0, Value1 ) );
        add_syntax( Syntax( "get_navigation_weighting",   "[$A0] = get_navigation_weighting( $A1 )",  BLUE_ASM_GET_ASTAR_WEIGHT,      Value0, Value1 ) );
        add_syntax( Syntax( "get_refine_period",          "[$A0] = get_refine_period( $A1)",          BLUE_ASM_GET_REFINE_PERIOD,     Value0, Value1 ) );
        add_syntax( Syntax( "is_location",                "[$A0] = is_location( $A1 )",               BLUE_ASM_IS_LOCATION,           Value0, Value1 ) );
        add_syntax( Syntax( "get_line_number_plus",       "[$A0] = get_line_number_plus( $A1 )",      BLUE_ASM_GET_LINENUMBER_PLUS,   Value0, Value1 ) );
    }

    /// These commands require a location as a param.
    Match_1_1i::Match_1_1i() : Matcher( g_regFn_1_1i, 6 )
    {
        add_syntax( Syntax( "get_distance_to",            "[$A0] = get_distance_to( $A1 )",           BLUE_ASM_DISTANCE_TO,           Value0, Value1 ) );
        add_syntax( Syntax( "get_cw_turn_count_to",       "[$A0] = get_cw_turn_count_to( $A1 )",      BLUE_ASM_CW_TURN_COUNT_TO,      Value0, Value1 ) );
        add_syntax( Syntax( "can_fire_at",                "[$A0] = can_fire_at( $A1 )",               BLUE_ASM_CAN_FIRE_AT,           Value0, Value1 ) );
        add_syntax( Syntax( "can_aim_at",                 "[$A0] = can_aim_at( $A1 )",                BLUE_ASM_CAN_AIM_AT,            Value0, Value1 ) );
        add_syntax( Syntax( "has_line_of_fire_to",        "[$A0] = has_line_of_fire_to( $A1 )",       BLUE_ASM_HAS_LINE_OF_FIRE_TO,   Value0, Value1 ) );
    }

    Match_1_2::Match_1_2() : Matcher( g_regFn_1_2, 11 )
    {
        add_syntax( Syntax( "get_random",                 "[$A0] = get_random( $A1, $A2 )",           BLUE_ASM_RAND_RANGE,            Value0, Value1, Value2 ) );
        add_syntax( Syntax( "compare_line",               "[$A0] = compare_line( $A1, $A2 )",         BLUE_ASM_COMPARE_CODE_LINE,     Value0, Value1, Value2 ) );
        add_syntax( Syntax( "try_buy",                    "[$A0] = try_buy( $A1, $A2 )",              BLUE_ASM_TRY_BUY_WITH_STORE,    Value0, Value1, Value2 ) );
    }

    /// These commands require locations as params.
    Match_1_2i::Match_1_2i() : Matcher( g_regFn_1_2i, 7 )
    {
        add_syntax( Syntax( "get_distance_between",       "[$A0] = get_distance_between( $A1, $A2 )", BLUE_ASM_DISTANCE_BETWEEN,      Value0, Value1, Value2 ) );
    }

    /// These commands require a location as 2nd param.
    Match_0_1_1i::Match_0_1_1i() : Matcher( g_regFn_0_1_1i, 6 )
    {
        add_syntax( Syntax( "navigate_n_towards",         "navigate_n_towards( $A0, $A1 )",           BLUE_ASM_NAVIGATE_N_TOWARDS,    Value0, Value1 ) );
        add_syntax( Syntax( "transfer_credits_to",        "transfer_credits_to( $A0, $A1 )",          BLUE_ASM_TRANSFER_CREDITS_TO,   Value0, Value1 ) );
    }
}

namespace blue
{
    //-------------------------------------------------------------------------

    /// Contains matchers which can compile and decompile.
    /// Can be queried for lists of commands.
    class Hidden_compiler
    {
    public:

        Hidden_compiler();

        Compilation_result compile( const std::string_view& line ) const;

        std::string decompile( const Instruction& x ) const;

        std::vector<const char*> predict_text( const char* text ) const;

        std::set<std::uint16_t> all_opcodes() const;

        static const Hidden_compiler* get();

    private:

        std::vector<std::unique_ptr<Matcher>>  matchers_;

        static std::unique_ptr<Hidden_compiler> compiler_;
    };

    //--------------------------------------------------------------------------

    // Static 'global' singleton instance of the hidden compiler. This is
    // an expensive and immutable object.
    std::unique_ptr<Hidden_compiler> Hidden_compiler::compiler_;

    //--------------------------------------------------------------------------

    const Hidden_compiler* Hidden_compiler::get()
    {
        if( Hidden_compiler::compiler_ )
        {
            return Hidden_compiler::compiler_.get();
        }
        else
        {
            Hidden_compiler::compiler_ = std::make_unique<Hidden_compiler>();
            return Hidden_compiler::get();
        }
    }

    Hidden_compiler::Hidden_compiler()
    {
        matchers_.reserve( BLUE_REGEX_COUNT );

        matchers_.emplace_back( std::make_unique<Match_goto>() );
        matchers_.emplace_back( std::make_unique<Match_assign>() );
        matchers_.emplace_back( std::make_unique<Match_value>() );
        matchers_.emplace_back( std::make_unique<Match_0_0>() );
        matchers_.emplace_back( std::make_unique<Match_0_1>() );
        matchers_.emplace_back( std::make_unique<Match_0_1i>() );
        matchers_.emplace_back( std::make_unique<Match_0_2>() );
        matchers_.emplace_back( std::make_unique<Match_0_2i>() );
        matchers_.emplace_back( std::make_unique<Match_0_3>() );
        matchers_.emplace_back( std::make_unique<Match_0_1i2>() );
        matchers_.emplace_back( std::make_unique<Match_1_0>() );
        matchers_.emplace_back( std::make_unique<Match_1_1>() );
        matchers_.emplace_back( std::make_unique<Match_1_1i>() );
        matchers_.emplace_back( std::make_unique<Match_1_2>() );
        matchers_.emplace_back( std::make_unique<Match_1_2i>() );
        matchers_.emplace_back( std::make_unique<Match_ifgoto_1op>() );
        matchers_.emplace_back( std::make_unique<Match_ifgoto_2op>() );
        matchers_.emplace_back( std::make_unique<Match_math_op1>() );
        matchers_.emplace_back( std::make_unique<Match_math_op2>() );
        matchers_.emplace_back( std::make_unique<Match_asm>() );
        matchers_.emplace_back( std::make_unique<Match_print_text>() );
        matchers_.emplace_back( std::make_unique<Match_comment_line>() );
        matchers_.emplace_back( std::make_unique<Match_assert1>() );
        matchers_.emplace_back( std::make_unique<Match_assert2>() );
        matchers_.emplace_back( std::make_unique<Match_0_1_1i>() );

#ifdef BLUE_PRINT_OPCODES_FROM_CTOR
        auto known_codes = all_opcodes();
        int counter = 0;

        auto f = cout.flags();
        for_each( known_codes.cbegin(), known_codes.cend(), [&]( std::uint16_t c )
        {
            cout << "OPCODE: 0x" << std::hex << std::uppercase << std::setw( 4 ) << std::setfill( '0' ) << c  << ' ' << counter++ << '\n';
        } );
        cout.flags( f );
#endif
    }

    Compilation_result Hidden_compiler::compile( const std::string_view& line ) const
    {
        Instruction x;

        for( size_t i = 0; i < matchers_.size(); ++i )
        {
            if( matchers_[i]->compiles( line, x ) )
            {
                //#ifndef NDEBUG
                // cout << "\"" << line << "\" decompiles to \"" << decompile(x) << "\"\n";
                //#endif
                return Compilation_result( x );
            }
        }

        return Compilation_result( "Unable to compile " + std::string(line) );
    }

    std::string Hidden_compiler::decompile( const Instruction& x ) const
    {
        if( x == Instruction::nop() )
        {
            return "";
        }

        for( size_t i = 0; i < matchers_.size(); ++i )
        {
            std::string s;
            if( matchers_[i]->decompiles( x, s ) )
            {
                return s;
            }
        }

        // Default is to convert to asm.
        std::string::value_type asmText[ 4 * 8 ] = { 0 };
        if( x[0] == BLUE_ASM_NOP && 0 == x[2] && 0 == x[3] )
        {
            SNPRINTF( asmText, sizeof( asmText ) / sizeof( std::string::value_type ),
                      "%5u", x[1] );
        }
        else
        {
            SNPRINTF( asmText, sizeof( asmText ) / sizeof( std::string::value_type ),
                      "0x%04x 0x%04x 0x%04x 0x%04x", x[0], x[1], x[2], x[3] );
        }

        return std::string( asmText );
    }

    std::vector<const char*> Hidden_compiler::predict_text( const char* text ) const
    {
        const auto lhs_lt_rhs = [ = ]( const char* lhs, const char* rhs ) -> bool
        {
            const bool both_are_text = ( lhs != nullptr && rhs != nullptr );
            return both_are_text ? ( strcmp( lhs, rhs ) < 0 ) : ( lhs < rhs );
        };

        const auto lhs_eq_rhs = [ = ]( const char* lhs, const char* rhs ) -> bool
        {
            const bool both_are_text = ( lhs != nullptr && rhs != nullptr );
            return both_are_text ? ( strcmp( lhs, rhs ) == 0 ) : ( lhs == rhs );
        };

        std::vector<const char*> possible_commands;

        const auto len = strlen( text );

        if( len > 0 )
        {
            possible_commands.reserve( 8u );

            for( size_t i = 0; i < matchers_.size(); ++i )
            {
                matchers_[i]->append_possible_commands( text, len, possible_commands );
            }

            std::sort( possible_commands.begin(), possible_commands.end(), lhs_lt_rhs );

            // Remove duplicates. std::unique removes only side-by-side duplicates.
            possible_commands.erase(
                unique( possible_commands.begin(), possible_commands.end(), lhs_eq_rhs ),
                possible_commands.end() );
        }

        return possible_commands;
    }

    std::set<std::uint16_t> Hidden_compiler::all_opcodes() const
    {
        std::vector<std::uint16_t> codes;
        codes.reserve( 128 );

        for( size_t i = 0; i < matchers_.size(); ++i )
        {
            matchers_[i]->append_opcodes( codes );
        }

        return std::set<std::uint16_t>( codes.cbegin(), codes.cend() );
    }

    //-------------------------------------------------------------------------

    Instructions compile_code( const std::vector<std::string>& code )
    {
        const size_t loc = code.size();
        std::vector<Instruction> compiled_code;
        compiled_code.reserve( loc );

        for( size_t i = 0; i < loc; ++i )
        {
            compiled_code.push_back( compile( code[i] ) );
        }

        return Instructions( move( compiled_code ) );
    }

    Instructions compile_code( const std::string_view& text )
    {
        std::istringstream input;
        input.str(std::string(text));
        std::vector<std::string> code;
        code.reserve(128u);
        for (std::string line; std::getline(input, line); )
        {
            code.push_back(line);
        }

        return compile_code( code );
    }

    Compilation_result try_compile( const std::string_view& line )
    {
        return Hidden_compiler::get()->compile( line );
    }

    Instruction compile( const std::string_view& line )
    {
        return try_compile( line ).get_instruction();
    }

    std::string decompile( const Instruction& x )
    {
        return Hidden_compiler::get()->decompile( x );
    }

    std::vector<const char*> predict_text( const char* text )
    {
        return Hidden_compiler::get()->predict_text( text );
    }
}
