#include "vm.h"
#include <iostream>
#include <typeinfo>

#include <cstring>
#include <cassert>
#include <time.h>
#include <typeinfo>

#include "utils.h"

enum cmpOp {
    CMP_EQ = 1,
    CMP_NE,
    CMP_GE,
    CMP_GT,
    CMP_LE,
    CMP_LT
};

VM::VM(vmClassFile *_cl, vmStack *_stack)
    : cl(_cl),
    stack(_stack),
    ptr(nullptr)
{
    /*
    for (uint8_t i = 0; i < 100; ++i) {
        locals.push_back(nullptr);
    }
    */
}

void VM::addClassRef(vmClassFile *cl)
{
    classes.push_back(cl);
}

bool VM::execute(vmCodeAttribute *code)
{
    assert(code != nullptr);

    pc = 0;
    ptr = code->code;
    while (pc < code->code_length) {
        //std::cout << " PC " << pc << "  " << code->code_length <<"\n";
        uint8_t opcode = fetch();
        //std::cout << " opcd " << pc << " " << std::hex << (long)opcode << std::dec <<"\n";
        decode(opcode);
    }

    return true;
}

uint8_t VM::fetch()
{
    return ptr[pc++];
}

void VM::decode(uint8_t opcode)
{
    switch (opcode) {
        case 0x02:
            stack->push(new vmInteger(-1));
            break;
        case 0x03:
            stack->push(new vmInteger(0));
            break;
        case 0x04:
            stack->push(new vmInteger(1));
            break;
        case 0x05:
            stack->push(new vmInteger(2));
            break;
        case 0x06:
            stack->push(new vmInteger(3));
            break;
        case 0x07:
            stack->push(new vmInteger(4));
            break;
        case 0x08:
            stack->push(new vmInteger(5));
            break;
        case 0x12:
            ldc();
            break;
        case 0x13:
            ldc_w();
            break;
        case 0x14:
            ldc_w();
            break;
        case 0x19:
            aload();
            break;
        case 0x1a:
            iload(0);
            break;
        case 0x1b:
            iload(1);
            break;
        case 0x1c:
            iload(2);
            break;
        case 0x1d:
            iload(3);
            break;
        case 0x1e:
            lload(0);
            break;
        case 0x1f:
            lload(1);
            break;
        case 0x20:
            lload(2);
            break;
        case 0x21:
            lload(3);
            break;
        case 0x2a:
            if (locals.empty()) {
                //solve "this" object from cl
                //locals.push_back(new vmObject(cl));
            }
            stack->push(locals[0]);
            break;
        case 0x3a:
            astore_idx();
            break;
        case 0x3b:
            istore(0);
            break;
        case 0x3c:
            istore(1);
            break;
        case 0x3d:
            istore(2);
            break;
        case 0x3e:
            istore(3);
            break;
        case 0x3f:
            lstore(0);
            break;
        case 0x40:
            lstore(1);
            break;
        case 0x41:
            lstore(2);
            break;
        case 0x42:
            lstore(3);
            break;
        case 0x59:
            dup();
            break;
        case 0x65:
            lsub();
            break;
        case 0x6b:
            dmul();
            break;
        case 0x6f:
            ddiv();
            break;
        case 0x8a:
            l2d();
            break;
        case 0x84:
            iinc();
            break;
        case 0xa2:
            icmp(CMP_GE);
            break;
        case 0xa7:
            vm_goto();
            break;
        case 0xb1:
            // return
            return;
        case 0xb2:
            getStatic();
            break;
        case 0xb6:
            invokeVirtual();
            break;
        case 0xb7:
            invokeSpecial();
            break;
        case 0xb8:
            invokeStatic();
            break;
        case 0xbb:
            vm_new();
            break;
        default:
            std::cout <<  "Unimplemented: " << std::hex << (uint32_t)opcode << " @" << std::dec << pc << "\n";
            throw "Unimplemented";
    }
}

