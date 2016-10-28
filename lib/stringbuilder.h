#pragma once

#include <vm_object.h>

class StringBuilder : public vmClass
{
public:
    StringBuilder();
    virtual ~StringBuilder() {}
    virtual vmClass *newInstance();

    std::string val;
};

class ClassLangString : public vmClass
{
public:
    ClassLangString();
    virtual ~ClassLangString() {}
    virtual vmClass *newInstance();

    std::string val;
};
