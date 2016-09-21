#include "classloader.h"
#include <iostream>
#include <fstream>

#include <arpa/inet.h>

vmClassFile::vmClassFile(std::string fname)
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
    m_pos = 4;
    m_pos = parseVersions(m_pos);
    m_pos = parseConstantPool(m_pos);

    return true;
}

bool vmClassFile::parseCafebabe()
{
    magic_number = ntohl(*(uint32_t*)(m_block));
    return (magic_number == 0xCAFEBABE);

    if (m_block[0] != 0xCA) return false;
    if (m_block[1] != 0xFE) return false;
    if (m_block[2] != 0xBA) return false;
    if (m_block[3] != 0xBE) return false;

    std::cout << "magic_number " << magic_number << "  " << 0xcafebabe << "\n";

    return true;
}

uint32_t vmClassFile::parseVersions(uint32_t pos)
{
    minor_version = ntohs(*(uint16_t*)(m_block + pos));
    pos += 2;
    major_version = ntohs(*(uint16_t*)(m_block + pos));
    pos += 2;

    return pos;
}

uint32_t vmClassFile::parseConstantPool(uint32_t pos)
{
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
    return pos;
}
