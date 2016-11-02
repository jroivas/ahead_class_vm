#pragma once

#include <string>
#include <vector>

#include <cstdint>

#include "vm_object.h"
#include "vm_constants.h"
#include "vm_methods.h"
#include "vm_attributes.h"
#include "vm_fields.h"

class vmInterfaceInfo
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

    std::vector<vmConstantInfo*> constant_pool;

    uint16_t access_flags;

    uint16_t this_class;
    uint16_t super_class;

    uint16_t interfaces_count;

    std::vector<vmInterfaceInfo*> interfaces;

    uint16_t fields_count;
    std::vector<vmFieldInfo*> fields;

    uint16_t methods_count;
    std::vector<vmMethodInfo*> methods;

    uint16_t attributes_count;
    std::vector<vmAttributeInfo*> attributes;

    bool parse();

private:
    uint8_t *m_block;
    uint32_t m_pos;
    bool parseCafebabe();
    void parseVersions();
    void parseConstantPool();
    void parseAccessFlags();
    void parseThis();
    void parseSuper();
    void parseInterfaces();
    void parseFields();
    void parseMethods();
    void parseAttributes();
};
