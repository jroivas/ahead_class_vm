#pragma once

#include <string>
#include <vector>

#include <cstdint>

#include "vm_object.h"

class vmConstantInfo
{
};

/*
class vmInterfaceList
{
    std::vector<vmObject*> items;
};
*/

class vmFieldInfo
{
};

class vmMethodInfo
{
};

class vmAttributeInfo
{
};

class vmClassFile {
public:
    vmClassFile() {}
    vmClassFile(std::string);

    bool readFromFile(std::string);

    uint32_t magic_number;

    uint16_t minor_version;
    uint16_t major_version;

    uint16_t constant_pool_count;

    //cp_info constant_pool[constant_pool_count - 1];
    std::vector<vmConstantInfo> constant_pool;

    uint16_t access_flags;

    uint16_t this_class;
    uint16_t super_class;

    uint16_t interfaces_count;

    std::vector<uint16_t> interfaces;

    uint16_t fields_count;
    std::vector<vmFieldInfo> fields;

    uint16_t methods_count;
    std::vector<vmFieldInfo> methods;

    uint16_t attributes_count;
    std::vector<vmAttributeInfo> attributes;

private:
    char *m_block;
};
