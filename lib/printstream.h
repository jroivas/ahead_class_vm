#pragma once

#include <vm_object.h>

class PrintStream : public vmClass
{
public:
    PrintStream();
    virtual ~PrintStream() {}
    virtual vmClass *newInstance();
};
