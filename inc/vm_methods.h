#pragma once

#include <vector>
#include <cstdint>
#include "vm_attributes.h"

class vmMethodInfo
{
protected:
    vmMethodInfo();

public:
    static vmMethodInfo *parse(uint8_t *p);

    uint16_t access_flags;
    uint16_t name_index;
    uint16_t desc_index;
    uint16_t attributes_count;
    std::vector<vmAttributeInfo*> attributes;
    uint32_t size;
};
