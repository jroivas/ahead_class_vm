#include "classloader.h"
#include <iostream>
#include <fstream>

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

    m_block = new char [size];
    cfile.read(m_block, size);

    cfile.close();

    return true;
}
