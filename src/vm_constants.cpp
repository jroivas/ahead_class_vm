#include "vm_constants.h"
#include <arpa/inet.h>
#include <iostream>
#include <cstring>

vmConstantInfo *vmConstantInfo::parse(uint8_t *p)
{
    if (!p) return nullptr;
    vmConstants tag = (vmConstants)*p;
    //std::cout << "Const " << (uint32_t)tag << "\n";

    switch (tag) {
        case C_Utf8:
            return new vmConstantUtf8(tag,
                ntohs(*(uint16_t*)(p + 1)),
                (const uint8_t*)(p + 3)
            );
        case C_Integer:
            break;
        case C_Float:
            break;
        case C_Long:
            break;
        case C_Double:
            break;
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
            std::cout << "Invalid constant info\n";
            throw "Invalid constant info";
    }
    return nullptr;
}

vmConstantUtf8::vmConstantUtf8(vmConstants tag, uint16_t l, const uint8_t *b)
    : vmConstantInfo(tag)
{
    length = l;
    size = l + 3;
    bytes = new uint8_t [length + 1];
    memmove(bytes, b, length);
}

vmConstantUtf8::~vmConstantUtf8()
{
    delete [] bytes;
}
