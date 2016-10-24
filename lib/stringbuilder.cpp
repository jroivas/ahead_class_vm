#include "stringbuilder.h"

StringBuilder::StringBuilder()
    : vmClass("java/lang/StringBuilder")
{
    setFunction("<init>", [](vmStack *) {});
}

vmClass *StringBuilder::newInstance()
{
    vmClass *res = new StringBuilder();
    res->baseClass = this;
    return res;
}
