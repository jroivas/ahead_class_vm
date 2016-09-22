#include "classloader.h"
#include <iostream>
#include <fstream>

#include "utils.h"

vmClassFile::vmClassFile(std::string fname)
    : m_pos(0)
{
    readFromFile(fname);
}

bool vmClassFile::readFromFile(std::string fname)
{
    std::ifstream cfile(fname.c_str(), std::ios::in|std::ios::binary);

    if (!cfile.is_open()) return false;

    cfile.seekg(0, std::ios::end);
    uint64_t size = cfile.tellg();
    cfile.seekg (0, std::ios::beg);

    m_block = new uint8_t [size + 1];
    cfile.read((char*)m_block, size);
    if (size != (uint64_t)cfile.gcount()) return false;

    cfile.close();

    return true;
}

bool vmClassFile::parse()
{
    if (!parseCafebabe()) return false;
    parseVersions();
    parseConstantPool();
    parseAccessFlags();
    parseThis();
    parseSuper();
    parseInterfaces();
    parseFields();
    parseMethods();
    parseAttributes();

    return true;
}

bool vmClassFile::parseCafebabe()
{
    magic_number = read32(m_block);
    m_pos = 4;
    return (magic_number == 0xCAFEBABE);
}

void vmClassFile::parseVersions()
{
    minor_version = read16(m_block + m_pos);
    major_version = read16(m_block + m_pos + 2);

    m_pos += 4;
}

void vmClassFile::parseConstantPool()
{
    uint32_t pos = m_pos;
    constant_pool_count = read16(m_block + pos);
    pos += 2;
    constant_pool.push_back(nullptr);
    for (uint16_t i = 0; i < constant_pool_count - 1; ++i) {
        //std::cout << "#" << (i+1) << " ";
        vmConstantInfo *res = vmConstantInfo::parse(m_block + pos);
        //std::cout << "\n";
        if (res == nullptr) {
            std::cout << " Invalid constant at " << i << "\n";
            throw "Invalid constant";
        } else {
            constant_pool.push_back(res);
        }
        pos += res->size;
        i += res->index_inc - 1;
    }
    m_pos = pos;
}

void vmClassFile::parseAccessFlags()
{
    access_flags = read16(m_block + m_pos);
    m_pos += 2;
}

void vmClassFile::parseThis()
{
    this_class = read16(m_block + m_pos);
    m_pos += 2;
}

void vmClassFile::parseSuper()
{
    super_class = read16(m_block + m_pos);
    m_pos += 2;
}

void vmClassFile::parseInterfaces()
{
    uint32_t pos = m_pos;
    interfaces_count = read16(m_block + m_pos);
    pos += 2;
    for (uint16_t i = 0; i < interfaces_count; ++i) {
        // FIXME
        std::cout <<  "Unknown interface at " << i << "\n";
        throw "Unknown interface";
    }
    m_pos = pos;
}

void vmClassFile::parseFields()
{
    uint32_t pos = m_pos;
    fields_count = read16(m_block + m_pos);
    pos += 2;
    for (uint16_t i = 0; i < fields_count; ++i) {
        vmFieldInfo *res = vmFieldInfo::parse(m_block + pos);
        if (res == nullptr) {
            std::cout <<  "Unknown field at " << i << "\n";
            throw "Unknown field";
        } else {
            fields.push_back(res);
        }
        pos += res->size;
    }
    m_pos = pos;
}

void vmClassFile::parseMethods()
{
    uint32_t pos = m_pos;
    methods_count = read16(m_block + m_pos);
    pos += 2;
    for (uint16_t i = 0; i < methods_count; ++i) {
        vmMethodInfo *res = vmMethodInfo::parse(m_block + pos);
        if (res == nullptr) {
            std::cout <<  "Unknown method at " << i << "\n";
            throw "Unknown method";
        } else {
            methods.push_back(res);
        }
        pos += res->size;
    }
    m_pos = pos;
}

void vmClassFile::parseAttributes()
{
    uint32_t pos = m_pos;
    attributes_count = read16(m_block + m_pos);
    pos += 2;
    for (uint16_t i = 0; i < attributes_count; ++i) {
        vmAttributeInfo *res = vmAttributeInfo::parse(m_block + pos);
        if (res == nullptr) {
            std::cout <<  "Unknown attribute at " << i << "\n";
            throw "Unknown attribute";
        } else {
            attributes.push_back(res);
        }
        pos += res->size;
    }
    m_pos = pos;
}
