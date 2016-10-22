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
    for (uint8_t i = 0; i < 100; ++i) {
        locals.push_back(nullptr);
    }
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
        case 0x40:
            lstore(1);
            break;
        case 0x84:
            iinc();
            break;
        case 0x2a:
            if (locals.empty()) {
                //solve "this" object from cl
                locals.push_back(new vmObject(cl));
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
        default:
            std::cout <<  "Unimplemented: " << std::hex << (uint32_t)opcode << "\n";
            throw "Unimplemented";
    }
}

vmConstantRef *VM::parseRef(uint16_t idx)
{
    vmConstantInfo *tmp = cl->constant_pool[idx];
    vmConstantRef *ref = dynamic_cast<vmConstantRef *>(tmp);
    if (!ref) {
        std::cout << "Invalid reference " << idx << "\n";
        throw "invalid reference";
    }
    return ref;
}

vmConstantClass *VM::parseRefClass(vmConstantRef *p)
{
    vmConstantInfo *tmp = p->resolve(cl->constant_pool);
    vmConstantClass *ref = dynamic_cast<vmConstantClass *>(tmp);
    if (!ref) {
        std::cout << "Invalid reference\n";
        throw "invalid reference";
    }
    return ref;
}

vmConstantNameAndType *VM::parseRefNameType(vmConstantRef *p)
{
    vmConstantInfo *tmp = p->resolve2(cl->constant_pool);
    vmConstantNameAndType *ref = dynamic_cast<vmConstantNameAndType *>(tmp);
    if (!ref) {
        std::cout << "Invalid reference\n";
        throw "invalid reference";
    }
    return ref;
}

vmConstantUtf8 *VM::parseRefUtf8(vmConstantInfo *p)
{
    vmConstantInfo *tmp = p->resolve(cl->constant_pool);
    vmConstantUtf8 *ref = dynamic_cast<vmConstantUtf8 *>(tmp);
    if (!ref) {
        std::cout << "Invalid reference\n";
        throw "invalid reference";
    }
    return ref;
}

void VM::invokeSpecial()
{
    uint16_t idx = read16(ptr + pc);
    pc += 2;

    vmConstantRef *method = parseRef(idx);
    vmConstantClass *classref = parseRefClass(method);
    vmConstantNameAndType *nametype = parseRefNameType(method);
    vmConstantUtf8 *classname = parseRefUtf8(classref);

    if (classname->str() == "java/lang/Object") {
        if (((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() == "<init>") {
            stack->push(new vmObject());
            return;
        }
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

    std::cout << " Invoke class " << classname->str() << "\n";
    std::cout << " Name  " << ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() << "\n";
    std::cout << " Type  " << ((vmConstantUtf8 *)(nametype->resolve2(cl->constant_pool)))->str() << "\n";

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
            vmLong *v = new vmLong(tt.tv_sec * 1000 + tt.tv_nsec / 1000000);
            std::cout << "TIMEMILLI " << v << " " << v->val << "\n";
            stack->push(v);
            //stack->push(new vmObject());
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
                stack->push(new SystemPrinter());
                return;
            }
        }
    }

    std::cout << " Invoke class " << classname->bytes << "\n";
    std::cout << " Name  " << ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->bytes << "\n";
    std::cout << " Type  " << ((vmConstantUtf8 *)(nametype->resolve2(cl->constant_pool)))->bytes << "\n";
    throw "Invalid static";
}


void VM::ldc()
{
    uint16_t idx = *(ptr + pc);
    pc += 1;

    vmConstantInfo *tmp = cl->constant_pool[idx];

    vmConstantInteger *intg = dynamic_cast<vmConstantInteger*>(tmp);
    if (intg) { stack->push(new vmInteger(intg->val)); return; }

    vmConstantLong *lintg = dynamic_cast<vmConstantLong*>(tmp);
    if (lintg) { stack->push(new vmLong(lintg->val)); return; }

    vmConstantFloat *flg = dynamic_cast<vmConstantFloat*>(tmp);
    if (flg) { stack->push(new vmFloat(flg->val)); return; }

    vmConstantDouble *dlg = dynamic_cast<vmConstantDouble*>(tmp);
    if (dlg) { stack->push(new vmDouble(dlg->val)); return; }

    vmConstantString *str = dynamic_cast<vmConstantString*>(tmp);
    if (str) {
        vmConstantUtf8 *n = parseRefUtf8(str);
        stack->push(new vmString(n->str()));
        return;
    }

    throw "Invalid object";
}

