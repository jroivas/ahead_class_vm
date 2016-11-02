#pragma once

#include <string>
#include <cstddef>
#include <functional>
#include <map>
#include <unordered_map>
#include <vector>
#include <utility>
#include <iostream>

#include <gc/gc.h>
#include <gc/gc_cpp.h>
#include <gc/gc_allocator.h>

class vmClassFile;
class vmStack;

enum vmType {
    TYPE_INV = 1,
    TYPE_OBJ,
    TYPE_STRING,
    TYPE_INTEGER,
    TYPE_LONG,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_REF,
    TYPE_CLASS
};

typedef int32_t nInteger;
typedef int64_t nLong;
typedef float nFloat;
typedef double nDouble;
typedef std::string nString;

class vmObject
{
public:
    vmObject() : classFile(nullptr), type(TYPE_INV) {}
    vmObject(vmType t) : classFile(nullptr), type(t) {}
    vmObject(vmClassFile *cl) : classFile(cl) {}
    virtual ~vmObject() {}

    vmClassFile *classFile;

    void init() {}
    vmType type;
};

class vmClass;
#include <dynload.h>

std::vector<std::string> parseField(std::string);
std::pair<std::string,std::string> parseParams(std::string);

class FunctionDesc
{
public:
    FunctionDesc(std::function<void(vmClass *, vmStack *)> f) : func(f), init(false) { }
    std::function<void(vmClass *, vmStack *)> func;
    std::string description;
    std::string returnType;
    std::vector<std::string> params;
    bool init;
    void parse(std::string desc) {
        description = desc;
        std::pair<std::string, std::string> rr = parseParams(description);
        params = parseField(rr.first);
        std::vector<std::string> ret = parseField(rr.second);
        if (!ret.empty()) {
            returnType = ret[0];
        }
        init = true;
    }
};

class vmClass : public vmObject
{
public:
    vmClass(std::string n, bool doReg=true) : vmObject(TYPE_CLASS), /* name(n),*/ baseClass(nullptr) {
        if (doReg) {
            name = n;
            registerClass(this);
        }
    }
    virtual ~vmClass() {}
    virtual vmClass *newInstance() = 0;
    bool isBaseClass() { return baseClass == nullptr; }

    static vmClass* castFrom(vmObject* o)
    {
        if (!o) throw "Nullptr";
        return o->type == TYPE_CLASS ? static_cast<vmClass *>(o) : nullptr;
    }

    inline FunctionDesc *getFunction(std::string name)
    {
        if (baseClass != nullptr) {
            return baseClass->getFunction(name);
        }
        return methods[name];
    }

    //FunctionDesc *getFunction(std::string name);
    void setFunction(std::string name, std::function<void(vmClass *, vmStack *)> f) {
        setFunction(name, new FunctionDesc(f));
    }
    void setFunction(std::string name, FunctionDesc *f);

    std::string val;
    std::string name;
    vmClass *baseClass;
    std::unordered_map<std::string, FunctionDesc*> methods;
};

class SystemPrinter : public vmClass
{
public:
    // FIXME
    SystemPrinter() : vmClass("SystemPrinter") {}
    virtual ~SystemPrinter() {}
    virtual vmClass *newInstance() {
        SystemPrinter* p = new SystemPrinter();
        p->baseClass = this;
        return p;
    }
};

class vmString : public vmObject
{
public:
    vmString() : vmObject(TYPE_STRING), val("") {}
    vmString(std::string v) : vmObject(TYPE_STRING), val(v) {}
    std::string val;
    static vmString* castFrom(vmObject* o)
    {
        if (!o) throw "Nullptr";
        return o->type == TYPE_STRING ? static_cast<vmString *>(o) : nullptr;
    }
};

class vmLocal : public vmObject
{
public:
    vmLocal() : vmObject(TYPE_INTEGER), val_long(0) {}
    union {
        nInteger val_int;
        nLong val_long;
        nFloat val_float;
        nDouble val_double;
        vmObject *val_obj;
    };
};

class vmInteger : public vmObject
{
public:
    vmInteger() : vmObject(TYPE_INTEGER), val(0) {}
    vmInteger(uint32_t v) : vmObject(TYPE_INTEGER), val(v) {}
    uint32_t val;
    static vmInteger* castFrom(vmObject* o)
    {
        if (!o) throw "Nullptr";
        return o->type == TYPE_INTEGER ? static_cast<vmInteger *>(o) : nullptr;
    }
};

class vmLong : public vmObject
{
public:
    vmLong() : vmObject(TYPE_LONG), val(0) {}
    vmLong(uint64_t v) : vmObject(TYPE_LONG), val(v) {}
    uint64_t val;
    static vmLong* castFrom(vmObject* o)
    {
        if (!o) throw "Nullptr";
        return o->type == TYPE_LONG ? static_cast<vmLong *>(o) : nullptr;
    }
};

class vmFloat : public vmObject
{
public:
    vmFloat() : vmObject(TYPE_FLOAT), val(0.0) {}
    vmFloat(float v) : vmObject(TYPE_FLOAT), val(v) {}
    float val;
    static vmFloat* castFrom(vmObject* o)
    {
        if (!o) throw "Nullptr";
        return o->type == TYPE_FLOAT ? static_cast<vmFloat *>(o) : nullptr;
    }
};

class vmDouble : public vmObject
{
public:
    vmDouble() : vmObject(TYPE_DOUBLE), val(0.0) {}
    vmDouble(float v) : vmObject(TYPE_DOUBLE), val(v) {}
    double val;
    static vmDouble* castFrom(vmObject* o)
    {
        if (!o) throw "Nullptr";
        return o->type == TYPE_DOUBLE ? static_cast<vmDouble *>(o) : nullptr;
    }
};

class vmRef : public vmObject
{
public:
    vmRef(vmObject *v) : vmObject(TYPE_REF), val(v) {}
    vmObject *val;
};

std::string typeName(vmObject *o);

