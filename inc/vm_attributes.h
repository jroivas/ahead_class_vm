#pragma once

#include <vector>
#include <cstdint>
#include "vm_exceptions.h"

class vmAttributeInfo
{
protected:
    vmAttributeInfo() : info(nullptr) {}
    vmAttributeInfo(vmAttributeInfo *);
    virtual ~vmAttributeInfo();
public:
    static vmAttributeInfo *parse(uint8_t *p);

    uint16_t name_index;
    uint32_t length;
    uint8_t *info;
    uint32_t size;
};

class vmCodeAttribute : public vmAttributeInfo
{
public:
    vmCodeAttribute(vmAttributeInfo *orig);

    uint16_t max_stack;
    uint16_t max_locals;
    uint32_t code_length;
    uint8_t *code;
    uint16_t exceptions_count;
    std::vector<vmExceptionInfo*> exceptions;
    uint16_t attributes_count;
    std::vector<vmAttributeInfo*> attributes;
};
