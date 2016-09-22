#pragma once

#include <cstdint>

class vmExceptionInfo
{
protected:
    vmExceptionInfo() {}
public:
    static vmExceptionInfo *parse(uint8_t *);

    uint16_t start_pc;
    uint16_t end_pc;
    uint16_t handler_pc;
    uint16_t catch_type;
    uint32_t size;
};
