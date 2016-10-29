#include "printstream.h"
#include <iostream>
#include <vm_stack.h>

PrintStream::PrintStream()
    : vmClass("java/io/PrintStream")
{
    setFunction("<init>", [&](vmStack *st) { });
    setFunction("println", [](vmStack *st) {
        vmObject *o = st->pop();
        if (o->type == TYPE_STRING) {
            //vmString *st = dynamic_cast<vmString*>(o);
            std::cout << vmString::castFrom(o)->val;
        } else {
            std::cout << "*** ERROR: Can't print " << typeName(o) << "\n";
        }
        std::cout << "\n";

    });
}

vmClass *PrintStream::newInstance()
{
    vmClass *res = new PrintStream();
    res->baseClass = this;
    return res;
}
