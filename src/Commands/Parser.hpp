#pragma once

#include "Command.hpp"

#include "../String/String.hpp"
#include "../Util/Cpp.hpp"

#include <stdint.h>

namespace Commands
{


    class Parser
    {

    public:
        Parser() = default;
        ~Parser() = default;

        void Parse(char c)
        {
            using namespace Util;

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

                    const auto commandIt = find_if(begin(commands), end(commands), [&] (const auto& v) { return buffer == v; });
                    if(commandIt != end(commands))
                    {
                        type = buffer == commands[0]? 1 : 2; // 1 = set, 2 = get
                    }

                    const auto padIt = find_if(begin(pads), end(pads), [&] (const auto& v) { return buffer == v;  });
                    if(padIt != end(pads))
                    {
                        args.target = padIt - begin(pads);
                    }
                    
                    const auto paramIt = find_if(begin(parameters), end(parameters), [&] (const auto& v) { return buffer == v; });
                    if(paramIt != end(parameters))
                    {
                        args.arg1 = paramIt - begin(parameters);
                        state = 2;
                    }
                    
                    break;
                }
                case 2:
                {
                    if(type == 2) // if command type is get
                    {
                        state = 3;
                        break;
                    }

                    if(c == '\n')
                    {
                        args.arg2 = ::atoi(value.GetData());
                        state = 3;
                        break;
                    }

                    value.Append(c);

                    break;
                }
            }
        }

        Util::optional<Command> PopCommand()
        {
            if(state != 3)
            {
                return {};
            }

            const auto command = Command{type, args};
            Reset();

            return {command};
        }

    private:

        void Reset()
        {
            type = 0;   
            args = {};    
            buffer.Clear();
            value.Clear();

            state = 0;
        }

        static constexpr uint8_t bufferLength = 32U;

        uint8_t index{};
        uint8_t state{};
        uint8_t type{};
        Arguments args{};

        String<bufferLength> buffer{};
        String<8U> value{};
    };

} // namespace Commands
