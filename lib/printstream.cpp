#include "printstream.h"
#include <iostream>
#include <vm_stack.h>

PrintStream::PrintStream()
    : vmClass("java/io/PrintStream")
{
    setFunction("<init>", [&](vmStack *st) { st->push(this); });
    setFunction("println", [](vmStack *st) {
        vmObject *o = st->pop();
        if (o->type == TYPE_STRING) {
            vmString *st = dynamic_cast<vmString*>(o);
            std::cout << st->val;
        } else {
            std::cout << "*** ERROR: Can't print " << typeName(o) << "\n";
        }
        std::cout << "\n";

    });
    /*
    setFunction("append", [&](vmStack *st) {
            vmObject *o = st->pop();
            if (o->type == TYPE_STRING) {
                vmString *st = dynamic_cast<vmString*>(o);
                val += st->val;
            } else if (o->type == TYPE_LONG) {
                vmLong *st = dynamic_cast<vmLong*>(o);
                val += std::to_string(st->val);
            } else {
                std::cout << "*** ERROR: Can't append " << typeName(o) << "\n";
            }
        st->push(this);
    });
    setFunction("toString", [&](vmStack *st) {
        st->push(new vmString(val));
    });
    */
}

vmClass *PrintStream::newInstance()
{
    vmClass *res = new PrintStream();
    res->baseClass = this;
    return res;
}
