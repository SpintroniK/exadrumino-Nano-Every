#pragma once

namespace Timing
{
    
    template <typename T>
    class Counter
    {     

    public:

        Counter() = default;
        ~Counter() = default;

        constexpr T GetCount()
        {
            return count;
        }

        constexpr void Increment()
        {
            ++count;
        }

    private:

        T count{};
    };

} // namespace Timing
