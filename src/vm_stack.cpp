#include "vm_stack.h"
#include <iostream>

vmStack::vmStack()
    : size(0)
{
}

vmStack::vmStack(uint32_t s)
    : size(s)
    //: stack(size)
{
}

void vmStack::push(vmObject *obj)
{
    ++size;
    std::cout << " >> PSH " << obj << " " << typeName(obj) << " SZ " << size << "\n";
    stack.push_back(obj);
}

vmObject *vmStack::pop()
{
    vmObject *tmp = stack.back();
    --size;
    std::cout << " << POP " << tmp << " " << typeName(tmp) <<  " SZ " << size << "\n";
    stack.pop_back();
    return tmp;
}
