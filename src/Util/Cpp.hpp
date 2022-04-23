#pragma once

namespace Util
{
    
    template <class T, T v>
    struct integral_constant 
    {
        static constexpr T value = v;
        using value_type = T;
        using type = integral_constant; // using injected-class-name
        constexpr operator value_type() const noexcept { return value; }
        constexpr value_type operator()() const noexcept { return value; } // since c++14
    };

    template <class T, class U>
    struct is_same : integral_constant<bool, false> {};
    
    template <class T>
    struct is_same<T, T> : integral_constant<bool, true> {};

    template<bool B, class T = void>
    struct enable_if {};
    
    template<class T>
    struct enable_if<true, T> { using type = T; };

    template< bool B, class T = void >
    using enable_if_t = typename enable_if<B,T>::type;

    template< class T, class U >
    inline constexpr bool is_same_v = is_same<T, U>::value;

} // namespace Util
