#include <langobject.h>
#include <vm_stack.h>

LangObject::LangObject()
    : vmClass("java/lang/Object")
{
    addFunction(new FunctionDesc("<init>", "()V"));
    /*
    setFunction("<init>", [](vmClass *clz, vmStack *st) {
        vmClass *thiz = vmClass::castFrom(st->pop());
        (void)thiz;
    });
    */
}

void LangObject::_init_(vmClassInstance *)
{
}

vmClassInstance *LangObject::newInstance()
{
    return new LangObjectInstance(this);
    /*
    vmClass *res = new LangObject();
    res->baseClass = this;
    return res;
    */
}
