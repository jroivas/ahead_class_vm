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
    void insert(vmObject *obj);
    vmObject *pop();
    vmObject *peek();

    uint32_t size;
private:
    std::vector<vmObject*> stack;
};
