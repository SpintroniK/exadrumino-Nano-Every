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

        static uint8_t Run(const Command& command, uint8_t*(&tp)[1][3])
        {
            using namespace Util;

            switch(command.type)
            {
                case 1: // set
                {
                    return command.args.arg2;
                }
                case 2: // get
                {
                    return 0;
                }
                
                default:
                    break;
            }

            return {};
        }

    private:

    };

} // namespace Commands
