#include "solosnake/win32/includewindows.hpp"
#include "solosnake/win32/exception_code_description.hpp"

namespace solosnake
{
    const char* get_exceptioncode_description( const unsigned int e )
    {
        switch( e )
        {
            case EXCEPTION_ACCESS_VIOLATION:
                return
                    "EXCEPTION_ACCESS_VIOLATION: The thread attempts to read from or write to a "
                    "virtual address for which it does not have access.";

            case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
                return
                    "EXCEPTION_ARRAY_BOUNDS_EXCEEDED: The thread attempts to access an array "
                    "element that is out of bounds, and the underlying hardware supports bounds "
                    "checking.";

            case EXCEPTION_BREAKPOINT:
                return
                    "EXCEPTION_BREAKPOINT: A breakpoint is encountered.";

            case EXCEPTION_DATATYPE_MISALIGNMENT:
                return
                    "EXCEPTION_DATATYPE_MISALIGNMENT: The thread attempts to read or write data "
                    "that is misaligned on hardware that does not provide alignment. For example, "
                    "16-bit values must be aligned on 2-byte boundaries, 32-bit values on 4-byte "
                    "boundaries, and so on.";

            case EXCEPTION_FLT_DENORMAL_OPERAND:
                return
                    "EXCEPTION_FLT_DENORMAL_OPERAND: One of the operands in a floating point "
                    "operation is denormal. A denormal value is one that is too small to "
                    "represent as a standard floating point value.";

            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                return
                    "EXCEPTION_FLT_DIVIDE_BY_ZERO: The thread attempts to divide a floating "
                    "point value by a floating point divisor of 0 (zero).";

            case EXCEPTION_FLT_INEXACT_RESULT:
                return
                    "EXCEPTION_FLT_INEXACT_RESULT: The result of a floating point operation "
                    "cannot be represented exactly as a decimal fraction.";

            case EXCEPTION_FLT_INVALID_OPERATION:
                return
                    "EXCEPTION_FLT_INVALID_OPERATION: A floating point exception that is not "
                    "included in this list.";

            case EXCEPTION_FLT_OVERFLOW:
                return
                    "EXCEPTION_FLT_OVERFLOW: The exponent of a floating point operation is "
                    "greater than the magnitude allowed by the corresponding type.";

            case EXCEPTION_FLT_STACK_CHECK:
                return
                    "EXCEPTION_FLT_STACK_CHECK: The stack has overflowed or underflowed, because "
                    "of a floating point operation.";

            case EXCEPTION_FLT_UNDERFLOW:
                return
                    "EXCEPTION_FLT_UNDERFLOW: The exponent of a floating point operation is less "
                    "than the magnitude allowed by the corresponding type.";

            case EXCEPTION_GUARD_PAGE:
                return
                    "EXCEPTION_GUARD_PAGE: The thread accessed memory allocated with the "
                    "PAGE_GUARD modifier.";

            case EXCEPTION_ILLEGAL_INSTRUCTION:
                return
                    "EXCEPTION_ILLEGAL_INSTRUCTION: The thread tries to execute an invalid "
                    "instruction.";

            case EXCEPTION_IN_PAGE_ERROR:
                return
                    "EXCEPTION_IN_PAGE_ERROR: The thread tries to access a page that is not "
                    "present, and the system is unable to load the page. For example, this "
                    "exception might occur if a network connection is lost while running a "
                    "program over a network.";

            case EXCEPTION_INT_DIVIDE_BY_ZERO:
                return
                    "EXCEPTION_INT_DIVIDE_BY_ZERO: The thread attempts to divide an integer "
                    "value by an integer divisor of 0 (zero).";

            case EXCEPTION_INT_OVERFLOW:
                return
                    "EXCEPTION_INT_OVERFLOW: The result of an integer operation creates a "
                    "value that is too large to be held by the destination register. In some "
                    "cases, this will result in a carry out of the most significant bit of "
                    "the result. Some operations do not set the carry flag.";

            case EXCEPTION_INVALID_DISPOSITION:
                return
                    "EXCEPTION_INVALID_DISPOSITION: An exception handler returns an invalid "
                    "disposition to the exception dispatcher. Programmers using a high-level "
                    "language such as C should never encounter this exception.";

            case EXCEPTION_INVALID_HANDLE:
                return
                    "EXCEPTION_INVALID_HANDLE: The thread used a handle to a kernel object "
                    "that was invalid (probably because it had been closed.)";

            case EXCEPTION_NONCONTINUABLE_EXCEPTION:
                return
                    "EXCEPTION_NONCONTINUABLE_EXCEPTION: The thread attempts to continue "
                    "execution after a non-continuable exception occurs.";

            case EXCEPTION_PRIV_INSTRUCTION:
                return
                    "EXCEPTION_PRIV_INSTRUCTION: The thread attempts to execute an "
                    "instruction with an operation that is not allowed in the current "
                    "computer mode.";

            case EXCEPTION_SINGLE_STEP:
                return
                    "EXCEPTION_SINGLE_STEP: A trace trap or other single instruction "
                    "mechanism signals that one instruction is executed.";

            case EXCEPTION_STACK_OVERFLOW:
                return
                    "EXCEPTION_STACK_OVERFLOW: The thread uses up its stack.";

            case STATUS_UNWIND_CONSOLIDATE :
                return
                    "STATUS_UNWIND_CONSOLIDATE: A frame consolidation has been executed.";
        }

        return "Unknown.";
    }
}