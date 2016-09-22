#pragma once

#include <cstdint>
#include <vector>
#include "vm_object.h"

class vmStack
{
public:
    vmStack();
    vmStack(uint32_t);

    void push(vmObject *obj);
    vmObject *pop();

    uint32_t size;
    std::vector<vmObject*> stack;
};