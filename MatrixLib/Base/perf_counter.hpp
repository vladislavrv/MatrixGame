#include <vector>
#include <chrono>
#include <utility>
#include <format>

class perf_counter
{
    using clock = std::chrono::high_resolution_clock;

public:
    perf_counter(size_t size = 4)
    {
        _values.reserve(size);
        reset();
    }

    void add(std::string name)
    {
        _values.emplace_back(name, clock::now());
    }

    void reset()
    {
        _values.clear();
        _values.emplace_back("", clock::now());
    }

    std::string get_string() const
    {
        constexpr auto to_microseconds = [](auto dur) { return std::chrono::duration_cast<std::chrono::microseconds>(dur); };

        std::string result;

        auto prev = _values.begin();
        for (auto cur = prev + 1; cur != _values.end(); ++prev, ++cur)
        {
            auto dur = to_microseconds(cur->second - prev->second);
            result += std::format("{}{}:{}", (result.empty() ? "" : "/"), cur->first, dur.count());
        }

        return result;
    }

private:
    std::vector<std::pair<std::string, clock::time_point>> _values;
};