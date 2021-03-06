#include "vm_constants.h"
#include <iostream>
#include <cstring>
#include <cmath>

#include "utils.h"

vmConstantInfo* vmConstantInfo::resolve(std::vector<vmConstantInfo*> &)
{
    return nullptr;
}
vmConstantInfo* vmConstantInfo::resolve2(std::vector<vmConstantInfo*> &)
{
    return nullptr;
}

vmConstantInfo* vmConstantClass::resolve(std::vector<vmConstantInfo*> &info)
{
    return info[index];
}

vmConstantInfo* vmConstantRef::resolve(std::vector<vmConstantInfo*> &info)
{
    return info[index];
}

vmConstantInfo* vmConstantRef::resolve2(std::vector<vmConstantInfo*> &info)
{
    return info[nameTypeIndex];
}

vmConstantInfo *vmConstantInfo::parse(uint8_t *p)
{
    if (!p) return nullptr;
    vmConstants tag = (vmConstants)*p;

    //std::cout << "tag " << tag << " ";

    switch (tag) {
        case C_Utf8:
            return new vmConstantUtf8(tag, read16(p + 1), (const uint8_t*)(p + 3));
        case C_Integer:
            return new vmConstantInteger(tag, read32(p + 1));
        case C_Float:
            return new vmConstantFloat(tag, read32(p + 1));
        case C_Long:
            return new vmConstantLong(tag, read64(p + 1));
        case C_Double:
            return new vmConstantDouble(tag, read64(p + 1));
        case C_Class:
            return new vmConstantClass(tag, read16(p + 1));
        case C_String:
            return new vmConstantString(tag, read16(p + 1));
        case C_FieldRef:
            return new vmConstantFieldRef(tag, read16(p + 1), read16(p + 3));
        case C_MethodRef:
            return new vmConstantMethodRef(tag, read16(p + 1), read16(p + 3));
        case C_InterfaceMethodRef:
            return new vmConstantInterfaceMethodRef(tag, read16(p + 1), read16(p + 3));
        case C_NameAndType:
            return new vmConstantNameAndType(tag, read16(p + 1), read16(p + 3));
        case C_MethodHandle:
            break;
        case C_MethodType:
            break;
        case C_InvokeDynamic:
            break;
        default:
            std::cout << "Invalid constant info: " << tag << "\n";
            throw "Invalid constant info";
    }
    std::cout << "Unimplemented constant info: " << tag << "\n";
    return nullptr;
}

vmConstantUtf8::vmConstantUtf8(vmConstants tag, uint16_t l, const uint8_t *b)
    : vmConstantInfo(tag)
{
    length = l;
    size = l + 3;
    uint8_t *bytes = new uint8_t [length + 1];
    memset(bytes, 0, length + 1);
    memmove(bytes, b, length);
    val = vmString(std::string((char*)bytes));
    delete [] bytes;
    //std::cout << " SS " << l << " ";
    //std::cout << " " << bytes << " ";
}

vmConstantUtf8::~vmConstantUtf8()
{
    //delete [] bytes;
}
