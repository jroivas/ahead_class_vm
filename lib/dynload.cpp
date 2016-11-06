#include <dynload.h>
#include <unordered_map>
#include <vm_object.h>
#include <typeinfo>

static std::unordered_map<std::string, vmClass*> __classes;

void registerClass(vmClass *cl)
{
    if (cl == nullptr) {
        throw "Invalid class";
    }
    //if (cl->baseClass != nullptr) return;
    __classes[cl->name] = cl;
}

vmClass *loadClass(std::string name)
{
    return __classes[name];
}

std::string solveClassObjectName(std::string name)
{
    if (name == "java/lang/System") return "";
    if (name == "java/io/PrintStream") return "PrintStream";
    if (name == "java/lang/StringBuilder") return "StringBuilder";
    if (name == "java/lang/String") return "ClassLangString";
    if (name == "java/lang/Object") return "LangObject";
    /* FIXME
    */
    for (auto c : __classes) {
        if (c.first == name) return typeid(c).name();
    }
    return "";
}
