#pragma once

#include "classloader.h"

class vmClassFile;

class vmObject
{
public:
    vmObject() : classFile(nullptr) {}
    vmObject(vmClassFile *cl) : classFile(cl) {}
    virtual ~vmObject() {}

    vmClassFile *classFile;

    void init() {}
};
