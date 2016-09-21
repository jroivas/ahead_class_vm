#include "vm_constants.h"
#include <iostream>
#include <cstring>

#include <arpa/inet.h>
#include <endian.h>

vmConstantInfo *vmConstantInfo::parse(uint8_t *p)
{
    if (!p) return nullptr;
    vmConstants tag = (vmConstants)*p;

    switch (tag) {
        case C_Utf8:
            return new vmConstantUtf8(tag,
                ntohs(*(uint16_t*)(p + 1)),
                (const uint8_t*)(p + 3)
            );
        case C_Integer:
            return new vmConstantInteger(tag,
                ntohl(*(uint32_t*)(p + 1))
            );
        case C_Float:
            return new vmConstantFloat(tag,
                ntohl(*(uint32_t*)(p + 1))
            );
        case C_Long:
            return new vmConstantLong(tag,
                be64toh(*(uint64_t*)(p + 1))
            );
        case C_Double:
            return new vmConstantDouble(tag,
                be64toh(*(uint64_t*)(p + 1))
            );
        case C_Class:
            return new vmConstantClass(tag, ntohs(*(uint16_t*)(p + 1)));
        case C_String:
            return new vmConstantString(tag,
                ntohs(*(uint16_t*)(p + 1))
            );
        case C_FieldRef:
            return new vmConstantFieldRef(tag,
                ntohs(*(uint16_t*)(p + 1)),
                ntohs(*(uint16_t*)(p + 3))
            );
        case C_MethodRef:
            return new vmConstantMethodRef(tag,
                ntohs(*(uint16_t*)(p + 1)),
                ntohs(*(uint16_t*)(p + 3))
            );
        case C_InterfaceMethodRef:
            return new vmConstantInterfaceMethodRef(tag,
                ntohs(*(uint16_t*)(p + 1)),
                ntohs(*(uint16_t*)(p + 3))
            );
        case C_NameAndType:
            return new vmConstantNameAndType(tag,
                ntohs(*(uint16_t*)(p + 1)),
                ntohs(*(uint16_t*)(p + 3))
            );
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
    bytes = new uint8_t [length + 1];
    memset(bytes, 0, length + 1);
    memmove(bytes, b, length);
}

vmConstantUtf8::~vmConstantUtf8()
{
    delete [] bytes;
}
