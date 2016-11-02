#pragma once

#include <string>

class vmClass;

void registerClass(vmClass *);
vmClass *loadClass(std::string);
