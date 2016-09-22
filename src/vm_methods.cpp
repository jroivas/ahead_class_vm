#include "vm_methods.h"
#include "utils.h"

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
    res->access_flags = read16(p + 0);
    res->name_index = read16(p + 2);
    res->desc_index = read16(p + 4);
    res->attributes_count = read16(p + 6);
    res->size = 8;
    for (uint16_t i = 0; i < res->attributes_count; ++i) {
        vmAttributeInfo *dd = vmAttributeInfo::parse(p + res->size);
        res->attributes.push_back(dd);
        res->size += dd->size;
    }
    return res;
}
