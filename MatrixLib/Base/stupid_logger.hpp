#include <cstdio>
#include <fstream>
#include <chrono>
#include <iostream>

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
/// Not much optimized (might became better with C++17), not much pretty,
/// but doing its job with no questions and additional external deps.
/// Simple usage example is at the very and of this file.
template <logger::level type>
class stupid
{
public:
    // TODO: with C++17 replace level with template parameter and
    // check it with 'if constexpr'. this will help optimizer to
    // throw out useless log calls at compile time (though it is
    // done anyway, at least by GCC)
    class entry
    {
        friend class stupid;

        // TODO: with C++17 replace caller info with std::source_location
        entry(std::ostream& out, logger::level level, const char* file, int line, const char* format)
        : _out{out}
        , _level{level}
        {
            if (_level != logger::level::nolog)
            {
                _format = format;
                _file = file;
                _line = line;
            }
        }

    public:
        entry(entry&&) = default;
        ~entry() = default;

        template<typename... Args>
        void operator() (Args&&... args)
        {
            if (_level != logger::level::nolog)
            {
                std::string fmt = std::string{"%s |%5s| "} + _format + " | %s:%d\n";
                char buf[1024];
                int len = 
                    std::snprintf(
                        buf,
                        sizeof(buf),
                        fmt.c_str(),
                        get_formatted_time().c_str(),
                        get_formatted_level(),
                        args...,
                        _file,
                        _line);

                if (len > 0)
                {
                    _out.write(buf, len);
                    _out.flush();
                }
            }
        }
    
    private:
        const char* get_formatted_level()
        {
            switch(_level)
            {
                case level::fatal:   return "FATAL";
                case level::error:   return "ERROR";
                case level::warning: return " WARN";
                case level::info:    return " INFO";
                case level::debug:   return "DEBUG";
                case level::trace:   return "TRACE";
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
        logger::level _level;
        const char* _format;
        const char* _file;
        int _line;
    };

    stupid(const std::string& path)
    : _out{std::cout}
    {
        // if (!_out.is_open())
        // {
        //     throw std::runtime_error("Failed to open log file: " + path);
        // }
    }

    entry fatal(const char* file, int line, const char* format)
    {
        return create_entry<level::fatal>(file, line, format);
    }

    entry error(const char* file, int line, const char* format)
    {
        return create_entry<level::error>(file, line, format);
    }

    entry warning(const char* file, int line, const char* format)
    {
        return create_entry<level::warning>(file, line, format);
    }

    entry info(const char* file, int line, const char* format)
    {
        return create_entry<level::info>(file, line, format);
    }

    entry debug(const char* file, int line, const char* format)
    {
        return create_entry<level::debug>(file, line, format);
    }

    entry trace(const char* file, int line, const char* format)
    {
        return create_entry<level::trace>(file, line, format);
    }

private:
    template <logger::level value>
    entry create_entry(const char* file, int line, const char* format)
    {
        if (value <= type)
        {
            return entry(_out, value, file, line, format);
        }
        else
        {
            return entry(_out, logger::level::nolog, file, line, format);
        }
    }

    std::ostream& _out;
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

#define LOG_F(fmt) lgr.fatal(__FILE__, __LINE__, (fmt))
#define LOG_E(fmt) lgr.error(__FILE__, __LINE__, (fmt))
#define LOG_W(fmt) lgr.warning(__FILE__, __LINE__, (fmt))
#define LOG_I(fmt) lgr.info(__FILE__, __LINE__, (fmt))
#define LOG_D(fmt) lgr.debug(__FILE__, __LINE__, (fmt))
#define LOG_T(fmt) lgr.trace(__FILE__, __LINE__, (fmt))

// Usage example:
// int main()
// {
//     logger_type lgr("./test.log");
//     LOG_F("%s - %d")("FATAL TEST", 42);
//     LOG_E("%s - %d")("ERROR TEST", 42);
//     LOG_W("%s - %d")("WARNING TEST", 43);
//     LOG_I("%s - %d")("INFO TEST", 44);
//     LOG_D("%s - %d")("DEBUG TEST", 45);
//     LOG_T("%s - %d")("TRACE TEST", 46);

//     return 0;
// }
