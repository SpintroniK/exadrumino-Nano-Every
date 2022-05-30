#pragma once

#include "Command.hpp"

#include "../Util/Cpp.hpp"

#include <stdint.h>

namespace Commands
{
    
    class Executor
    {        

    public:
        Executor() = delete;
        ~Executor() = default;

        static constexpr Util::optional<uint8_t> Run(const Command& commad)
        {
            using namespace Util;

            switch (commad.type)
            {
                case 1:
                {
                    return 10;
                }
                case 2:
                {
                    return 12;
                }
                
                default:
                    break;
            }

            return {};
        }

    private:

    };

} // namespace Commands
