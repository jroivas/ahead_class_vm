#include "stringbuilder.h"
#include <iostream>
#include <vm_stack.h>

StringBuilder::StringBuilder()
    : vmClass("java/lang/StringBuilder")
{
    setFunction("<init>", [](vmStack *) {});
    setFunction("append", [&](vmStack *st) {
        vmObject *o = st->pop();
        if (o->type == TYPE_STRING) {
            vmString *st = dynamic_cast<vmString*>(o);
            val += st->val;
        } else if (o->type == TYPE_LONG) {
            vmLong *v = dynamic_cast<vmLong*>(o);
            val += std::to_string(v->val);
        } else if (o->type == TYPE_DOUBLE) {
            vmDouble *v = dynamic_cast<vmDouble*>(o);
            val += std::to_string(v->val);
        } else {
            std::cout << "*** ERROR: Can't append " << typeName(o) << "\n";
        }
        st->push(this);
    });
    setFunction("toString", [&](vmStack *st) {
        st->push(new vmString(val));
    });
}

vmClass *StringBuilder::newInstance()
{
    vmClass *res = new StringBuilder();
    res->baseClass = this;
    return res;
}
