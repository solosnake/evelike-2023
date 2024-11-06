#ifndef blue_instruction_hpp
#define blue_instruction_hpp

#include <array>
#include <cstdint>
#include <string_view>
#include <vector>
#include "solosnake/blue/blue_fixed_angle.hpp"
#include "solosnake/blue/blue_unaliased.hpp"

namespace blue
{
    constexpr std::uint16_t BLUE_MAX_BOT_INSTRUCTIONS        {1000};
    constexpr std::uint16_t BLUE_INSTRUCTION_TEXT_MAX_LENGTH {6u};

    constexpr std::uint16_t BLUE_ASM_NOP                     {0x0000};
    constexpr std::uint16_t BLUE_ASM_JMP                     {0x0001};
    constexpr std::uint16_t BLUE_ASM_STORE_JMP               {0x0002};
    constexpr std::uint16_t BLUE_ASM_COPY                    {0x0003};
    constexpr std::uint16_t BLUE_ASM_PRINT_TXT               {0x0004};
    constexpr std::uint16_t BLUE_ASM_PRINT_VAL               {0x0005};
    constexpr std::uint16_t BLUE_ASM_ADD                     {0x0006};
    constexpr std::uint16_t BLUE_ASM_SUB                     {0x0007};
    constexpr std::uint16_t BLUE_ASM_MUL                     {0x0008};
    constexpr std::uint16_t BLUE_ASM_DIV                     {0x0009};
    constexpr std::uint16_t BLUE_ASM_RAND_RANGE              {0x000A};
    constexpr std::uint16_t BLUE_ASM_RAND                    {0x000B};
    constexpr std::uint16_t BLUE_ASM_CAP                     {0x000C};
    constexpr std::uint16_t BLUE_ASM_MAX_CAP                 {0x000D};
    constexpr std::uint16_t BLUE_ASM_SYNCH                   {0x000E};
    constexpr std::uint16_t BLUE_ASM_SENSE                   {0x000F};
    constexpr std::uint16_t BLUE_ASM_SENSE_MAX               {0x0010};
    constexpr std::uint16_t BLUE_ASM_TURN                    {0x0011};
    constexpr std::uint16_t BLUE_ASM_ADVANCE                 {0x0012};
    constexpr std::uint16_t BLUE_ASM_ADVANCE_WITH_STORE      {0x0013};
    constexpr std::uint16_t BLUE_ASM_SENSE_RESULT_COUNT      {0x0014};
    constexpr std::uint16_t BLUE_ASM_SENSE_RESULT_LOCATION   {0x0015};
    constexpr std::uint16_t BLUE_ASM_SENSE_RESULT_TYPE       {0x0016};
    constexpr std::uint16_t BLUE_ASM_MAX_SENSE_RANGE         {0x0017};
    constexpr std::uint16_t BLUE_ASM_CW_TURN_COUNT_TO        {0x0018};
    constexpr std::uint16_t BLUE_ASM_DISTANCE_TO             {0x0019};
    constexpr std::uint16_t BLUE_ASM_DISTANCE_BETWEEN        {0x001A};
    constexpr std::uint16_t BLUE_ASM_GET_LOCATION            {0x001B};
    constexpr std::uint16_t BLUE_ASM_GET_LINENUMBER_PLUS     {0x001C};
    constexpr std::uint16_t BLUE_ASM_LINENUMBER              {0x001D};
    constexpr std::uint16_t BLUE_ASM_LINE_COUNT              {0x001E};
    constexpr std::uint16_t BLUE_ASM_MAX_TRANSMIT_RANGE      {0x001F};
    constexpr std::uint16_t BLUE_ASM_TRANSMIT                {0x0020};
    constexpr std::uint16_t BLUE_ASM_MAX_BROADCAST_RANGE     {0x0021};
    constexpr std::uint16_t BLUE_ASM_BROADCAST               {0x0022};
    constexpr std::uint16_t BLUE_ASM_GET_BLUEPRINT_COUNT     {0x0023};
    constexpr std::uint16_t BLUE_ASM_ASSERT                  {0x0024};
    constexpr std::uint16_t BLUE_ASM_XOR                     {0x0025};
    constexpr std::uint16_t BLUE_ASM_OR                      {0x0026};
    constexpr std::uint16_t BLUE_ASM_AND                     {0x0027};
    constexpr std::uint16_t BLUE_ASM_LEFT_SHIFT              {0x0028};
    constexpr std::uint16_t BLUE_ASM_RIGHT_SHIFT             {0x0029};
    constexpr std::uint16_t BLUE_ASM_EQ                      {0x002A};
    constexpr std::uint16_t BLUE_ASM_NEQ                     {0x002B};
    constexpr std::uint16_t BLUE_ASM_BOOL_AND                {0x002C};
    constexpr std::uint16_t BLUE_ASM_BOOL_OR                 {0x002D};
    constexpr std::uint16_t BLUE_ASM_BOOL_LT                 {0x002E};
    constexpr std::uint16_t BLUE_ASM_BOOL_GT                 {0x002F};
    constexpr std::uint16_t BLUE_ASM_BOOL_LTE                {0x0030};
    constexpr std::uint16_t BLUE_ASM_BOOL_GTE                {0x0031};
    constexpr std::uint16_t BLUE_ASM_MODULO                  {0x0032};
    constexpr std::uint16_t BLUE_ASM_JMP_EQ                  {0x0033};
    constexpr std::uint16_t BLUE_ASM_JMP_NEQ                 {0x0034};
    constexpr std::uint16_t BLUE_ASM_JMP_LT                  {0x0035};
    constexpr std::uint16_t BLUE_ASM_JMP_GT                  {0x0036};
    constexpr std::uint16_t BLUE_ASM_JMP_LTE                 {0x0037};
    constexpr std::uint16_t BLUE_ASM_JMP_GTE                 {0x0038};
    constexpr std::uint16_t BLUE_ASM_ABS                     {0x0039};
    constexpr std::uint16_t BLUE_ASM_SWAP_CODE               {0x003A};
    constexpr std::uint16_t BLUE_ASM_COPY_CODE               {0x003B};
    constexpr std::uint16_t BLUE_ASM_COMPARE_CODE_LINE       {0x003C};
    constexpr std::uint16_t BLUE_ASM_MSG_CLEAR_MSGS          {0x003D};
    constexpr std::uint16_t BLUE_ASM_COPY_MSGBUF_CODE        {0x003E};
    constexpr std::uint16_t BLUE_ASM_MOVE_MSG_MSGBUF         {0x003F};
    constexpr std::uint16_t BLUE_ASM_MSGBUF_LINE_COUNT       {0x0040};
    constexpr std::uint16_t BLUE_ASM_GET_MSGS_COUNT          {0x0041};
    constexpr std::uint16_t BLUE_ASM_GET_MAX_MSGS            {0x0042};
    constexpr std::uint16_t BLUE_ASM_CAN_BUILD               {0x0043};
    constexpr std::uint16_t BLUE_ASM_BUILD                   {0x0044};
    constexpr std::uint16_t BLUE_ASM_CAN_AIM_AT              {0x0045};
    constexpr std::uint16_t BLUE_ASM_AIM_AT                  {0x0046};
    constexpr std::uint16_t BLUE_ASM_CAN_FIRE_AT             {0x0047};
    constexpr std::uint16_t BLUE_ASM_FIRE_AT                 {0x0048};
    constexpr std::uint16_t BLUE_ASM_HAS_LINE_OF_FIRE_TO     {0x0049};
    constexpr std::uint16_t BLUE_ASM_SELF_DESTRUCT           {0x004A};
    constexpr std::uint16_t BLUE_ASM_IS_LOCATION             {0x004B};
    constexpr std::uint16_t BLUE_ASM_IS_CARGO_LOCKED         {0x004C};
    constexpr std::uint16_t BLUE_ASM_LOCK_CARGO              {0x004D};
    constexpr std::uint16_t BLUE_ASM_CARGOHOLD_FREE_SPACE    {0x004E};
    constexpr std::uint16_t BLUE_ASM_CARGOHOLD_SPACE_USED    {0x004F};
    constexpr std::uint16_t BLUE_ASM_CARGOHOLD_VOLUME        {0x0050};
    constexpr std::uint16_t BLUE_ASM_CARGOHOLD_UNITS_OF      {0x0051};
    constexpr std::uint16_t BLUE_ASM_CARGOHOLD_VOL_OF        {0x0052};
    constexpr std::uint16_t BLUE_ASM_GIVE_CARGO_TO           {0x0053};
    constexpr std::uint16_t BLUE_ASM_TAKE_CARGO_FROM         {0x0054};
    constexpr std::uint16_t BLUE_ASM_NAVIGATE_TO             {0x0055};
    constexpr std::uint16_t BLUE_ASM_NAVIGATE_N_TOWARDS      {0x0056};
    constexpr std::uint16_t BLUE_ASM_SET_ASTAR_WEIGHT        {0x0057};
    constexpr std::uint16_t BLUE_ASM_GET_ASTAR_WEIGHT        {0x0058};
    constexpr std::uint16_t BLUE_ASM_REFINE                  {0x0059};
    constexpr std::uint16_t BLUE_ASM_GET_REFINE_PERIOD       {0x005A};
    constexpr std::uint16_t BLUE_ASM_PLOT_ROUTE_BETWEEN      {0x005B};
    constexpr std::uint16_t BLUE_ASM_GET_ROUTE_LENGTH        {0x005C};
    constexpr std::uint16_t BLUE_ASM_COPY_ROUTE              {0x005D};
    constexpr std::uint16_t BLUE_ASM_SET_ERROR_CODE          {0x005E};
    constexpr std::uint16_t BLUE_ASM_GET_ERROR_CODE          {0x005F};
    constexpr std::uint16_t BLUE_ASM_GET_PRICE               {0x0060};
    constexpr std::uint16_t BLUE_ASM_GET_AVAILABILITY        {0x0061};
    constexpr std::uint16_t BLUE_ASM_TRY_BUY                 {0x0062};
    constexpr std::uint16_t BLUE_ASM_TRY_BUY_WITH_STORE      {0x0063};
    constexpr std::uint16_t BLUE_ASM_FOR_SALE                {0x0064};
    constexpr std::uint16_t BLUE_ASM_CANCEL_SALES            {0x0065};
    constexpr std::uint16_t BLUE_ASM_GET_CREDITS_BALANCE     {0x0066};
    constexpr std::uint16_t BLUE_ASM_TRANSFER_CREDITS_TO     {0x0067};

