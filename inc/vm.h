#pragma once

#include <vector>
#include <string>

#include "classloader.h"
#include "vm_stack.h"

class VM
{
public:
    VM(vmClassFile *, vmStack *);

    void addClassRef(vmClassFile *);

    bool execute(vmCodeAttribute *code);
    std::string transcompile(std::string name, vmCodeAttribute *code);

    uint32_t pc;
    std::vector<vmClassFile *> classes;
    std::map<uint16_t, vmObject *> locals;
    vmClassFile * cl;
    vmStack *stack;
    uint8_t *ptr;

private:
    uint8_t fetch();
    uint16_t fetch16();
    void decode(uint8_t);
    void invokeSpecial();
    void invokeVirtual();
    void invokeStatic();
    void putField();
    void getStatic();
    void ldc();
    void ldc_w();
    void ldc_idx(uint16_t index);
    void lstore(uint8_t index);
    void istore(uint8_t index);
    void aload();
    void iload(uint8_t index);
    void lload(uint8_t index);
    void icmp(uint8_t oper);
    void astore_idx();
    void iinc();
    void lsub();
    void dmul();
    void ddiv();
    void dup();
    void l2d();
    void vm_goto();
    void vm_new();

    std::string indent();
    void iin() { ++_indent; }
    void din() { --_indent; }
    std::string fixName(std::string name);
    std::string genCode(uint8_t opcode, std::string &name);
    std::string gen_iconst(int16_t val);
    std::string gen_iload(int16_t val);
    std::string gen_istore(int16_t val);
    std::string gen_lload(int16_t val);
    std::string gen_aload();
    std::string gen_lstore(int16_t val);
    std::string gen_getStatic();
    std::string gen_invokeStatic();
    std::string gen_invokeVirtual();
    std::string gen_putField();
    std::string gen_ldc();
    std::string gen_ldc_w();
    std::string gen_ldc_idx(uint16_t idx);
    std::string gen_icmp(uint8_t oper, std::string &name);
    std::string gen_astore_idx();
    std::string gen_iinc();
    std::string gen_dmul();
    std::string gen_ddiv();
    std::string gen_goto(std::string &name);
    std::string gen_lsub();
    std::string gen_new();
    std::string gen_dup();
    std::string gen_l2d();

    vmInteger *toInteger(vmObject *);
    vmLong *toLong(vmObject *);
    vmDouble *toDouble(vmObject *);

    vmConstantRef *parseRef(uint16_t idx);
    vmConstantClass *parseRefClass(vmConstantRef *ref);
    vmConstantNameAndType *parseRefNameType(vmConstantRef *ref);
    vmConstantUtf8 *parseRefUtf8(vmConstantInfo *ref);
    vmConstantClass *parseClassConstant(uint16_t idx);
    uint8_t _indent;
    uint32_t _temp;
public:
    std::vector<std::string> loadstack;
    std::map<std::string, std::string> loaded_classes;
    std::map<std::string, std::pair<std::string, std::string>> loaded_func;
};
