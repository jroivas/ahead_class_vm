#pragma once

#include <cstdint>
#include <vector>
#include "vm_object.h"

enum {
    ST_EMPTY = 0,
    ST_INT = 1,
    ST_LONG,
    ST_FLOAT,
    ST_DOUBLE,
    ST_OBJ
};

/*
class StackData {
public:
    StackData(nInteger v) : val_i(v), type(ST_INT) { }
    StackData(nLong v) : val_l(v), type(ST_INT) { }
    StackData(nFloat v) : val_f(v), type(ST_FLOAT) { }
    StackData(nDouble v) : val_f(v), type(ST_DOUBLE) { }
    StackData(vmObject *v) : val_o(v), type(ST_OBJ) { }

    union {
        nInteger val_i;
        nLong val_l;
        nFloat val_f;
        nDouble val_d;
        vmObject *val_o;
    };
    uint8_t type;
};
*/

class vmStack
{
public:
    vmStack();
    vmStack(uint32_t);

    void push(vmObject *obj);
    void insert(vmObject *obj);
    vmObject *pop();
    vmObject *peek();
    uint8_t peekType() const;

    void pushInteger(nInteger v);
    void pushLong(nLong v);
    void pushFloat(nFloat v);
    void pushDouble(nDouble v);
    void pushObject(vmObject *o);

    nInteger popInteger();
    nLong popLong();
    nFloat popFloat();
    nDouble popDouble();
    vmObject *popObject();

    uint32_t size;
    uint32_t max_size;
private:
    std::vector<vmObject*> stack;
    //std::vector<StackData> m_stack;
    uint64_t *m_data;
    uint8_t *m_types;
};
