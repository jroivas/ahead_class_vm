#include "vm.h"
#include <iostream>
#include <typeinfo>

#include <cstring>
#include <cassert>

#include "utils.h"

VM::VM(vmClassFile *_cl, vmStack *_stack)
    : cl(_cl),
    stack(_stack),
    ptr(nullptr)
{
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
        std::cout << " PC " << pc << "  " << code->code_length <<"\n";
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
        case 0x12:
            ldc();
            break;
        case 0x2a:
            if (locals.empty()) {
                locals.push_back(new vmObject(cl));
            }
            stack->push(locals[0]);
            break;
        case 0xb1:
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

    if (classname->str() == "java/io/PrintStream") {
        // FIXME real class and virtual methods
        if (((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() == "println") {
            // FIXME do not hardcode parameter handling
            if (((vmConstantUtf8 *)(nametype->resolve2(cl->constant_pool)))->str() == "(Ljava/lang/String;)V") {
                //std::cout << ((vmConstantUtf8 *)(nametype->resolve2(cl->constant_pool)))->str() << "\n";
                //vmObject *obj = stack->pop();
            }
            //return;
        }
    }

    std::cout << " Invoke class " << classname->str() << "\n";
    std::cout << " Name  " << ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() << "\n";
    std::cout << " Type  " << ((vmConstantUtf8 *)(nametype->resolve2(cl->constant_pool)))->str() << "\n";
    throw "Invalid virtual call";
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
