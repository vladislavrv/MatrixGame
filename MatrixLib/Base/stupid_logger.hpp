#include <cstdio>
#include <fstream>
#include <chrono>
#include <iostream>
#include <source_location>
#include <string_view>

namespace logger
{
enum class level : uint8_t
{ 
    fatal,
    error,
    warning,
    info,
    debug,
    trace,
    nolog
}; 

/// Just a stupid logger class.
/// Not much optimized, not much pretty, but doing its job with no
/// questions and additional external deps.
/// Simple usage example is at the very end of this file.
template <logger::level type>
class stupid
{
public:
    template <logger::level _level>
    class entry
    {
        friend class stupid;

        entry(std::ostream& out, std::source_location caller, std::string_view format)
        : _out{out}
        {
            if constexpr (_level != logger::level::nolog)
            {
                _format = format;
                _caller = caller;
            }
        }

    public:
        entry(entry&&) = default;
        ~entry()
        {
            if (!_logged)
            {
                (*this)();
            }
        }

        template<typename... Args>
        void operator() (Args&&... args)
        {
            if constexpr (_level != logger::level::nolog)
            {
                if (_logged)
                {
                    return;
                }

                std::string fmt = "%s |%5s| " + std::string{_format} + " | %s:%d\n";
                char buf[1024];
                int len = 
                    std::snprintf(
                        buf,
                        sizeof(buf),
                        fmt.c_str(),
                        get_formatted_time().c_str(),
                        get_formatted_level(),
                        args...,
                        _caller.file_name(),
                        _caller.line());

                if (len > 0)
                {
                    _out.write(buf, len);
                    _out.flush();
                }

                _logged = true;
            }
        }
    
    private:
        static consteval const char* get_formatted_level()
        {
            switch(_level)
            {
                case level::fatal:   return "FATAL";
                case level::error:   return "ERROR";
                case level::warning: return " WARN";
                case level::info:    return " INFO";
                case level::debug:   return "DEBUG";
                case level::trace:   return "TRACE";
                case level::nolog:   return "NOLOG"; // shouldn't happen
            }

            return ""; // unreachable
        }

        std::string get_formatted_time()
        {
            auto dur = std::chrono::system_clock::now().time_since_epoch();
            auto sec = std::chrono::duration_cast<std::chrono::seconds>(dur);
            auto usec = std::chrono::duration_cast<std::chrono::microseconds>(dur - sec);

            char buf[32] = {0};
            time_t tt = sec.count();
            auto offset = std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&tt));
            std::snprintf(buf + offset, sizeof(buf) - offset, ".%06lld", usec.count());
            return buf;
        }

        std::ostream& _out;
        std::string_view _format;
        std::source_location _caller;
        bool _logged{false};
    };

    stupid(const std::string& path)
    : _out{path, std::ios::app}
    {
        if (!_out.is_open())
        {
            throw std::runtime_error("Failed to open log file: " + path);
        }
    }

    auto fatal(const char* format, std::source_location caller = std::source_location::current())
    {
        return create_entry<level::fatal>(caller, format);
    }

    auto error(const char* format, std::source_location caller = std::source_location::current())
    {
        return create_entry<level::error>(caller, format);
    }

    auto warning(const char* format, std::source_location caller = std::source_location::current())
    {
        return create_entry<level::warning>(caller, format);
    }

    auto info(const char* format, std::source_location caller = std::source_location::current())
    {
        return create_entry<level::info>(caller, format);
    }

    auto debug(const char* format, std::source_location caller = std::source_location::current())
    {
        return create_entry<level::debug>(caller, format);
    }

    auto trace(const char* format, std::source_location caller = std::source_location::current())
    {
        return create_entry<level::trace>(caller, format);
    }

private:
    template <logger::level value>
    auto create_entry(std::source_location caller, const char* format)
    {
        if constexpr (value <= type)
        {
            return entry<value>(_out, caller, format);
        }
        else
        {
            return entry<logger::level::nolog>(_out, caller, format);
        }
    }

    std::ofstream _out;
};

} // namespace logger

// Logger object should be defined somewhere else,
// probably somewhere close to main()
#ifdef _DEBUG
    using logger_type = logger::stupid<logger::level::trace>;
#else
    using logger_type = logger::stupid<logger::level::warning>;
#endif

extern logger_type lgr;

// Usage example:
// int main()
// {
//     logger_type lgr("./test.log");
//     lgr.fatal("%s - %d")("FATAL TEST", 42);
//     lgr.error("%s - %d")("ERROR TEST", 42);
//     lgr.warning("%s - %d")("WARNING TEST", 43);
//     lgr.info("%s - %d")("INFO TEST", 44);
//     lgr.debug("%s - %d")("DEBUG TEST", 45);
//     lgr.trace("%s - %d")("TRACE TEST", 46);

//     return 0;
// }
