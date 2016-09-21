#include "vm_attributes.h"
#include <cstring>
#include <iostream>
#include <arpa/inet.h>

vmAttributeInfo *vmAttributeInfo::parse(uint8_t *p)
{
    vmAttributeInfo *res = new vmAttributeInfo();
    res->name_index = ntohs(*(uint16_t*)(p + 0));
    res->length = ntohl(*(uint32_t*)(p + 2));
    res->size = 6;
    res->info = new uint8_t[res->length];
    memmove(res->info, p + 6, res->length);
    res->size += res->length;
    return res;
}
