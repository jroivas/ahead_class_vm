#include "stringbuilder.h"
#include <iostream>
#include <algorithm>
#include <vm_stack.h>

StringBuilder::StringBuilder()
    : vmClass("java/lang/StringBuilder")
{
    setFunction("<init>", [&](vmStack *st) {
        this->val = "";
        st->push(this);
    });
    setFunction("append", [&](vmStack *st) {
        vmObject *o = st->pop();
        /*vmObject *t = st->pop();
        vmString* obj = vmString::castFrom(t);
        if (!obj) {
            std::cout << "*** ERROR: invalid obj target " << typeName(t) << "\n";
            throw "*** ERROR: Invalid obj target " + typeName(t);
        }
        */
        if (o->type == TYPE_STRING) {
            val += vmString::castFrom(o)->val;
        } else if (o->type == TYPE_LONG) {
            val += std::to_string(vmLong::castFrom(o)->val);
        } else if (o->type == TYPE_DOUBLE) {
            val += std::to_string(vmDouble::castFrom(o)->val);
        } else if (o->type == TYPE_INTEGER) {
            val += std::to_string(vmInteger::castFrom(o)->val);
        } else {
            std::cout << "*** ERROR: Can't append " << typeName(o) << "\n";
            throw "*** ERROR: Can't append " + typeName(o);
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

ClassLangString::ClassLangString()
    : vmClass("java/lang/String")
{
    setFunction("<init>", [&](vmStack *st) { st->push(this); });
    setFunction("replaceAll", [&](vmStack *st) {
        vmString* rpl = vmString::castFrom(st->pop());
        vmString* hey = vmString::castFrom(st->pop());
        vmString* obj = vmString::castFrom(st->pop());
        if (obj && hey && rpl) {
            /*
            std::cout << " H " << obj->val << " : " << hey->val << " " << rpl->val << "\n";
            std::replace(val.begin(), val.end(), hey->val, rpl->val);
            */
            size_t start_pos = 0;
            while ((start_pos = obj->val.find(hey->val, start_pos)) != std::string::npos) {
                obj->val.replace(start_pos, hey->val.length(), rpl->val);
                start_pos += rpl->val.length();
            }
            //std::cout << " P " << obj->val << "\n";
            st->push(obj);
        } else {
            std::cout << "*** ERROR: Can't replaceAll, wrong arguments" << "\n";
            throw "*** ERROR: Can't replaceAll, wrong arguments";
        }
    });
}

vmClass *ClassLangString::newInstance()
{
    vmClass *res = new ClassLangString();
    res->baseClass = this;
    return res;
}
