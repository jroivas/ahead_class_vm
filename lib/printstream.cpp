#include "printstream.h"
#include <iostream>
#include <vm_stack.h>

PrintStream::PrintStream()
    : vmClass("java/io/PrintStream")
{
    addFunction(new FunctionDesc("<init>", "()V"));
    addFunction(new FunctionDesc("println", "(Ljava/lang/String;)V"));
/*
    setFunction("<init>", [](vmClass *clz, vmStack *st) {
        vmClass *thiz = vmClass::castFrom(st->pop());
        (void)thiz;
        // FIXME use out object
        vmClass *out = vmClass::castFrom(st->pop());
        (void)out;
    });
    setFunction("println", [](vmClass *clz, vmStack *st) {
        vmObject *o = st->pop();
        if (o->type == TYPE_STRING) {
            //vmString *st = dynamic_cast<vmString*>(o);
            std::cout << vmString::castFrom(o)->val;
        } else {
            std::cout << "*** ERROR: Can't print " << typeName(o) << "\n";
        }
        std::cout << "\n";

    });
*/
}

void PrintStream::_init_(vmClassInstance *thiz)
{
}

void PrintStream::println(vmClassInstance *_thiz, vmString *o)
{
    //PrintStreamInstance *thiz = static_cast<PrintStreamInstance *>(_thiz);
    std::cout << o->val << "\n";
}

void PrintStream::println(vmClassInstance *_thiz, ClassLangStringInstance *o)
{
    //PrintStreamInstance *thiz = static_cast<PrintStreamInstance *>(_thiz);
    std::cout << o->val->val << "\n";
}

vmClassInstance *PrintStream::newInstance()
{
    return new PrintStreamInstance(this);
    /*
    vmClass *res = new PrintStream();
    res->baseClass = this;
    return res;
    */
}
