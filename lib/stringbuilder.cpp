#include "stringbuilder.h"

vmClass *StringBuilder::newInstance()
{
    vmClass *res = new StringBuilder();
    res->baseClass = this;
    return res;
}
