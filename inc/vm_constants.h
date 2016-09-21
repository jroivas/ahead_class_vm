#pragma once

#include <cstdint>

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
    vmConstants tag;
    uint32_t size;

    static vmConstantInfo *parse(uint8_t *p);
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
        descriptor = v;
        size = 5;
    }
    uint16_t index;
    uint16_t descriptor;
};
