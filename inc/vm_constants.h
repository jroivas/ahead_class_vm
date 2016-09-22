#pragma once

#include <cstdint>
#include <vector>

enum vmConstants {
    C_Utf8 = 1,
    C_Integer = 3,
    C_Float = 4,
    C_Long = 5,
    C_Double = 6,
    C_Class = 7,
    C_String = 8,
    C_FieldRef = 9,
    C_MethodRef = 10,
    C_InterfaceMethodRef = 11,
    C_NameAndType = 12,
    C_MethodHandle = 15,
    C_MethodType = 16,
    C_InvokeDynamic = 18
};

class vmConstantInfo
{
protected:
    vmConstantInfo(vmConstants tag) {}

public:
    virtual ~vmConstantInfo() {}
    vmConstants tag;
    uint32_t size;

    static vmConstantInfo *parse(uint8_t *p);
    virtual vmConstantInfo *resolve(std::vector<vmConstantInfo*> &);
    virtual vmConstantInfo *resolve2(std::vector<vmConstantInfo*> &);
};

class vmConstantClass : public vmConstantInfo
{
public:
    vmConstantClass(vmConstants tag, uint16_t v) : vmConstantInfo(tag)
    {
        index = v;
        size = 3;
    }
    uint16_t index;
    virtual vmConstantInfo *resolve(std::vector<vmConstantInfo*> &);
};

class vmConstantRef : public vmConstantInfo
{
public:
    vmConstantRef(vmConstants tag, uint16_t v, uint16_t n) : vmConstantInfo(tag)
    {
        index = v;
        nameTypeIndex = n;
        size = 5;
    }
    uint16_t index;
    uint16_t nameTypeIndex;
    virtual vmConstantInfo *resolve(std::vector<vmConstantInfo*> &);
    virtual vmConstantInfo *resolve2(std::vector<vmConstantInfo*> &);
};

class vmConstantMethodRef : public vmConstantRef
{
public:
    vmConstantMethodRef(vmConstants tag, uint16_t v, uint16_t n) : vmConstantRef(tag, v, n) {}
};

class vmConstantFieldRef : public vmConstantRef
{
public:
    vmConstantFieldRef(vmConstants tag, uint16_t v, uint16_t n) : vmConstantRef(tag, v, n) {}
};

class vmConstantInterfaceMethodRef : public vmConstantRef
{
public:
    vmConstantInterfaceMethodRef(vmConstants tag, uint16_t v, uint16_t n) : vmConstantRef(tag, v, n) {}
};

class vmConstantString : public vmConstantInfo
{
public:
    vmConstantString(vmConstants tag, uint16_t v) : vmConstantInfo(tag)
    {
        index = v;
        size = 3;
    }
    uint16_t index;
};

class vmConstantUtf8 : public vmConstantInfo
{
public:
    vmConstantUtf8(vmConstants tag, uint16_t, const uint8_t *);
    ~vmConstantUtf8();
    uint16_t length;
    uint8_t *bytes;
};

class vmConstantNameAndType : public vmConstantInfo
{
public:
    vmConstantNameAndType(vmConstants tag, uint16_t v, uint16_t d) : vmConstantInfo(tag)
    {
        index = v;
        descriptor = d;
        size = 5;
    }
    uint16_t index;
    uint16_t descriptor;

    virtual vmConstantInfo *resolve(std::vector<vmConstantInfo*> &info) { return info[index]; }
    virtual vmConstantInfo *resolve2(std::vector<vmConstantInfo*> &info) { return info[descriptor]; }
};

class vmConstantInteger : public vmConstantInfo
{
public:
    vmConstantInteger(vmConstants tag, uint32_t v) : vmConstantInfo(tag)
    {
        val = v;
        size = 5;
    }
    uint32_t val;
};

class vmConstantFloat : public vmConstantInfo
{
public:
    vmConstantFloat(vmConstants tag, uint32_t v) : vmConstantInfo(tag)
    {
        val = (float)v;
        size = 5;
    }
    float val;
};

class vmConstantLong : public vmConstantInfo
{
public:
    vmConstantLong(vmConstants tag, uint64_t v) : vmConstantInfo(tag)
    {
        val = v;
        size = 9;
    }
    uint64_t val;
};

class vmConstantDouble : public vmConstantInfo
{
public:
    vmConstantDouble(vmConstants tag, uint64_t v) : vmConstantInfo(tag)
    {
        val = (double)v;
        size = 9;
    }
    double val;
};
