#include "vm.h"
#include <iostream>

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

    /*
    // FIXME Sanity
    vmConstantInfo *tmp = cl->constant_pool[idx];
    vmConstantMethodRef *method = dynamic_cast<vmConstantMethodRef *>(tmp);

    if (!method) throw "aa";
    // TODO More automatic resolving, now this is stupid...

    tmp = method->resolve(cl->constant_pool);
    vmConstantClass *classref = dynamic_cast<vmConstantClass *>(tmp);
    if (!method) throw "bb";

    tmp = classref->resolve(cl->constant_pool);
    vmConstantUtf8 *classname = dynamic_cast<vmConstantUtf8 *>(tmp);
    if (!classname) throw "cc";

    vmConstantNameAndType *nametype = dynamic_cast<vmConstantNameAndType *>(method->resolve2(cl->constant_pool));
    */
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

    //vmConstantInfo *aa = b->resolve(cl->constant_pool);
    //std::cout << "aa: " << aa <<"\n";
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
    }

    std::cout << " Invoke class " << classname->bytes << "\n";
    std::cout << " Name  " << ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->bytes << "\n";
    std::cout << " Type  " << ((vmConstantUtf8 *)(nametype->resolve2(cl->constant_pool)))->bytes << "\n";
}