vmConstantRef *VM::parseRef(uint16_t idx)
{
    vmConstantInfo *tmp = cl->constant_pool[idx];
    if (tmp->tag != C_FieldRef &&
        tmp->tag != C_MethodRef &&
        tmp->tag != C_InterfaceMethodRef) {
        std::cout << "Invalid reference " << idx << "\n";
        throw "invalid reference";
    }
    return static_cast<vmConstantRef *>(tmp);
}

vmConstantClass *VM::parseClassConstant(uint16_t idx)
{
    vmConstantInfo *tmp = cl->constant_pool[idx];
    if (tmp->tag != C_Class) {
        std::cout << "Invalid reference\n";
        throw "invalid reference";
    }
    return static_cast<vmConstantClass *>(tmp);
}

vmConstantClass *VM::parseRefClass(vmConstantRef *p)
{
    vmConstantInfo *tmp = p->resolve(cl->constant_pool);
    if (tmp->tag != C_Class) {
        std::cout << "Invalid reference\n";
        throw "invalid reference";
    }
    return static_cast<vmConstantClass *>(tmp);
}

vmConstantNameAndType *VM::parseRefNameType(vmConstantRef *p)
{
    vmConstantInfo *tmp = p->resolve2(cl->constant_pool);
    if (tmp->tag != C_NameAndType) {
        std::cout << "Invalid reference\n";
        throw "invalid reference";
    }
    return static_cast<vmConstantNameAndType *>(tmp);
}

vmConstantUtf8 *VM::parseRefUtf8(vmConstantInfo *p)
{
    vmConstantInfo *tmp = p->resolve(cl->constant_pool);
    if (tmp->tag != C_Utf8) {
        std::cout << "Invalid reference\n";
        throw "invalid reference";
    }
    return static_cast<vmConstantUtf8 *>(tmp);
}

