#pragma once

#include <string>
#include <cstddef>

class vmClassFile;

class vmObject
{
public:
    vmObject() : classFile(nullptr) {}
    vmObject(vmClassFile *cl) : classFile(cl) {}
    virtual ~vmObject() {}

    vmClassFile *classFile;

    void init() {}
};

class SystemPrinter : public vmObject
{
public:
    SystemPrinter() : vmObject() {}
};

class vmString : public vmObject
{
public:
    vmString(std::string v) : vmObject(), val(v) {}
    std::string val;
};

class vmInteger : public vmObject
{
public:
    vmInteger(uint32_t v) : vmObject(), val(v) {}
    uint32_t val;
};

class vmLong : public vmObject
{
public:
    vmLong(uint64_t v) : vmObject(), val(v) {}
    uint64_t val;
};

class vmFloat : public vmObject
{
public:
    vmFloat(float v) : vmObject(), val(v) {}
    float val;
};

class vmDouble : public vmObject
{
public:
    vmDouble(float v) : vmObject(), val(v) {}
    double val;
};

