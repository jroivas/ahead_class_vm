#include "vm_exceptions.h"
#include "utils.h"

vmExceptionInfo *vmExceptionInfo::parse(uint8_t *p)
{
    vmExceptionInfo *res = new vmExceptionInfo();
    res->start_pc = read16(p);
    res->end_pc = read16(p + 2);
    res->handler_pc = read16(p + 4);
    res->catch_type = read16(p + 6);
    res->size = 8;
    return res;
}
