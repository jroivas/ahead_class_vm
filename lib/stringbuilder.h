#pragma once

#include <vm_object.h>

class StringBuilder : public vmClass
{
public:
    StringBuilder();
    virtual ~StringBuilder() {}
    virtual vmClass *newInstance();
};