    // Special
    constexpr std::uint16_t BLUE_ASM_LOCATION                {0x0999}; // Not directly available to users.

    //--------------------------------------------------------------------------
    // MASKS
    //--------------------------------------------------------------------------
    constexpr std::uint16_t BLUE_VALUE_ARG_MASK              {0xF000u};
    constexpr std::uint16_t BLUE_INSTRUCTION_MASK            {0x0FFFu};
    constexpr std::uint16_t BLUE_VALUE_ARG0                  {0x1000u};
    constexpr std::uint16_t BLUE_VALUE_ARG1                  {0x2000u};
    constexpr std::uint16_t BLUE_VALUE_ARG2                  {0x4000u};

    static_assert( ( 2 * sizeof( char ) ) == sizeof( std::uint16_t ),
                    "Size of char not half size of uint16_t!" );

    /// A single Instruction for the blue Machine.
    class Instruction
    {
    public:

        /// Returns a raw Instruction; this is used by the Instruction compiler.
        /// We can if we wish add validation here that these are well formed
        /// Instructions.
        static Instruction from_asm(
            std::uint16_t cmd_with_flags,
            std::uint16_t a = 0,
            std::uint16_t b = 0,
            std::uint16_t c = 0 );

        /// Converts (without any form of checking) each chunk of 4 unsigned
        /// shorts into an Instruction. Throws if the incoming vector size is
        /// not a multiple of 4.
        static std::vector<Instruction> asm_to_instructions(
            const std::vector<std::uint16_t>& );

