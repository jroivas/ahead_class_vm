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