void VM::lstore(uint8_t index)
{
    vmObject *d = stack->pop();
    locals[index] = toLong(d);
}

void VM::istore(uint8_t index)
{
    vmObject *d = stack->pop();
    std::cout << "IS\n";
    locals[index] = toInteger(d);
}

void VM::astore_idx()
{
    uint8_t index = *(ptr + pc);
    pc++;

    std::cout << "ST IDX " << (long)index << "\n";
    vmObject *a = stack->pop();
    std::cout << "TT " << typeid(*a).name() << "\n";
    locals[index] = new vmRef(a);
    //stack->push(new vmRef(locals[index]));
}


vmInteger *VM::toInteger(vmObject *d)
{
    std::cout << "toI " << d  << " " << typeName(d) << "\n";
    switch (d->type) {
        case TYPE_INTEGER: return static_cast<vmInteger*>(d);
        case TYPE_LONG: return new vmInteger(((vmLong*)d)->val);
        case TYPE_FLOAT: return new vmInteger(((vmFloat*)d)->val);
        case TYPE_DOUBLE: return new vmInteger(((vmDouble*)d)->val);
        case TYPE_REF: std::cout << " REF " << "\n"; return toInteger(((vmRef*)d)->val);
        default:
            std::cout << " PC " << pc << "\n";
            std::cout << " Convert from " << typeName(d) << "\n";
            throw "Invalid conversion";
    }
}

vmLong *VM::toLong(vmObject *d)
{
    switch (d->type) {
        case TYPE_INTEGER: return static_cast<vmLong*>(d);
        case TYPE_LONG: return new vmLong(((vmInteger*)d)->val);
        case TYPE_FLOAT: return new vmLong(((vmFloat*)d)->val);
        case TYPE_DOUBLE: return new vmLong(((vmDouble*)d)->val);
        case TYPE_REF: return toLong(((vmRef*)d)->val);
        default:
            std::cout << "PC " << pc << "\n";
            std::cout << "Convert from " << typeName(d) << "\n";
            throw "Invalid conversion";
    }
}

void VM::iload(uint8_t index)
{
    std::cout << "Il " << pc << "\n";
    vmInteger *i = toInteger(locals[index]);
    std::cout << " VAL " << typeName(i) << "\n";
    stack->push(i);
}

void VM::icmp(uint8_t oper)
{
    std::cout << "Icmp" << pc << "\n";
    int16_t target = read16(ptr + pc);

    std::cout << " TGT " << stack->size <<"\n";
    vmInteger *v1 = toInteger(stack->pop());
    vmInteger *v2 = toInteger(stack->pop());

    switch (oper) {
        case CMP_EQ:
            if (v1->val == v2->val) vm_goto();
            break;
        case CMP_NE:
            if (v1->val != v2->val) vm_goto();
            break;
        case CMP_GE:
            if (v1->val >= v2->val) vm_goto();
            break;
        case CMP_GT:
            if (v1->val > v2->val) vm_goto();
            break;
        case CMP_LE:
            if (v1->val <= v2->val) vm_goto();
            break;
        case CMP_LT:
            if (v1->val < v2->val) vm_goto();
            break;
        default:
            break;
    }
}

void VM::iinc()
{
    uint8_t index = *(ptr + pc);
    pc++;
    int8_t val = *(ptr + pc);
    pc++;

    ((vmInteger*)locals[index])->val += val;
}

void VM::vm_goto()
{
    int16_t target = read16(ptr + pc);
    std::cout << "TARGET: " << target << " PC " << pc << "\n";
    pc += target - 1;
}
