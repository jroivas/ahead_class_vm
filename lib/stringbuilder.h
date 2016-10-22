#pragma once

#include <vm_object.h>

class StringBuilder : public vmClass
{
public:
    StringBuilder() : vmClass("java/lang/StringBuilder") {}
    virtual ~StringBuilder() {}
    virtual vmClass *newInstance();
};
