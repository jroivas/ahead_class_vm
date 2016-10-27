#pragma once

#include <string>
#include <cstddef>
#include <functional>
#include <map>
#include <vector>
#include <utility>

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
void registerClass(vmClass *cl);
std::vector<std::string> parseField(std::string);
std::pair<std::string,std::string> parseParams(std::string);

class vmClass : public vmObject
{
public:
    vmClass(std::string n) : vmObject(TYPE_CLASS), name(n), baseClass(nullptr) {
        registerClass(this);
    }
    virtual ~vmClass() {}
    virtual vmClass *newInstance() = 0;
    bool isBaseClass() { return baseClass == nullptr; }

    std::function<void(vmStack *)> getFunction(std::string name);
    void setFunction(std::string name, std::function<void(vmStack *)> f) {
        methods[name] = f;
    }

    std::string name;
    vmClass *baseClass;
    std::map<std::string, std::function<void(vmStack *)>> methods;
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
    vmString(std::string v) : vmObject(TYPE_STRING), val(v) {}
    std::string val;
};

class vmInteger : public vmObject
{
public:
    vmInteger(uint32_t v) : vmObject(TYPE_INTEGER), val(v) {}
    uint32_t val;
};

class vmLong : public vmObject
{
public:
    vmLong(uint64_t v) : vmObject(TYPE_LONG), val(v) {}
    uint64_t val;
};

class vmFloat : public vmObject
{
public:
    vmFloat(float v) : vmObject(TYPE_FLOAT), val(v) {}
    float val;
};

class vmDouble : public vmObject
{
public:
    vmDouble(float v) : vmObject(TYPE_DOUBLE), val(v) {}
    double val;
};

class vmRef : public vmObject
{
public:
    vmRef(vmObject *v) : vmObject(TYPE_REF), val(v) {}
    vmObject *val;
};

std::string typeName(vmObject *o);

