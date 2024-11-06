#ifndef SOLOSNAKE_LOG_HPP
#define SOLOSNAKE_LOG_HPP

#include <string_view>

namespace solosnake
{
    /// Abstract log interface.
    class Log
    {
    public:

        /// Destructor that does nothing.
        virtual ~Log() noexcept;

        /// Writes the given informational text to the log system.
        /// @param text The informational text to write to the log.
        virtual void log_info(const std::string_view& text) noexcept = 0;

        /// Writes the given error text to the log system.
        /// @param text The error text to write to the log.
        virtual void log_error(const std::string_view& text) noexcept = 0;
    };
}

#endif