        /// n is number of Instructions, not number of uint16's.
        static std::vector<Instruction> asm_to_instructions(
            const std::uint16_t* unaliased p,
            size_t n );

        Instruction() = default;

        Instruction( const Instruction& ) = default;

        Instruction& operator= ( const Instruction& ) = default;

        Instruction( Instruction&& ) = default;

        Instruction& operator= ( Instruction && ) = default;

        //----------------------------------------------------------------------
        // SPECIAL STATIC CONSTRUCTOR FUNCTIONS.
        //----------------------------------------------------------------------

        static Instruction value( std::uint16_t );

        static Instruction nop();

        static Instruction location( std::uint8_t x, std::uint8_t y );

        //----------------------------------------------------------------------

        static bool check_assert( std::uint16_t op1, std::uint16_t op2, std::uint16_t op );

        /// Returns the assert operator as a string, and true if it comes
        /// after the first op.
        static bool assert_op_to_str( std::uint16_t op, char s[3] );

        //----------------------------------------------------------------------

        /// Used when an instruction is printed from within an machine's code.
        /// Returns the 3 arguments in some printable format. Non ASCII elements
        /// will be printed as '?'.
        std::string         args_as_string() const;

        /// Used when an instruction is printed from within an machine's code.
        /// Returns the 3 arguments as 6 chars. Non ASCII elements will be
        /// printed as '?'.
        std::array<char,6>  args_as_chars() const noexcept;

        std::uint16_t       cmd() const noexcept;

        std::uint16_t       flags() const noexcept;

        std::uint16_t       arg( unsigned int n ) const noexcept;

        std::uint16_t&      arg( unsigned int n ) noexcept;

        std::uint16_t       operator[]( size_t n ) const noexcept;

        bool is_indirection( unsigned int n ) const noexcept;

        bool operator == ( const Instruction& rhs ) const noexcept;

        bool operator != ( const Instruction& rhs ) const noexcept;

    private:

        friend class to_from_json;

        static constexpr std::uint16_t cast( unsigned int n ) noexcept;

        Instruction(
            std::uint16_t cmd_with_flags,
            std::uint16_t a = 0,
            std::uint16_t b = 0,
            std::uint16_t c = 0 ) noexcept;

    private:

        std::uint16_t      command_ = { BLUE_ASM_NOP };
        std::uint16_t      args_[3] = { 0, 0, 0 };
    };
}

#include "solosnake/blue/blue_instruction.inl"
#endif
