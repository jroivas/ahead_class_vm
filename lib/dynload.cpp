#include <dynload.h>
#include <unordered_map>
#include <vm_object.h>

static std::unordered_map<std::string, vmClass*> __classes;

void registerClass(vmClass *cl)
{
    if (cl == nullptr) {
        throw "Invalid class";
    }
    if (cl->baseClass != nullptr) return;
    __classes[cl->name] = cl;
}

vmClass *loadClass(std::string name)
{
    return __classes[name];
}

