#include "stringbuilder.h"
#include <iostream>
#include <algorithm>
#include <vm_stack.h>

StringBuilder::StringBuilder()
    : vmClass("java/lang/StringBuilder")
{
    addFunction(new FunctionDesc("<init>", "()V"));
    addFunction(new FunctionDesc("append", "(Ljava/lang/String;)Ljava/lang/StringBuilder;"));
    addFunction(new FunctionDesc("append", "(J)Ljava/lang/StringBuilder;"));
    addFunction(new FunctionDesc("append", "(D)Ljava/lang/StringBuilder;"));
    addFunction(new FunctionDesc("toString", "()Ljava/lang/String;"));
#if 0
    //if (base) {
    setFunction("<init>", [](vmClass *clz, vmStack *st) {
        vmClass *thiz = vmClass::castFrom(st->pop());
        thiz->val = "";
        //st->push(thiz);
    });

    /*
    FunctionDesc *f = new FunctionDesc([&](vmStack *st) {
        vmObject *o = st->pop();
        / *vmObject *t = st->pop();
        vmString* obj = vmString::castFrom(t);
        if (!obj) {
            std::cout << "*** ERROR: invalid obj target " << typeName(t) << "\n";
            throw "*** ERROR: Invalid obj target " + typeName(t);
        }
        * /
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
    f->parse("(Ljava/lang/String;)Ljava/lang/StringBuilder;");
    setFunction("append", f);
    */

    setFunction("append", [](vmClass *clz, vmStack *st) {
        vmObject *o = st->pop();
        vmClass *thiz = vmClass::castFrom(st->pop());
        if (o->type == TYPE_STRING) {
            thiz->val += vmString::castFrom(o)->val;
        } else if (o->type == TYPE_LONG) {
            thiz->val += std::to_string(vmLong::castFrom(o)->val);
        } else if (o->type == TYPE_DOUBLE) {
            thiz->val += std::to_string(vmDouble::castFrom(o)->val);
        } else if (o->type == TYPE_INTEGER) {
            thiz->val += std::to_string(vmInteger::castFrom(o)->val);
        } else {
            std::cout << "*** ERROR: Can't append " << typeName(o) << "\n";
            throw "*** ERROR: Can't append " + typeName(o);
        }
        st->push(thiz);
    });
    setFunction("toString", [](vmClass *clz, vmStack *st) {
        vmClass *thiz = vmClass::castFrom(st->pop());
        //std::cout << " THIS " << thiz << " " << thiz->val << "\n";
        st->push(new vmString(thiz->val));
    });
    //}
#endif
}

void StringBuilder::_init_(vmClassInstance *_thiz)
{
    StringBuilderInstance *thiz = dynamic_cast<StringBuilderInstance *>(_thiz);
    if (!thiz) {
        throw "ERR: Wrong type: " + typeName(_thiz);
    }
    thiz->val.val = "";
}

vmClassInstance *StringBuilder::append(vmClassInstance *_thiz, vmString *s)
{
    StringBuilderInstance *thiz = static_cast<StringBuilderInstance *>(_thiz);
    thiz->val.val += s->val;
    return thiz;
}

vmClassInstance *StringBuilder::append(vmClassInstance *_thiz, vmLong*o)
{
    StringBuilderInstance *thiz = static_cast<StringBuilderInstance *>(_thiz);
    thiz->val.val += std::to_string(o->val);
    return thiz;
}

vmClassInstance *StringBuilder::append(vmClassInstance *_thiz, vmDouble*o)
{
    StringBuilderInstance *thiz = static_cast<StringBuilderInstance *>(_thiz);
    thiz->val.val += std::to_string(o->val);
    return thiz;
}

vmString *StringBuilder::toString(vmClassInstance *_thiz)
{
    StringBuilderInstance *thiz = static_cast<StringBuilderInstance *>(_thiz);
    return &thiz->val;
    //return new vmString(thiz->val);
}


vmClassInstance *StringBuilder::newInstance()
{
    return new StringBuilderInstance(this);
    /*
    vmClass *res = new StringBuilderInstance(false);
    vmClass *b = this;
    while (b->baseClass != nullptr) {
        b = b->baseClass;
    }
    res->baseClass = b;
    return res;
    */
}

ClassLangString::ClassLangString()
    : vmClass("java/lang/String")
{
    addFunction(new FunctionDesc("<init>", "()V"));
    addFunction(new FunctionDesc("append", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;"));
#if 0
    setFunction("<init>", [](vmClass *clz, vmStack *st) {
        vmClass *thiz = vmClass::castFrom(st->pop());
        (void)thiz;
    });
    setFunction("replaceAll", [](vmClass *thiz, vmStack *st) {
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
#endif
}

void ClassLangString::_init_(vmClassInstance *_thiz)
{
    ClassLangStringInstance *thiz = static_cast<ClassLangStringInstance *>(_thiz);
    thiz->val = new vmString();
}

ClassLangStringInstance *ClassLangString::replaceAll(vmClassInstance *_thiz, vmString *hey, vmString *rpl)
{
    ClassLangStringInstance *thiz = static_cast<ClassLangStringInstance *>(_thiz);
    size_t start_pos = 0;
    while ((start_pos = thiz->val->val.find(hey->val, start_pos)) != std::string::npos) {
        thiz->val->val.replace(start_pos, hey->val.length(), rpl->val);
        start_pos += rpl->val.length();
    }
    return thiz;
}

#if 0
vmString *ClassLangString::replaceAll(vmClassInstance * _thiz, /*vmString *thiz,*/ vmString *hey, vmString *rpl)
{
    ClassLangStringInstance *thiz = static_cast<ClassLangStringInstance *>(_thiz);
    size_t start_pos = 0;
    while ((start_pos = thiz->val.find(hey->val, start_pos)) != std::string::npos) {
        thiz->val.replace(start_pos, hey->val.length(), rpl->val);
        start_pos += rpl->val.length();
    }
    return thiz;
}
#endif

vmClassInstance *ClassLangString::newInstance()
{
    return new ClassLangStringInstance(this);
    /*
    vmClass *res = new ClassLangString();
    res->baseClass = this;
    return res;
    */
}
