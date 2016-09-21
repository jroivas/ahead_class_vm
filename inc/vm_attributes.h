#pragma once

#include <cstdint>

class vmAttributeInfo
{
protected:
    vmAttributeInfo() {}
public:
    static vmAttributeInfo *parse(uint8_t *p);

    uint16_t name_index;
    uint32_t length;
    uint8_t *info;
    uint32_t size;
};


