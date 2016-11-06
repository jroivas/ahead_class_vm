#pragma once

#include <vm_object.h>

class LangObjectInstance : public vmClassInstance
{
public:
    LangObjectInstance(vmClass *base) : vmClassInstance(base) {}
};

class LangObject : public vmClass
{
public:
    LangObject();
    virtual ~LangObject() {}
    virtual vmClassInstance *newInstance();

    void _init_(vmClassInstance *);
};
