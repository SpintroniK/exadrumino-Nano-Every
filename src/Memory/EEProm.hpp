#pragma once

#include <avr/eeprom.h>

namespace Memory
{
    template <typename T>
    class EEPromValue
    {        
    public:
        explicit EEPromValue(T* val) : ptr{val}
        {
            static_assert(sizeof(T) == sizeof(uint8_t), "Error EEPROM implementation only supports bytes.");
        }

        EEPromValue() : ptr{nullptr}
        {

        }

        explicit EEPromValue(EEPromValue<T>&& other)
        {
            ptr = other.ptr;
            other.ptr = nullptr;
        }

        ~EEPromValue() = default;

        EEPromValue(const EEPromValue<T>& other)
        {
            ptr = other.ptr;
        }

        EEPromValue<T> operator=(const EEPromValue<T>& other)
        {
            ptr = other.ptr;
            return *this;
        }

        EEPromValue<T> operator=(EEPromValue<T>&& other)
        {
            ptr = other.ptr;
            return *this;
        }

        EEPromValue<T> operator=(const T& val)
        {
            eeprom_update_byte(ptr, val);
            return *this;
        }

        operator const T() const
        {
            return eeprom_read_byte(ptr);
        }

    private:

        const T* ptr;
    };
    
} // namespace Memory
