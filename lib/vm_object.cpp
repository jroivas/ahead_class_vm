#include <vm_object.h>

std::string typeName(vmObject *o) {
    if (o) {
        switch (o->type) {
            case TYPE_INV: return "<Invalid>";
            case TYPE_OBJ: return "Object";
            case TYPE_STRING: return "String";
            case TYPE_INTEGER: return "Integer";
            case TYPE_LONG: return "Long";
            case TYPE_FLOAT: return "Float";
            case TYPE_DOUBLE: return "Double";
            case TYPE_REF: return "Ref";
            case TYPE_CLASS: return "Class";
            case TYPE_CLASS_INST: return "ClassInst";
        }
    }
    return "INVALID";
}

#include <iostream>

//std::function<void(vmStack*)> vmClass::getFunction(std::string name)
#if 0
FunctionDesc *vmClass::getFunction(std::string name)
{
    if (baseClass != nullptr) {
        return baseClass->getFunction(name);
    }
    return methods[name];
    /*for (auto m : methods) {
        if (m.first == name) {
            return m.second;
        }
    }
    //std::function<void(vmStack*)> r;
    return nullptr;
    */
}
#endif

#if 0
void vmClass::setFunction(std::string name, FunctionDesc *f)
{
    if (baseClass == nullptr) {
    /*if (baseClass != nullptr && baseClass != this) {
        baseClass->setFunction(name, f);
    } else {
        */
        methods[name] = f;
    }
}
#endif

std::string formatType(std::string val, uint32_t array)
{
    for (uint32_t i = 0; i < array; ++i) {
        // FIXME
        val+= "[]";
    }
    return val;
}

std::string parseType(std::string t)
{
    // FIXME expects Ltype;
    size_t l = t.length();
    if (l <= 2) return t;
    if (t[0] != 'L') return t;
    return t.substr(1, l - 2);
}

std::string typeToNative(std::string val)
{
    if (val == "void") return "void";
    if (val == "byte") return "uint8_t";
    if (val == "char") return "char";
    if (val == "double") return "vmDouble*";
    if (val == "float") return "vmFloat*";
    if (val == "int") return "vmInteger*";
    if (val == "long") return "vmLong*";
    if (val == "short") return "vmInteger*"; //FIXME
    if (val == "bool") return "bool";
    if (val == "java/lang/String") return "vmString*";
    //FIXME
    return "vmObject* /* " + val + " */";
}

std::vector<std::string> parseField(std::string f)
{
    std::vector<std::string> types;
    bool ref = false;
    std::string v = "";
    uint32_t array = 0;
    for (auto c : f) {
        if (ref && c == ';') {
            types.push_back(formatType(v, array));
            ref = false;
            array = 0;
        } else if (ref) {
            v += c;
        } else {
            switch (c) {
                case 'B': types.push_back(formatType("byte", array)); array = 0; break;
                case 'C': types.push_back(formatType("char", array)); array = 0; break;
                case 'D': types.push_back(formatType("double", array)); array = 0; break;
                case 'F': types.push_back(formatType("float", array)); array = 0; break;
                case 'I': types.push_back(formatType("int", array)); array = 0; break;
                case 'J': types.push_back(formatType("long", array)); array = 0; break;
                case 'S': types.push_back(formatType("short", array)); array = 0; break;
                case 'Z': types.push_back(formatType("bool", array)); array = 0; break;
                case '[': array++; break;
                case 'V': break;
                case 'L':
                    ref = true;
                    v = "";
                    break;
                default:
                    std::cout << "Invalid type: " << c << "\n";
                    throw "Invalid type: " + c;
            }
        }
    }
    return types;
}

std::pair<std::string,std::string> parseParams(std::string f)
{
    std::string params = "";
    std::string ret = "";
    bool bracket = false;
    bool retval = false;
    for (auto c : f) {
        if (c == '(') bracket = true;
        else if (c == ')') {
            bracket = false;
            retval = true;
        } else if (bracket) {
            params += c;
        } else if (retval) {
            ret += c;
        }
    }
    return std::pair<std::string,std::string>(params, ret);
}
