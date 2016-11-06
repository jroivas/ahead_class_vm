#pragma once

#include <vm_object.h>
#include "stringbuilder.h"

class PrintStreamInstance : public vmClassInstance
{
public:
    PrintStreamInstance(vmClass *base) : vmClassInstance(base) {}
};

class PrintStream : public vmClass
{
public:
    PrintStream();
    virtual ~PrintStream() {}
    //virtual vmClass *newInstance();
    virtual vmClassInstance *newInstance();

    void _init_(vmClassInstance *thiz);
    void println(vmClassInstance *thiz, vmString *o);
    void println(vmClassInstance *thiz, ClassLangStringInstance *o);
};
