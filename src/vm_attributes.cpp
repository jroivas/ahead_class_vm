#include "vm_attributes.h"

#include <cstring>

#include "utils.h"

vmAttributeInfo::vmAttributeInfo(vmAttributeInfo *orig)
{
    name_index = orig->name_index;
    length = orig->length;
    info = orig->info;

    // FIXME?
    orig->info = nullptr;
}

vmAttributeInfo::~vmAttributeInfo()
{
    if (info) {
        delete [] info;
    }
}

vmAttributeInfo *vmAttributeInfo::parse(uint8_t *p)
{
    vmAttributeInfo *res = new vmAttributeInfo();
    res->name_index = read16(p + 0);
    res->length = read32(p + 2);
    res->size = 6;
    res->info = new uint8_t[res->length];
    memmove(res->info, p + 6, res->length);
    res->size += res->length;
    return res;
}

vmCodeAttribute::vmCodeAttribute(vmAttributeInfo *orig)
    : vmAttributeInfo(orig)
{
    max_stack = read16(info);
    max_locals = read16(info + 2);
    code_length = read32(info + 4);
}
