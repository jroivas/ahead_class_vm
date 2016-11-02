#include "vm_stack.h"
#include <iostream>
#include <cstring>

#define assert_stack_size_push() if (size>=max_size) throw "PUSH: Out of bounds";
#define assert_stack_size_pop() if (size==0) throw "POP: Out of bounds";

vmStack::vmStack()
    : size(0),
    max_size(32)
{
    m_data = new uint64_t[max_size];
    m_types = new uint8_t[max_size];
}

vmStack::vmStack(uint32_t s)
    : size(0),
    max_size(s)
{
    m_data = new uint64_t[max_size];
    m_types = new uint8_t[max_size];
}

void vmStack::push(vmObject *obj)
{
    ++size;
    //std::cout << " >> PSH " << obj << " " << typeName(obj) << " SZ " << size << "\n";
    stack.push_back(obj);
}

void vmStack::insert(vmObject *obj)
{
    ++size;
    stack.insert(stack.begin(), obj);
}

vmObject *vmStack::pop()
{
    vmObject *tmp = stack.back();
    --size;
    //std::cout << " << POP " << tmp << " " << typeName(tmp) <<  " SZ " << size << "\n";
    stack.pop_back();
    return tmp;
}

vmObject *vmStack::peek()
{
    return stack.back();
}

uint8_t vmStack::peekType() const
{
    if (size == 0) return ST_EMPTY;
    return m_types[size - 1];
}

void vmStack::pushInteger(nInteger v)
{
    assert_stack_size_push();
    m_types[size] = ST_INT;
    m_data[size++] = static_cast<uint64_t>(v);
}

void vmStack::pushLong(nLong v)
{
    assert_stack_size_push();
    m_types[size] = ST_LONG;
    m_data[size++] = static_cast<uint64_t>(v);
}

void vmStack::pushFloat(nFloat v)
{
    assert_stack_size_push();
    m_types[size] = ST_FLOAT;
    memcpy(&m_data[size++], &v, sizeof(nFloat));
}

void vmStack::pushDouble(nDouble v)
{
    assert_stack_size_push();
    m_types[size] = ST_DOUBLE;
    memcpy(&m_data[size++], &v, sizeof(nDouble));
}

void vmStack::pushObject(vmObject *v)
{
    assert_stack_size_push();
    m_types[size] = ST_OBJ;
    m_data[size++] = reinterpret_cast<uint64_t>(v);
}

nInteger vmStack::popInteger()
{
    assert_stack_size_pop();
    if (m_types[--size] != ST_INT) throw "Invalid type";
    return static_cast<nInteger>(m_data[size]);
}

nLong vmStack::popLong()
{
    assert_stack_size_pop();
    if (m_types[--size] != ST_LONG) throw "Invalid type";
    return static_cast<nLong>(m_data[size]);
}


nFloat vmStack::popFloat()
{
    assert_stack_size_pop();
    if (m_types[--size] != ST_FLOAT) throw "Invalid type";
    nFloat v;
    memcpy(&v, &m_data[size], sizeof(nFloat));
    return v;
}

nDouble vmStack::popDouble()
{
    assert_stack_size_pop();
    if (m_types[--size] != ST_DOUBLE) throw "Invalid type";
    nDouble v;
    memcpy(&v, &m_data[size], sizeof(nDouble));
    return v;
}

vmObject *vmStack::popObject()
{
    assert_stack_size_pop();
    if (m_types[--size] != ST_OBJ) throw "Invalid type";
    return reinterpret_cast<vmObject*>(m_data[size]);
}