void VM::invokeSpecial()
{
    invokeVirtual();
    return;
    uint16_t idx = read16(ptr + pc);
    pc += 2;

    vmConstantRef *method = parseRef(idx);
    vmConstantClass *classref = parseRefClass(method);
    vmConstantNameAndType *nametype = parseRefNameType(method);
    vmConstantUtf8 *classname = parseRefUtf8(classref);

    vmObject *objectRef = stack->pop();
    std::cout << " Invoke special " << classname->str() << "\n";
    std::cout << " Name  " << ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() << "\n";
    std::cout << " Type  " << ((vmConstantUtf8 *)(nametype->resolve2(cl->constant_pool)))->str() << "\n";

    if (classname->str() == "java/lang/Object") {
        if (((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() == "<init>") {
            stack->push(new vmObject());
            return;
        }
    }
    if (objectRef->type == TYPE_CLASS) {
        vmClass *inst = static_cast<vmClass*>(objectRef);
        auto f = inst->getFunction(((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str());
        if (!f) {
            throw "Invalid function on " + classname->str() + ": " + ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str();
        }
        f->func(stack);
        return;
    }

    std::cout << " Invoke class " << classname->str() << "\n";
    std::cout << " Name  " << ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() << "\n";
    std::cout << " Type  " << ((vmConstantUtf8 *)(nametype->resolve2(cl->constant_pool)))->str() << "\n";
    throw "Invalid call";
}

void VM::invokeVirtual()
{
    uint16_t idx = read16(ptr + pc);
    pc += 2;

    vmConstantRef *method = parseRef(idx);
    vmConstantClass *classref = parseRefClass(method);
    vmConstantNameAndType *nametype = parseRefNameType(method);
    vmConstantUtf8 *classname = parseRefUtf8(classref);

    /*
    std::cout << " Invoke class " << classname->str() << "\n";
    std::cout << " Name  " << ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() << "\n";
    std::cout << " Desc  " << ((vmConstantUtf8 *)(nametype->resolve2(cl->constant_pool)))->str() << "\n";
    */
    /*
    std::pair<std::string, std::string> rr = parseParams(((vmConstantUtf8 *)(nametype->resolve2(cl->constant_pool)))->str());
    std::vector<std::string> pp = parseField(rr.first);
    std::vector<std::string> r = parseField(rr.second);
    */
    /*
    std::cout << " params: ";
    for (auto p : pp) {
        std::cout << " " << p << " ";
    }
    std::cout << "\n";
    if (r.size()>0){
    std::cout << " retval: " << r[0] << "\n";
    }
    */


    /*
    if (classname->str() == "java/io/PrintStream") {
        // FIXME real class and virtual methods
        if (((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() == "println") {
            // FIXME do not hardcode parameter handling
            if (((vmConstantUtf8 *)(nametype->resolve2(cl->constant_pool)))->str() == "(Ljava/lang/String;)V") {
                //std::cout << ((vmConstantUtf8 *)(nametype->resolve2(cl->constant_pool)))->str() << "\n";
                vmObject *obj = stack->pop();
                vmString *str = static_cast<vmString*>(obj);
                std::cout << str->val << "\n";
                return;
            }
            //return;
        }
    }
    */

    if (classname->str() == "java/lang/Object") {
        if (((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() == "<init>") {
            stack->push(new vmObject());
            return;
        }
    }

    /*
    vmStack *st = new vmStack();
    for (auto p : pp) {
        st->insert(stack->pop());
    }
    // FIXME first pop off arguments, then pop objectref
    vmObject *objectRef = stack->pop();
    st->insert(objectRef);
    */

    // FIXME new stack frame
    //vmClass *inst = dynamic_cast<vmClass*>(objectRef);
    //vmClass *inst = dynamic_cast<vmClass*>(objectRef);
    vmClass *inst = loadClass(classname->str());
    if (inst) {
        std::string fname = ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str();
        FunctionDesc *f = inst->getFunction(fname);
        if (!f) {
            std::cout <<  "Invalid function on " + classname->str() + ": " + ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() << "\n";
            throw "Invalid function on " + classname->str() + ": " + ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str();
        }
        if (!f->init) {
            f->parse(((vmConstantUtf8 *)(nametype->resolve2(cl->constant_pool)))->str());
        }
        // FIXME constructing new stack
        /*vmStack *st = new vmStack();
        for (auto p : f->params) {
            st->insert(stack->pop());
        }

        vmObject *objectRef = stack->pop();
        st->insert(objectRef);
        */

        // Call
        f->func(stack);
        /*
        if (f->returnType != "") {
            stack->push(st->pop());
        }
        */
        return;
    }

    //std::cout << " obj " << typeName(objectRef) << "\n";
    std::cout << " Invoke class " << classname->str() << "\n";
    std::cout << " Name  " << ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() << "\n";
    std::cout << " Type  " << ((vmConstantUtf8 *)(nametype->resolve2(cl->constant_pool)))->str() << "\n";
    throw "Invalid virtual call";
}

void VM::invokeStatic()
{
    uint16_t idx = read16(ptr + pc);
    pc += 2;

    vmConstantRef *method = parseRef(idx);
    vmConstantClass *classref = parseRefClass(method);
    vmConstantNameAndType *nametype = parseRefNameType(method);
    vmConstantUtf8 *classname = parseRefUtf8(classref);

    if (classname->str() == "java/lang/System") {
        if (((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() == "currentTimeMillis") {
            struct timespec tt;
            clock_gettime(CLOCK_REALTIME, &tt);
            stack->push(new vmLong(tt.tv_sec * 1000 + tt.tv_nsec / 1000000));
            /*
            vmLong *v = new vmLong(tt.tv_sec * 1000 + tt.tv_nsec / 1000000);
            std::cout << "TIMEMILLI " << v << " " << v->val << "\n";
            stack->push(v);
            */
            return;
        }
    }

    std::cout << " Invoke class " << classname->str() << "\n";
    std::cout << " Name  " << ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() << "\n";
    std::cout << " Type  " << ((vmConstantUtf8 *)(nametype->resolve2(cl->constant_pool)))->str() << "\n";
    throw "Invalid call";
}

void VM::getStatic()
{
    uint16_t idx = read16(ptr + pc);
    pc += 2;

    vmConstantRef *method = parseRef(idx);
    vmConstantClass *classref = parseRefClass(method);
    vmConstantNameAndType *nametype = parseRefNameType(method);
    vmConstantUtf8 *classname = parseRefUtf8(classref);

    if (classname->str() == "java/lang/System") {
        if (((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() == "out") {
            if (((vmConstantUtf8 *)(nametype->resolve2(cl->constant_pool)))->str() == "Ljava/io/PrintStream;") {
                vmClass *cl = loadClass("java/io/PrintStream");
                //cl->val = ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str();
                cl->val = "out";
                if (!cl) {
                    throw "Can't load class: java/io/PrintStream";
                }
                stack->push(cl);
                //stack->push(new SystemPrinter());
                return;
            }
        }
    }

    std::cout << " getStatic class " << classname->str() << "\n";
    std::cout << " Name  " << ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() << "\n";
    std::cout << " Type  " << ((vmConstantUtf8 *)(nametype->resolve2(cl->constant_pool)))->str() << "\n";
    throw "Invalid static";
}


void VM::ldc()
{
    uint16_t idx = *(ptr + pc);
    pc += 1;
    ldc_idx(idx);
}

void VM::ldc_w()
{
    uint16_t idx = read16(ptr + pc);
    pc += 2;
    ldc_idx(idx);
}

void VM::ldc_idx(uint16_t idx)
{
    // Constant create objects, use here as-is
    vmConstantInfo *tmp = cl->constant_pool[idx];

    switch (tmp->tag) {
        case C_Integer:
            stack->push(&((vmConstantInteger*)tmp)->val);
            //stack->push(new vmInteger(((vmConstantInteger*)tmp)->val));
            break;
        case C_Long:
            stack->push(&((vmConstantLong*)tmp)->val);
            break;
        case C_Float:
            stack->push(&((vmConstantFloat*)tmp)->val);
            break;
        case C_Double:
            stack->push(&((vmConstantDouble*)tmp)->val);
            break;
        case C_String: {
            stack->push(&parseRefUtf8((vmConstantString*)tmp)->val);
            //vmConstantUtf8 *n = parseRefUtf8((vmConstantString*)tmp);
            //stack->push(new vmString(n->str()));
            break;
        }
        default:
            std::cout <<  "Invalid object: " << idx << " " << tmp << "\n";
            throw "Invalid object";
            break;
    }
}

void VM::lstore(uint8_t index)
{
    vmObject *d = stack->pop();
    locals[index] = toLong(d);
}

void VM::istore(uint8_t index)
{
    locals[index] = toInteger(stack->pop());;
}

void VM::astore_idx()
{
    uint8_t index = *(ptr + pc);
    pc++;

    locals[index] = new vmRef(stack->pop());
}


vmInteger *VM::toInteger(vmObject *d)
{
    switch (d->type) {
        case TYPE_INTEGER: return static_cast<vmInteger*>(d);
        //case TYPE_INTEGER: return new vmInteger(static_cast<vmInteger*>(d)->val);
        case TYPE_LONG: return new vmInteger(((vmLong*)d)->val);
        case TYPE_FLOAT: return new vmInteger(((vmFloat*)d)->val);
        case TYPE_DOUBLE: return new vmInteger(((vmDouble*)d)->val);
        case TYPE_REF: std::cout << " REF "  << "\n"; return toInteger(((vmRef*)d)->val);
        default:
            std::cout << " PC " << pc << "\n";
            std::cout << " Convert from " << typeName(d) << "\n";
            throw "Invalid conversion";
    }
}

vmLong *VM::toLong(vmObject *d)
{
    switch (d->type) {
        case TYPE_LONG: return static_cast<vmLong*>(d);
        case TYPE_INTEGER: return new vmLong(((vmInteger*)d)->val);
        case TYPE_FLOAT: return new vmLong(((vmFloat*)d)->val);
        case TYPE_DOUBLE: return new vmLong(((vmDouble*)d)->val);
        case TYPE_REF: return toLong(((vmRef*)d)->val);
        default:
            std::cout << "PC " << pc << "\n";
            std::cout << "Convert from " << typeName(d) << "\n";
            throw "Invalid conversion";
    }
}

vmDouble *VM::toDouble(vmObject *d)
{
    switch (d->type) {
        case TYPE_DOUBLE: return static_cast<vmDouble*>(d);
        case TYPE_LONG: return new vmDouble(((vmInteger*)d)->val);
        case TYPE_FLOAT: return new vmDouble(((vmFloat*)d)->val);
        case TYPE_INTEGER: return new vmDouble(((vmInteger*)d)->val);
        case TYPE_REF: return toDouble(((vmRef*)d)->val);
        default:
            std::cout << "PC " << pc << "\n";
            std::cout << "Convert from " << typeName(d) << "\n";
            throw "Invalid conversion";
    }
}

void VM::aload()
{
    uint8_t index = *(ptr + pc);
    stack->push(new vmRef(locals[index]));
}

void VM::iload(uint8_t index)
{
    stack->push(toInteger(locals[index]));
}

void VM::lload(uint8_t index)
{
    stack->push(toLong(locals[index]));
}

void VM::icmp(uint8_t oper)
{
    vmInteger *v1 = toInteger(stack->pop());
    vmInteger *v2 = toInteger(stack->pop());

    switch (oper) {
        case CMP_EQ:
            if (v2->val == v1->val) return vm_goto();
            break;
        case CMP_NE:
            if (v2->val != v1->val) return vm_goto();
            break;
        case CMP_GE:
            if (v2->val >= v1->val) return vm_goto();
            break;
        case CMP_GT:
            if (v2->val > v1->val) return vm_goto();
            break;
        case CMP_LE:
            if (v2->val <= v1->val) return vm_goto();
            break;
        case CMP_LT:
            if (v2->val < v1->val) return vm_goto();
            break;
        default:
            break;
    }
    pc += 2;
}

void VM::iinc()
{
    uint8_t index = *(ptr + pc);
    pc++;
    int8_t val = *(ptr + pc);
    pc++;

    toInteger(locals[index])->val += val;
    //((vmInteger*)locals[index])->val += val;
}

void VM::vm_goto()
{
    int16_t target = read16(ptr + pc);
    //std::cout << "TARGET: " << target << " PC " << pc << "\n";
    pc += target - 1;
    //std::cout << " TARGET PC " << pc << "\n";
}

void VM::lsub()
{
    vmLong *v1 = toLong(stack->pop());
    vmLong *v2 = toLong(stack->pop());
    //std::cout << " lsub " << v2->val <<  " " << v1->val <<  " " << (v2->val - v1->val) << "\n";
    stack->push(new vmLong(v2->val - v1->val));
}

void VM::ddiv()
{
    vmDouble *v1 = toDouble(stack->pop());
    vmDouble *v2 = toDouble(stack->pop());
    if (v1->val == 0) {
        throw "Divide by zero";
    }
    stack->push(new vmDouble(v2->val / v1->val));
}

void VM::dmul()
{
    vmDouble *v1 = toDouble(stack->pop());
    vmDouble *v2 = toDouble(stack->pop());
    stack->push(new vmDouble(v2->val * v1->val));
}

void VM::vm_new()
{
    int16_t idx = read16(ptr + pc);
    pc += 2;

    // FIXME
    vmConstantClass *classref = parseClassConstant(idx);
    vmConstantUtf8 *classname = parseRefUtf8(classref);
    //std::cout << " New class type " << classname->bytes << "\n";
    vmClass *c = loadClass(classname->str());
    //std::cout << " New class base " << c << "\n";
    stack->push(c->newInstance());
}

void VM::dup()
{
    vmObject *o = stack->peek();
    stack->push(o);
}

void VM::l2d()
{
    vmLong *l = toLong(stack->pop());
    stack->push(new vmDouble(l->val));
}
