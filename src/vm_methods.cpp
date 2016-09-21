#include "vm_methods.h"
#include <cstring>
#include <arpa/inet.h>

vmMethodInfo::vmMethodInfo()
    : access_flags(0),
    name_index(0),
    desc_index(0),
    attributes_count(0)
{
}

vmMethodInfo *vmMethodInfo::parse(uint8_t *p)
{
    vmMethodInfo *res = new vmMethodInfo();
    res->access_flags = ntohs(*(uint16_t*)(p + 0));
    res->name_index = ntohs(*(uint16_t*)(p + 2));
    res->desc_index = ntohs(*(uint16_t*)(p + 4));
    res->attributes_count = ntohl(*(uint16_t*)(p + 6));
    res->size = 8;
    for (int16_t i = 0; i < res->attributes_count - 1; ++i) {
        vmAttributeInfo *att = vmAttributeInfo::parse(p + res->size);
        res->size += att->size;
        res->attributes.push_back(att);
    }
    return res;
}

vmAttributeInfo *vmAttributeInfo::parse(uint8_t *p)
{
    vmAttributeInfo *res = new vmAttributeInfo();
    res->name_length = ntohs(*(uint16_t*)(p + 0));
    res->length = ntohl(*(uint16_t*)(p + 2));
    res->size = 6;
    res->info = new uint8_t[res->length + 1];
    memmove(res->info, p + 6, res->length);
    res->size += res->length;
    return res;
}
