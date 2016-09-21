#include "classloader.h"
#include <iostream>
#include <fstream>

#include <arpa/inet.h>

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
    std::streampos size = cfile.tellg();
    cfile.seekg (0, std::ios::beg);

    m_block = new uint8_t [size];
    cfile.read((char*)m_block, size);

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

    return true;
}

bool vmClassFile::parseCafebabe()
{
    magic_number = ntohl(*(uint32_t*)(m_block));
    m_pos = 4;
    return (magic_number == 0xCAFEBABE);
}

void vmClassFile::parseVersions()
{
    minor_version = ntohs(*(uint16_t*)(m_block + m_pos));
    major_version = ntohs(*(uint16_t*)(m_block + m_pos + 2));

    m_pos += 4;
}

void vmClassFile::parseConstantPool()
{
    uint32_t pos = m_pos;
    constant_pool_count = ntohs(*(uint16_t*)(m_block + pos));
    pos += 2;
    for (uint16_t i = 0; i < constant_pool_count - 1; ++i) {
        vmConstantInfo *res = vmConstantInfo::parse(m_block + pos);
        if (res == nullptr) {
            std::cout << " Invalid constant at " << i << "\n";
            throw "Invalid constant";
        } else {
            constant_pool.push_back(res);
        }
        pos += res->size;
    }
    m_pos = pos;
}

void vmClassFile::parseAccessFlags()
{
    access_flags = ntohs(*(uint16_t*)(m_block + m_pos));
    m_pos += 2;
}

void vmClassFile::parseThis()
{
    this_class = ntohs(*(uint16_t*)(m_block + m_pos));
    m_pos += 2;
}

void vmClassFile::parseSuper()
{
    super_class = ntohs(*(uint16_t*)(m_block + m_pos));
    m_pos += 2;
}

void vmClassFile::parseInterfaces()
{
    uint32_t pos = m_pos;
    interfaces_count = ntohs(*(uint16_t*)(m_block + pos));
    pos += 2;
    for (uint16_t i = 0; i < interfaces_count  - 1; ++i) {
        // FIXME
        std::cout <<  "Unknown interface at" << i << "\n";
        throw "Unknown interface";
    }
    m_pos = pos;
}

void vmClassFile::parseFields()
{
    uint32_t pos = m_pos;
    fields_count = ntohs(*(uint16_t*)(m_block + pos));
    pos += 2;
    for (uint16_t i = 0; i < fields_count  - 1; ++i) {
        // FIXME
        std::cout <<  "Unknown field at" << i << "\n";
        throw "Unknown field";
    }
    m_pos = pos;
}
