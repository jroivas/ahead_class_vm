#include "vm_fields.h"
#include <arpa/inet.h>

vmFieldInfo *vmFieldInfo::parse(uint8_t *p)
{
    vmFieldInfo *res = new vmFieldInfo();
    res->access_flags = ntohs(*(uint16_t*)(p + 0));
    res->name_index = ntohs(*(uint16_t*)(p + 2));
    res->desc_index = ntohs(*(uint16_t*)(p + 4));
    res->attributes_count = ntohs(*(uint16_t*)(p + 6));
    res->size = 8;
    for (uint16_t i = 0; i < res->attributes_count; ++i) {
        vmAttributeInfo *dd = vmAttributeInfo::parse(p + res->size);
        res->attributes.push_back(dd);
        res->size += dd->size;
    }

    return res;
}
