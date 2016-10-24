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
        }
    }
    return "INVALID";
}

std::function<void(vmStack*)> vmClass::getFunction(std::string name)
{
    for (auto m : methods) {
        if (m.first == name) return m.second;
    }
    return nullptr;
}

std::string formatType(std::string val, uint32_t array)
{
    for (uint32_t i = 0; i < array) {
        // FIXME
        val+= "[]";
    }
    return val;
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
                case 'L':
                    ref = true;
                    v = "";
                    break;
                default:
                    throw "Invalid type: " + c;
            }
        }
    }
    return types;
}
