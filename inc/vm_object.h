#pragma once

#include <string>
#include <cstddef>

class vmClassFile;

enum vmType {
    TYPE_INV = 1,
    TYPE_OBJ,
    TYPE_STRING,
    TYPE_INTEGER,
    TYPE_LONG,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_REF,
    TYPE_STATIC
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

class SystemPrinter : public vmObject
{
public:
    SystemPrinter() : vmObject(TYPE_STATIC) {}
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

