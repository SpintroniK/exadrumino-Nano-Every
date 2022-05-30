#pragma once

namespace Commands
{

    inline constexpr const char* commands[]{"set", "get"};
    inline constexpr const char* pads[]{ "kick", "snare", "crash", "tom", "floorTom", "ride" };
    inline constexpr const char* parameters[]{"threshold", "scan", "mask"};

    struct Arguments
    {
        uint8_t target{};
        uint8_t arg1{};
        uint8_t arg2{};
    };
    
    struct Command
    {
        uint8_t type{};
        Arguments args{};
    };



} // namespace Commands
