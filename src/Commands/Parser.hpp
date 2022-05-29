#pragma once

#include "../String/String.hpp"
#include "../Util/Cpp.hpp"

#include <stdint.h>

namespace Commands
{

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


    class Parser
    {

    public:
        Parser() = default;
        ~Parser() = default;

        void Parse(char c)
        {
            
            switch(state)
            {
                case 0:
                {
                    if(c == '/')
                    {
                        state = 1;
                    }
                    break;
                }
                case 1:
                {
                    if(c == ' ')
                    {
                        buffer.Clear();
                        break;
                    }

                    buffer.Append(c);

                    if(buffer == "set")
                    {
                        type = 1;
                    }

                    if(buffer == "snare")
                    {
                        args.target = 1;
                    }
                    
                    if(buffer == "threshold")
                    {
                        args.arg1 = 1;
                        state = 2;
                    }
                    
                    break;
                }
                case 2: break; // Command needs to be exectued to switch to state 3
                case 3:
                {
                    type = 0;   
                    args = {};    
                    buffer.Clear();

                    state = 0;

                    break;
                }
            }
        }

        Util::Optional<Command> PopCommand()
        {
            if(state != 2)
            {
                return {};
            }

            state = 3;

            return {type, args};
        }

    private:

        static constexpr uint8_t bufferLength = 16U;

        uint8_t index{};
        uint8_t state{};
        uint8_t type{};
        Arguments args{};

        String<bufferLength> buffer{};
    };

} // namespace Commands
