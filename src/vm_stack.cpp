#include "vm_stack.h"

vmStack::vmStack()
{
}

vmStack::vmStack(uint32_t s)
    : size(s)
    //: stack(size)
{
}

void vmStack::push(vmObject *obj)
{
    stack.push_back(obj);
}

vmObject *vmStack::pop()
{
    vmObject *tmp = stack.back();
    stack.pop_back();
    return tmp;
}
