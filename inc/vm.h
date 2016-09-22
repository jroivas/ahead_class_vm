#pragma once

#include <vector>

#include "classloader.h"
#include "vm_stack.h"

class VM
{
public:
    VM(vmClassFile *, vmStack *);

    void addClassRef(vmClassFile *);

    bool execute(vmCodeAttribute *code);

    uint32_t pc;
    std::vector<vmClassFile *> classes;
    std::vector<vmObject *> locals;
    vmClassFile * cl;
    vmStack *stack;
    uint8_t *ptr;

private:
    uint8_t fetch();
    void decode(uint8_t);
    void invokeSpecial();
    void invokeVirtual();
    void getStatic();
    void ldc();

    vmConstantRef *parseRef(uint16_t idx);
    vmConstantClass *parseRefClass(vmConstantRef *ref);
    vmConstantNameAndType *parseRefNameType(vmConstantRef *ref);
    vmConstantUtf8 *parseRefUtf8(vmConstantInfo *ref);
};