#pragma once

#include <vm_object.h>

class StringBuilderInstance : public vmClassInstance
{
public:
    StringBuilderInstance(vmClass *base) : vmClassInstance(base) {}

    //std::string val;
    vmString val;
};

class ClassLangStringInstance : public vmClassInstance
{
public:
    ClassLangStringInstance(vmClass *base) : vmClassInstance(base) {}

    vmString *val;
};

class StringBuilder : public vmClass
{
public:
    StringBuilder();
    //StringBuilder(bool base=true);
    virtual ~StringBuilder() {}
    virtual vmClassInstance *newInstance();

    //std::string val;
    void _init_(vmClassInstance *);
    vmClassInstance *append(vmClassInstance *, ClassLangStringInstance*);
    vmClassInstance *append(vmClassInstance *, vmString*);
    vmClassInstance *append(vmClassInstance *, vmLong*);
    vmClassInstance *append(vmClassInstance *, vmDouble*);
    vmString *toString(vmClassInstance *);
};

class ClassLangString : public vmClass
{
public:
    ClassLangString();
    virtual ~ClassLangString() {}
    virtual vmClassInstance *newInstance();

    void _init_(vmClassInstance *thiz);
    ClassLangStringInstance *replaceAll(vmClassInstance *thiz, vmString *hey, vmString *rpl);
    //vmString *replaceAll(vmClassInstance *thiz, /*vmString *thiz,*/ vmString *hey, vmString *rpl);
    //std::string val;
};
