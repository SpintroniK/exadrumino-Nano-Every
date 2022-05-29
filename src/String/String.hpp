#pragma once

#include <stdint.h>
#include <string.h>

template <uint8_t length>
class String
{
public:

    auto GetData() const { return &buffer[0]; }

    void Append(char c)
    {
        buffer[index++] = c;
    }

    void Clear()
    {
        for(uint8_t i = 0; i < index; ++i)
        {
            buffer[i] = '\0';
        }

        index = 0;
    }

    bool operator ==(const char* str) const
    {
        return ::strcmp(str, GetData()) == 0;
    }

private:

    char buffer[length]{};
    uint8_t index{};
    uint8_t size{length};
};