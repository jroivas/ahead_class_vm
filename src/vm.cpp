#include "vm.h"
#include <iostream>
#include <sstream>
#include <typeinfo>

#include <cstring>
#include <cassert>
#include <time.h>
#include <typeinfo>

#include "utils.h"

enum cmpOp {
    CMP_EQ = 1,
    CMP_NE,
    CMP_GE,
    CMP_GT,
    CMP_LE,
    CMP_LT
};

VM::VM(vmClassFile *_cl, vmStack *_stack)
    : cl(_cl),
    stack(_stack),
    ptr(nullptr),
    _indent(0),
    _temp(0)
{
}

void VM::addClassRef(vmClassFile *cl)
{
    classes.push_back(cl);
}

std::string VM::indent()
{
    std::string r = "";
    for (uint8_t i = 0; i < _indent; ++i) {
        r += "    ";
    }
    return r;
}

std::string VM::fixName(std::string name)
{
    std::string res = "";
    for (auto c : name) {
        if (c == '<' or c == '>') res += '_';
        else if (c == '/') res += '_';
        else res += c;
    }
    return res;
}

std::string VM::transcompile(std::string name, vmCodeAttribute *code)
{
    std::string res = "";
    std::string pre = "";
    std::string fname = fixName(name); 
    if (fname == "main") fname = "class_main";
    pre += indent() + "vmObject *" + fname +  "() {\n";

    iin();
    //pre += indent() + "vmStack *stack = new vmStack(" + std::to_string(code->max_stack) + ");\n";
    res += indent() + "uint32_t stackpos = 0;\n";
    res += indent() + "vmObject *stack[" + std::to_string(code->max_stack) + "];\n";
    for (uint16_t stacks = 0; stacks < code->max_stack; ++stacks) {
        res += indent() + "stack[" + std::to_string(stacks) + "] = nullptr;\n";
    }
    pre += indent() + "vmObject *retVal = nullptr;\n";
    for (uint16_t locals = 0; locals < code->max_locals; ++locals) {
        res += indent() + "vmObject *local" + std::to_string(locals) + " = nullptr;\n";
    }
    /*
    for (uint16_t locals = 0; locals < code->max_locals; ++locals) {
        res += indent() + "vmLocal local" + std::to_string(locals) + ";\n";
    }
    */
    pc = 0;
    ptr = code->code;
    while (pc < code->code_length) {
        res += fname + "goto_target_" + std::to_string(pc) + ":\n";
        uint8_t opcode = fetch();
        res += genCode(opcode, fname);
    }

    for (auto lc : loaded_classes) {
        std::string cname = solveClassObjectName(lc.first);
        pre += indent() +  cname + "* " + lc.second + " = new " + cname + "();\n";
        //pre += indent() +"vmClass *" + lc.second + " = loadClass(\"" + lc.first + "\");\n";
        pre += indent() + "if (!" + lc.second + ") {\n";
        iin();
        pre += indent() + "throw \"Can't load class: " + lc.first +"\";\n";
        din();
        pre += indent() + "}\n";
    }
    for (auto lc : loaded_func) {
        pre += indent() + "FunctionDesc *" + lc.first + " = " + lc.second.first + "->getFunction(\"" + lc.second.second + "\");\n";
        pre += indent() + "if (!" + lc.first + ") {\n";
        iin();
        pre += indent() + "throw \"Invalid function on " + lc.second.first + ": " + lc.second.second + "\";\n";
        din();
        pre += indent() + "}\n";
    }
    loaded_classes.erase(loaded_classes.begin(), loaded_classes.end());
    loaded_func.erase(loaded_func.begin(), loaded_func.end());

    din();
    res += indent() + "}\n";

    return pre + res;
}

std::string VM::genCode(uint8_t opcode, std::string &name)
{
    switch (opcode) {
        case 0x02:
            return gen_iconst(-1);
        case 0x03:
            return gen_iconst(0);
        case 0x04:
            return gen_iconst(1);
        case 0x05:
            return gen_iconst(2);
        case 0x06:
            return gen_iconst(3);
        case 0x07:
            return gen_iconst(4);
        case 0x08:
            return gen_iconst(5);

        case 0x12:
            return gen_ldc();
        case 0x13:
            return gen_ldc_w();
        case 0x14:
            return gen_ldc_w();
        case 0x19:
            return gen_aload();
        case 0x1a:
            return gen_iload(0);
        case 0x1b:
            return gen_iload(1);
        case 0x1c:
            return gen_iload(2);
        case 0x1d:
            return gen_iload(3);
        case 0x1e:
            return gen_lload(0);
        case 0x1f:
            return gen_lload(1);
        case 0x20:
            return gen_lload(2);
        case 0x21:
            return gen_lload(3);

        case 0x2a:
            return indent() + "stack[stackpos++] = local0;\n";
            //return indent() + "stack->push(&local0);\n";

        case 0x3a:
            return gen_astore_idx();
        case 0x3b:
            return gen_istore(0);
        case 0x3c:
            return gen_istore(1);
        case 0x3d:
            return gen_istore(2);
        case 0x3e:
            return gen_istore(3);
        case 0x3f:
            return gen_lstore(0);
        case 0x40:
            return gen_lstore(1);
        case 0x41:
            return gen_lstore(2);
        case 0x42:
            return gen_lstore(3);
        case 0x59:
            return gen_dup();

        case 0x65:
            return gen_lsub();
        case 0x6b:
            return gen_dmul();
        case 0x6f:
            return gen_ddiv();

        case 0x8a:
            return gen_l2d();
        case 0x84:
            return gen_iinc();

        case 0xa2:
            return gen_icmp(CMP_GE, name);
        case 0xa7:
            return gen_goto(name);

        case 0xb1:
            return indent() + "return retVal;\n";
        case 0xb2:
            return gen_getStatic();
        case 0xb6:
            return gen_invokeVirtual();
        case 0xb7:
            return gen_invokeVirtual();
        case 0xb8:
            return gen_invokeStatic();
        case 0xbb:
            return gen_new();
        /*
        case 0xb1:
            // return
            return;
        */
        default:
            std::stringstream s;
            s <<  "Unimplemented: " << std::hex << (uint32_t)opcode << " @" << std::dec << pc << "\n";
            throw s.str();
    }
}

std::string VM::gen_ldc()
{
    return gen_ldc_idx(fetch());
}

std::string VM::gen_ldc_w()
{
    return gen_ldc_idx(fetch16());
}

std::string VM::gen_ldc_idx(uint16_t idx)
{
    // Constant create objects, use here as-is
    vmConstantInfo *tmp = cl->constant_pool[idx];

    switch (tmp->tag) {
        case C_Integer:
            return indent() + "stack[stackpos++] = new vmInteger(" + std::to_string(((vmConstantInteger*)tmp)->val.val) + ");\n";
            //return indent() + "stack->pushInteger(" + std::to_string(((vmConstantInteger*)tmp)->val.val) + ");\n";
        case C_Long:
            return indent() + "stack[stackpos++] = new vmLong(" + std::to_string(((vmConstantLong*)tmp)->val.val) + ");\n";
            //return indent() + "stack->pushLong(" + std::to_string(((vmConstantLong*)tmp)->val.val) + ");\n";
        case C_Float:
            return indent() + "stack[stackpos++] = new vmFloat(" + std::to_string(((vmConstantFloat*)tmp)->val.val) + ");\n";
            //return indent() + "stack->pushFloat(" + std::to_string(((vmConstantFloat*)tmp)->val.val) + ");\n";
        case C_Double:
            return indent() + "stack[stackpos++] = new vmDouble(" + std::to_string(((vmConstantDouble*)tmp)->val.val) + ");\n";
            //return indent() + "stack->pushDouble(" + std::to_string(((vmConstantDouble*)tmp)->val.val) +");\n";
        case C_String: {
            return indent() + "stack[stackpos++] = new vmString(\"" + (parseRefUtf8((vmConstantString*)tmp)->str()) +"\");\n";
            //return indent() + "stack->pushObject(new vmString(\"" + (parseRefUtf8((vmConstantString*)tmp)->str()) +"\"));\n";
        }
        default:
            std::cout <<  "Invalid object: " << idx << " " << tmp << "\n";
            throw "Invalid object";
    }
}

std::string VM::gen_iconst(int16_t val)
{
    return indent() + "stack[stackpos++] = new vmInteger(" + std::to_string(val) + ");\n";
    //return indent() + "stack->pushInteger(" + std::to_string(val) + ");\n";
}

std::string VM::gen_iload(int16_t index)
{
    return indent() + "stack[stackpos++] = local" + std::to_string(index) + ";\n";
    //return indent() + "stack->pushInteger(local" + std::to_string(index) + ".val_int);\n";
}

std::string VM::gen_aload()
{
    uint8_t index = fetch();
    return indent() + "stack[stackpos++] = local" + std::to_string(index) + ";\n";
    //return indent() + "stack->pushObject(local" + std::to_string(index) + ".val_obj);\n";
}

std::string VM::gen_astore_idx()
{
    uint8_t idx = fetch();
    // FIXME
    return indent() + "local" + std::to_string(idx) + " = stack[--stackpos];\n";
    //return indent() + "local" + std::to_string(idx) + ".val_obj = stack->pop();\n";
}

std::string VM::gen_istore(int16_t index)
{
    return indent() + "local" + std::to_string(index) + " = stack[--stackpos];\n";
    //return indent() + "local" + std::to_string(index) + ".val_int = stack->popInteger();\n";
}

std::string VM::gen_lload(int16_t index)
{
    return indent() + "stack[stackpos++] = local" + std::to_string(index) + ";\n";
    //return indent() + "stack->pushLong(local" + std::to_string(index) + ".val_long);\n";
}

std::string VM::gen_lstore(int16_t index)
{
    //return indent() + "local" + std::to_string(index) + ".val_long = stack->popLong();\n";
    return indent() + "local" + std::to_string(index) + " = stack[--stackpos];\n";
}

std::string VM::gen_l2d()
{
    /*
    return indent() + "{\n";
    return indent() + "vmDouble *d = new vmDouble(vmLong::castFrom(stack[--stackpos])->val);\n";
    return indent() + "stack[stackpos++] = d;\n";
    return indent() + "}\n";
    */
    return indent() + "stack[stackpos - 1] = new vmDouble(vmLong::castFrom(stack[stackpos - 1])->val);\n";

    //return indent() + "stack[stackpos] = new vmDouble(vmLong::castFrom(stack[stackpos])->val);\n";
    //res += indent() + "stack->pushDouble(stack->popLong());\n";
    //std::string res = "";
    //return res;
}

std::string VM::gen_iinc()
{
    uint8_t index = fetch();
    uint8_t val = fetch();
    // FIXME
    return indent() + "vmInteger::castFrom(local" + std::to_string(index) + ")->val += " +std::to_string(val) + ";\n";
    //return indent() + "local" + std::to_string(index) + ".val_int += " +std::to_string(val) + ";\n";
}

std::string VM::gen_lsub()
{
    std::string res = "";

    res += indent() + "{\n";
    iin();
    res += indent() + "vmLong *v1 = vmLong::castFrom(stack[--stackpos]);\n";
    res += indent() + "vmLong *v2 = vmLong::castFrom(stack[--stackpos]);\n";
    res += indent() + "stack[stackpos++] = new vmLong(v2->val - v1->val);\n";
/*
    res += indent() + "nLong v1 = stack->popLong();\n";
    res += indent() + "nLong v2 = stack->popLong();\n";
    res += indent() + "stack->pushLong(v2 - v1);\n";
*/
    din();
    res += indent() + "}\n";
    /*
    res += indent() + "vmLong v1 = toLong(stack->pop());\n";
    res += indent() + "vmLong v2 = toLong(stack->pop());\n";
    res += indent() + "stack->push(vmLong(v2.val - v1.val));\n";
    */

    return res;
}

std::string VM::gen_dmul()
{
    std::string res = "";

    res += indent() + "vmDouble *v1 = vmDouble::castFrom(stack[--stackpos]);\n";
    res += indent() + "vmDouble *v2 = vmDouble::castFrom(stack[--stackpos]);\n";
    res += indent() + "stack[stackpos++] = new vmDouble(v2->val * v1->val);\n";
    //res += indent() + "stack->pushDouble(stack->popDouble() * stack->popDouble());\n";

    return res;
}

std::string VM::gen_ddiv()
{
    std::string res = "";

    
    res += indent() + "{\n";
    iin();
    /*
    res += indent() + "double v1 = stack->popDouble();\n";
    res += indent() + "double v2 = stack->popDouble();\n";
    res += indent() + "if (v1 == 0) throw \"Divide by zero\";\n";
    res += indent() + "stack->pushDouble(v2 / v1);\n";
    */
    res += indent() + "vmDouble *v1 = vmDouble::castFrom(stack[--stackpos]);\n";
    res += indent() + "vmDouble *v2 = vmDouble::castFrom(stack[--stackpos]);\n";
    res += indent() + "if (v1->val == 0) throw \"Divide by zero\";\n";
    res += indent() + "stack[stackpos++] = new vmDouble(v2->val / v1->val);\n";
    din();
    res += indent() + "}\n";

    return res;
}

std::string VM::gen_icmp(uint8_t oper, std::string &name)
{
    int16_t target = fetch16();
    // 8 bit opcode + 16 bit addr = -3
    target = pc + target - 3;  

    std::string res = "";
    res += indent() + "{\n";
    iin();
    res += indent() + "nInteger v1 = vmInteger::castFrom(stack[--stackpos])->val;\n";
    res += indent() + "nInteger v2 = vmInteger::castFrom(stack[--stackpos])->val;\n";
    //res += indent() + "nInteger v2 = stack->popInteger();\n";

    switch (oper) {
        case CMP_EQ:
            res += indent() + "if (v2 == v1) goto " + name + "goto_target_" + std::to_string(target) +";\n";
            break;
        case CMP_NE:
            res += indent() + "if (v2 != v1) goto " + name + "goto_target_" + std::to_string(target) +";\n";
            break;
        case CMP_GE:
            res += indent() + "if (v2 >= v1) goto " + name + "goto_target_" + std::to_string(target) +";\n";
            break;
        case CMP_GT:
            res += indent() + "if (v2 > v1) goto " + name + "goto_target_" + std::to_string(target) +";\n";
            break;
        case CMP_LE:
            res += indent() + "if (v2 <= v1) goto " + name + "goto_target_" + std::to_string(target) +";\n";
            break;
        case CMP_LT:
            res += indent() + "if (v2 < v1) goto " + name + "goto_target_" + std::to_string(target) +";\n";
            break;
        default:
            throw "Invalid operation";
    }
    din();
    res += indent() + "}\n";

    return res;
}

std::string VM::gen_goto(std::string &name)
{
    int16_t target = fetch16();
    // 8 bit opcode + 16 bit addr = -3
    target = pc + target - 3;  
    return indent() + "goto " + name + "goto_target_" + std::to_string(target) + ";\n";
}

std::string VM::gen_dup()
{
    std::string res;
    res += indent() + "{\n";
    iin();
    res += indent() + "stack[stackpos] = stack[stackpos - 1];\n";
    res += indent() + "++stackpos;\n";
    din();
    res += indent() + "}\n";
    //res += indent() + "stack->dup();\n";
    return res;
}

std::string VM::gen_new()
{
    int16_t idx = fetch16();

    vmConstantClass *classref = parseClassConstant(idx);
    vmConstantUtf8 *classname = parseRefUtf8(classref);

    std::string res;
    res += indent() + "{\n";
    iin();
    std::string n = fixName(classname->str());
    loaded_classes[classname->str()] = n;
    //res += indent() + "vmClass *c = loadClass(\"" + classname->str() + "\");\n";
    //res += indent() + "vmClass *c = loadClass(\"" + classname->str() + "\");\n";
    //res += indent() + "stack->push(c->newInstance());\n";
    //res += indent() + "stack->push(" + n + "->newInstance());\n";
    res += indent() + "stack[stackpos++] = " + n + "->newInstance();\n";
    din();
    res += indent() + "}\n";
    return res;
}

std::string VM::gen_invokeStatic()
{
    uint16_t idx = fetch16();

    vmConstantRef *method = parseRef(idx);
    vmConstantClass *classref = parseRefClass(method);
    vmConstantNameAndType *nametype = parseRefNameType(method);
    vmConstantUtf8 *classname = parseRefUtf8(classref);

    std::string res = "";
    bool ok = false;

    if (classname->str() == "java/lang/System") {
        if (((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() == "currentTimeMillis") {
        res += indent() + "{\n";
        iin();
        res += indent() + "struct timespec tt_" + std::to_string(++_temp) + ";\n";
        res += indent() + "clock_gettime(CLOCK_REALTIME, &tt_" + std::to_string(_temp) + ");\n";
        //res += indent() + "stack->pushLong(tt_" + std::to_string(_temp) + ".tv_sec * 1000 + tt_" + std::to_string(_temp) + ".tv_nsec / 1000000);\n";
        res += indent() + "stack[stackpos++] = new vmLong(tt_" + std::to_string(_temp) + ".tv_sec * 1000 + tt_" + std::to_string(_temp) + ".tv_nsec / 1000000);\n";
        din();
        res += indent() + "}\n";
        //res += indent() + "stack->push(new vmLong(tt.tv_sec * 1000 + tt.tv_nsec / 1000000));\n";
        ok = true;
        }
    }
    if (!ok) {
        throw "Invalid call: " + classname->str() + " " + ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str();
    }
    return res;
}

std::string VM::gen_getStatic()
{
    uint16_t idx = fetch16();

    vmConstantRef *method = parseRef(idx);
    vmConstantClass *classref = parseRefClass(method);
    vmConstantNameAndType *nametype = parseRefNameType(method);
    vmConstantUtf8 *classname = parseRefUtf8(classref);

    std::string res = "";
    // FIXME
    if (classname->str() == "java/lang/System") {
        if (((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() == "out") {
            if (((vmConstantUtf8 *)(nametype->resolve2(cl->constant_pool)))->str() == "Ljava/io/PrintStream;") {
                res += indent() + "{\n";
                iin();
                std::string n = "java_io_PrintStream";
                loaded_classes["java/io/PrintStream"] = n;
                /*
                //res += indent() + "vmClass *cl = loadClass(\"java/io/PrintStream\");\n";
                res += indent() + "if (!cl) throw \"Can't load class\";\n";
                res += indent() + "stack->push(cl);\n";
                */
                //res += indent() + "stack->push(" + n + ");\n";
                res += indent() + "stack[stackpos++] = " + n + ";\n";
                din();
                res += indent() + "}\n";
                return res;
            }
        }
    }
    throw "Invalid static";
}

std::string VM::gen_invokeVirtual()
{
    uint16_t idx = fetch16();

    vmConstantRef *method = parseRef(idx);
    vmConstantClass *classref = parseRefClass(method);
    vmConstantNameAndType *nametype = parseRefNameType(method);
    vmConstantUtf8 *classname = parseRefUtf8(classref);

    loadstack.push_back(classname->str());
    std::string res = "";
    //uint32_t ii = ++_temp;
    //uint32_t fi = ++_temp;
    res += indent() + "{\n";
    iin();
    std::string n = fixName(classname->str());
    loaded_classes[classname->str()] = n;

    std::string fname = ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str();
    std::string desc = ((vmConstantUtf8 *)(nametype->resolve2(cl->constant_pool)))->str();
    // FIXME class needs to be parsed, should pause and search for rest of
    // the search path for it if not found or parsed yet
    vmClass *inst = loadClass(classname->str());
    if (!inst) throw "Invalid class: " + classname->str();

    FunctionDesc *fdesc = inst->getFunction(fname, desc);
    if (!fdesc) throw "Invalid method: " + fname + " "+ desc + " on " + classname->str();

    std::string c = fixName(fname);
    // FIXME
    //for (uint32_t pi = 0; pi < fdesc->params.size(); ++pi) {
    for (uint32_t pi = fdesc->params.size(); pi > 0; --pi) {
        std::string pname = fdesc->params[pi - 1];
        std::string ptype = typeToNative(pname);
        //res += indent() + ptype + " p" + std::to_string(pi - 1) + " = (" + ptype +" )stack->pop();\n";
        res += indent() + ptype + " p" + std::to_string(pi - 1) + " = (" + ptype +" )stack[--stackpos];\n";
    }

    std::string rettype = "";
    if (!fdesc->returnType.empty()) {
        rettype = typeToNative(fdesc->returnType) + " retVal = ";
    }
    //res += indent() + "vmClassInstance *thiz = vmClassInstance::castFrom(stack->pop());\n";
    res += indent() + "vmClassInstance *thiz = vmClassInstance::castFrom(stack[--stackpos]);\n";
    res += indent() + rettype + n + "->" + c + "(";
    res += "thiz";
    for (uint32_t pi = 0; pi < fdesc->params.size(); ++pi) {
        res += ", ";
        res += "p" + std::to_string(pi);
    }
    res += ");\n";
    if (!rettype.empty()) {
        //res += indent() + "stack->push(retVal);\n";
        res += indent() + "stack[stackpos++] = retVal;\n";
    }
    //res += indent() + c + "->func(" + n + ", stack);\n";
    /*
    std::string c = n + "__" + fixName(f);
    loaded_func[c] = std::pair<std::string, std::string>(n, f);

    res += indent() + c + "->func(" + n + ", stack);\n";
    */

    /*
    res += indent() + "FunctionDesc *__f" + std::to_string(fi) + " = " + n + "->getFunction(\"" + ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str()+ "\");\n";
    res += indent() + "if (!__f" + std::to_string(fi) + ") {\n";
    iin();
    res += indent() + "throw \"Invalid function on " + classname->str() + ": " + ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() + "\";\n";
    din();
    res += indent() + "}\n";
    res += indent() + "__f" + std::to_string(fi) + "->func(" + n + ", stack);\n";
    */

    /*
    res += indent() + "vmClass *__inst" + std::to_string(ii) + " = loadClass(\"" + classname->str() + "\");\n";
    res += indent() + "if (__inst" + std::to_string(ii) + ") {\n";
    iin();
    res += indent() + "FunctionDesc *__f" + std::to_string(fi) + " = __inst" + std::to_string(ii) + "->getFunction(\"" + ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str()+ "\");\n";
    res += indent() + "if (!__f" + std::to_string(fi) + ") {\n";
    iin();
    res += indent() + "throw \"Invalid function on " + classname->str() + ": " + ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() + "\";\n";
    din();
    res += indent() + "}\n";
    res += indent() + "__f" + std::to_string(fi) + "->func(__inst" + std::to_string(ii) + ", stack);\n";
    din();

    res += indent() + "} else {\n";
    iin();
    res += indent() + "throw \"Invalid class: " + classname->str() + "\";\n";
    din();
    res += indent() + "}\n";
    */
    din();
    res += indent() + "}\n";
    return res;
}

bool VM::execute(vmCodeAttribute *code)
{
    assert(code != nullptr);
    std::cout << " MaxStack " << code->max_stack << "\n";
    std::cout << " MaxLocals " << code->max_locals << "\n";

    pc = 0;
    ptr = code->code;
    while (pc < code->code_length) {
        //std::cout << " PC " << pc << "  " << code->code_length <<"\n";
        uint8_t opcode = fetch();
        //std::cout << " opcd " << pc << " " << std::hex << (long)opcode << std::dec <<"\n";
        decode(opcode);
    }

    return true;
}

uint8_t VM::fetch()
{
    return ptr[pc++];
}

uint16_t VM::fetch16()
{
    uint16_t idx = read16(ptr + pc);
    pc += 2;
    return idx;
}

void VM::decode(uint8_t opcode)
{
    switch (opcode) {
        case 0x02:
            stack->push(new vmInteger(-1));
            break;
        case 0x03:
            stack->push(new vmInteger(0));
            break;
        case 0x04:
            stack->push(new vmInteger(1));
            break;
        case 0x05:
            stack->push(new vmInteger(2));
            break;
        case 0x06:
            stack->push(new vmInteger(3));
            break;
        case 0x07:
            stack->push(new vmInteger(4));
            break;
        case 0x08:
            stack->push(new vmInteger(5));
            break;
        case 0x12:
            ldc();
            break;
        case 0x13:
            ldc_w();
            break;
        case 0x14:
            ldc_w();
            break;
        case 0x19:
            aload();
            break;
        case 0x1a:
            iload(0);
            break;
        case 0x1b:
            iload(1);
            break;
        case 0x1c:
            iload(2);
            break;
        case 0x1d:
            iload(3);
            break;
        case 0x1e:
            lload(0);
            break;
        case 0x1f:
            lload(1);
            break;
        case 0x20:
            lload(2);
            break;
        case 0x21:
            lload(3);
            break;
        case 0x2a:
            if (locals.empty()) {
                //solve "this" object from cl
                //locals.push_back(new vmObject(cl));
            }
            stack->push(locals[0]);
            break;
        case 0x3a:
            astore_idx();
            break;
        case 0x3b:
            istore(0);
            break;
        case 0x3c:
            istore(1);
            break;
        case 0x3d:
            istore(2);
            break;
        case 0x3e:
            istore(3);
            break;
        case 0x3f:
            lstore(0);
            break;
        case 0x40:
            lstore(1);
            break;
        case 0x41:
            lstore(2);
            break;
        case 0x42:
            lstore(3);
            break;
        case 0x59:
            dup();
            break;
        case 0x65:
            lsub();
            break;
        case 0x6b:
            dmul();
            break;
        case 0x6f:
            ddiv();
            break;
        case 0x8a:
            l2d();
            break;
        case 0x84:
            iinc();
            break;
        case 0xa2:
            icmp(CMP_GE);
            break;
        case 0xa7:
            vm_goto();
            break;
        case 0xb1:
            // return
            return;
        case 0xb2:
            getStatic();
            break;
        case 0xb6:
            invokeVirtual();
            break;
        case 0xb7:
            invokeSpecial();
            break;
        case 0xb8:
            invokeStatic();
            break;
        case 0xbb:
            vm_new();
            break;
        default:
            /*
            std::cout <<  "Unimplemented: " << std::hex << (uint32_t)opcode << " @" << std::dec << pc << "\n";
            throw "Unimplemented: " + std::to_string((uint32_t)opcode);
            */
            std::stringstream s;
            s <<  "Unimplemented: " << std::hex << (uint32_t)opcode << " @" << std::dec << pc << "\n";
            throw s.str();
    }
}

vmConstantRef *VM::parseRef(uint16_t idx)
{
    vmConstantInfo *tmp = cl->constant_pool[idx];
    if (tmp->tag != C_FieldRef &&
        tmp->tag != C_MethodRef &&
        tmp->tag != C_InterfaceMethodRef) {
        std::cout << "Invalid reference " << idx << "\n";
        throw "invalid reference";
    }
    return static_cast<vmConstantRef *>(tmp);
}

vmConstantClass *VM::parseClassConstant(uint16_t idx)
{
    vmConstantInfo *tmp = cl->constant_pool[idx];
    if (tmp->tag != C_Class) {
        std::cout << "Invalid reference\n";
        throw "invalid reference";
    }
    return static_cast<vmConstantClass *>(tmp);
}

vmConstantClass *VM::parseRefClass(vmConstantRef *p)
{
    vmConstantInfo *tmp = p->resolve(cl->constant_pool);
    if (tmp->tag != C_Class) {
        std::cout << "Invalid reference\n";
        throw "invalid reference";
    }
    return static_cast<vmConstantClass *>(tmp);
}

vmConstantNameAndType *VM::parseRefNameType(vmConstantRef *p)
{
    vmConstantInfo *tmp = p->resolve2(cl->constant_pool);
    if (tmp->tag != C_NameAndType) {
        std::cout << "Invalid reference\n";
        throw "invalid reference";
    }
    return static_cast<vmConstantNameAndType *>(tmp);
}

vmConstantUtf8 *VM::parseRefUtf8(vmConstantInfo *p)
{
    vmConstantInfo *tmp = p->resolve(cl->constant_pool);
    if (tmp->tag != C_Utf8) {
        std::cout << "Invalid reference\n";
        throw "invalid reference";
    }
    return static_cast<vmConstantUtf8 *>(tmp);
}

void VM::invokeSpecial()
{
    invokeVirtual();
    /*
    return;
    uint16_t idx = read16(ptr + pc);
    pc += 2;

    vmConstantRef *method = parseRef(idx);
    vmConstantClass *classref = parseRefClass(method);
    vmConstantNameAndType *nametype = parseRefNameType(method);
    vmConstantUtf8 *classname = parseRefUtf8(classref);

    vmObject *objectRef = stack->pop();
    std::cout << " Invoke special " << classname->str() << "\n";
    std::cout << " Name  " << ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() << "\n";
    std::cout << " Type  " << ((vmConstantUtf8 *)(nametype->resolve2(cl->constant_pool)))->str() << "\n";

    if (classname->str() == "java/lang/Object") {
        if (((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() == "<init>") {
            stack->push(new vmObject());
            return;
        }
    }
    if (objectRef->type == TYPE_CLASS) {
        vmClass *inst = static_cast<vmClass*>(objectRef);


        auto f = inst->getFunction(((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str());
        FunctionDesc *f = inst->getFunction(fname);
        //std::cout << "FF " << f << " " << classname->str() << " " << fname << "\n";
        if (!f) {
            std::cout <<  "Invalid function on " + classname->str() + ": " + ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() << "\n";
            throw "Invalid function on " + classname->str() + ": " + ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str();
        }
        if (!f->init) {
            f->parse(((vmConstantUtf8 *)(nametype->resolve2(cl->constant_pool)))->str());
        }
        if (!f) {
            throw "Invalid function on " + classname->str() + ": " + ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str();
        }
        f->func(inst, stack);
        return;
    }

    std::cout << " Invoke class " << classname->str() << "\n";
    std::cout << " Name  " << ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() << "\n";
    std::cout << " Type  " << ((vmConstantUtf8 *)(nametype->resolve2(cl->constant_pool)))->str() << "\n";
    throw "Invalid call";
    */
}

void VM::invokeVirtual()
{
    uint16_t idx = read16(ptr + pc);
    pc += 2;

    vmConstantRef *method = parseRef(idx);
    vmConstantClass *classref = parseRefClass(method);
    vmConstantNameAndType *nametype = parseRefNameType(method);
    vmConstantUtf8 *classname = parseRefUtf8(classref);

    if (classname->str() == "java/lang/Object") {
        if (((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() == "<init>") {
            stack->push(new vmObject());
            return;
        }
    }

    vmClass *inst = loadClass(classname->str());
    if (inst) {
        std::string fname = ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str();
        FunctionDesc *f = inst->getFunction(fname);
        //std::cout << "FF " << f << " " << classname->str() << " " << fname << "\n";
        if (!f) {
            std::cout <<  "Invalid function on " + classname->str() + ": " + ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() << "\n";
            throw "Invalid function on " + classname->str() + ": " + ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str();
        }
        /*
        if (!f->init) {
            f->parse(((vmConstantUtf8 *)(nametype->resolve2(cl->constant_pool)))->str());
        }
        */
        // FIXME constructing new stack
        /*
        vmStack *st = new vmStack();
        for (auto p : f->params) {
            st->insert(stack->pop());
        }

        vmObject *objectRef = stack->pop();
        st->insert(objectRef);
        */

        // Call
        f->func(inst, stack);
        /*
        f->func(st);
        if (f->returnType != "") {
            stack->push(st->pop());
        }
        */
        return;
    }

    //std::cout << " obj " << typeName(objectRef) << "\n";
    std::cout << " Invoke class " << classname->str() << "\n";
    std::cout << " Name  " << ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() << "\n";
    std::cout << " Type  " << ((vmConstantUtf8 *)(nametype->resolve2(cl->constant_pool)))->str() << "\n";
    throw "Invalid virtual call";
}

void VM::invokeStatic()
{
    uint16_t idx = read16(ptr + pc);
    pc += 2;

    vmConstantRef *method = parseRef(idx);
    vmConstantClass *classref = parseRefClass(method);
    vmConstantNameAndType *nametype = parseRefNameType(method);
    vmConstantUtf8 *classname = parseRefUtf8(classref);

    if (classname->str() == "java/lang/System") {
        if (((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() == "currentTimeMillis") {
            struct timespec tt;
            clock_gettime(CLOCK_REALTIME, &tt);
            stack->push(new vmLong(tt.tv_sec * 1000 + tt.tv_nsec / 1000000));
            /*
            vmLong *v = new vmLong(tt.tv_sec * 1000 + tt.tv_nsec / 1000000);
            std::cout << "TIMEMILLI " << v << " " << v->val << "\n";
            stack->push(v);
            */
            return;
        }
    }

    std::cout << " Invoke class " << classname->str() << "\n";
    std::cout << " Name  " << ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() << "\n";
    std::cout << " Type  " << ((vmConstantUtf8 *)(nametype->resolve2(cl->constant_pool)))->str() << "\n";
    throw "Invalid call";
}

void VM::getStatic()
{
    uint16_t idx = read16(ptr + pc);
    pc += 2;

    vmConstantRef *method = parseRef(idx);
    vmConstantClass *classref = parseRefClass(method);
    vmConstantNameAndType *nametype = parseRefNameType(method);
    vmConstantUtf8 *classname = parseRefUtf8(classref);

    if (classname->str() == "java/lang/System") {
        if (((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() == "out") {
            if (((vmConstantUtf8 *)(nametype->resolve2(cl->constant_pool)))->str() == "Ljava/io/PrintStream;") {
                vmClass *cl = loadClass("java/io/PrintStream");
                //cl->val = ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str();
                cl->val = "out";
                if (!cl) {
                    throw "Can't load class: java/io/PrintStream";
                }
                stack->push(cl);
                //stack->push(new SystemPrinter());
                return;
            }
        }
    }

    std::cout << " getStatic class " << classname->str() << "\n";
    std::cout << " Name  " << ((vmConstantUtf8 *)(nametype->resolve(cl->constant_pool)))->str() << "\n";
    std::cout << " Type  " << ((vmConstantUtf8 *)(nametype->resolve2(cl->constant_pool)))->str() << "\n";
    throw "Invalid static";
}


void VM::ldc()
{
    uint16_t idx = *(ptr + pc);
    pc += 1;
    ldc_idx(idx);
}

void VM::ldc_w()
{
    uint16_t idx = read16(ptr + pc);
    pc += 2;
    ldc_idx(idx);
}

void VM::ldc_idx(uint16_t idx)
{
    // Constant create objects, use here as-is
    vmConstantInfo *tmp = cl->constant_pool[idx];

    switch (tmp->tag) {
        case C_Integer:
            stack->push(&((vmConstantInteger*)tmp)->val);
            //stack->push(new vmInteger(((vmConstantInteger*)tmp)->val));
            break;
        case C_Long:
            stack->push(&((vmConstantLong*)tmp)->val);
            break;
        case C_Float:
            stack->push(&((vmConstantFloat*)tmp)->val);
            break;
        case C_Double:
            stack->push(&((vmConstantDouble*)tmp)->val);
            break;
        case C_String: {
            vmString *s = &(parseRefUtf8((vmConstantString*)tmp)->val);
            stack->push(s);
            //vmConstantUtf8 *n = parseRefUtf8((vmConstantString*)tmp);
            //stack->push(new vmString(n->str()));
            break;
        }
        default:
            std::cout <<  "Invalid object: " << idx << " " << tmp << "\n";
            throw "Invalid object";
            break;
    }
}

void VM::lstore(uint8_t index)
{
    vmObject *d = stack->pop();
    locals[index] = toLong(d);
}

void VM::istore(uint8_t index)
{
    locals[index] = toInteger(stack->pop());;
}

void VM::astore_idx()
{
    uint8_t index = *(ptr + pc);
    pc++;

    locals[index] = stack->pop();
}


vmInteger *VM::toInteger(vmObject *d)
{
    switch (d->type) {
        case TYPE_INTEGER: return static_cast<vmInteger*>(d);
        //case TYPE_INTEGER: return new vmInteger(static_cast<vmInteger*>(d)->val);
        case TYPE_LONG: return new vmInteger(((vmLong*)d)->val);
        case TYPE_FLOAT: return new vmInteger(((vmFloat*)d)->val);
        case TYPE_DOUBLE: return new vmInteger(((vmDouble*)d)->val);
        case TYPE_REF: std::cout << " REF "  << "\n"; return toInteger(((vmRef*)d)->val);
        default:
            std::cout << " PC " << pc << "\n";
            std::cout << " Convert from " << typeName(d) << "\n";
            throw "Invalid conversion";
    }
}

vmLong *VM::toLong(vmObject *d)
{
    switch (d->type) {
        case TYPE_LONG: return static_cast<vmLong*>(d);
        case TYPE_INTEGER: return new vmLong(((vmInteger*)d)->val);
        case TYPE_FLOAT: return new vmLong(((vmFloat*)d)->val);
        case TYPE_DOUBLE: return new vmLong(((vmDouble*)d)->val);
        case TYPE_REF: return toLong(((vmRef*)d)->val);
        default:
            std::cout << "PC " << pc << "\n";
            std::cout << "Convert from " << typeName(d) << "\n";
            throw "Invalid conversion";
    }
}

vmDouble *VM::toDouble(vmObject *d)
{
    switch (d->type) {
        case TYPE_DOUBLE: return static_cast<vmDouble*>(d);
        case TYPE_LONG: return new vmDouble(((vmInteger*)d)->val);
        case TYPE_FLOAT: return new vmDouble(((vmFloat*)d)->val);
        case TYPE_INTEGER: return new vmDouble(((vmInteger*)d)->val);
        case TYPE_REF: return toDouble(((vmRef*)d)->val);
        default:
            std::cout << "PC " << pc << "\n";
            std::cout << "Convert from " << typeName(d) << "\n";
            throw "Invalid conversion";
    }
}

void VM::aload()
{
    uint8_t index = *(ptr + pc);
    pc++;
    //stack->push(new vmRef(locals[index]));
    stack->push(locals[index]);
}

void VM::iload(uint8_t index)
{
    stack->push(toInteger(locals[index]));
}

void VM::lload(uint8_t index)
{
    stack->push(toLong(locals[index]));
}

void VM::icmp(uint8_t oper)
{
    vmInteger *v1 = toInteger(stack->pop());
    vmInteger *v2 = toInteger(stack->pop());

    //std::cout << " CMP " << v2->val << " " << oper << " " << v1->val << "\n";

    switch (oper) {
        case CMP_EQ:
            if (v2->val == v1->val) return vm_goto();
            break;
        case CMP_NE:
            if (v2->val != v1->val) return vm_goto();
            break;
        case CMP_GE:
            if (v2->val >= v1->val) return vm_goto();
            break;
        case CMP_GT:
            if (v2->val > v1->val) return vm_goto();
            break;
        case CMP_LE:
            if (v2->val <= v1->val) return vm_goto();
            break;
        case CMP_LT:
            if (v2->val < v1->val) return vm_goto();
            break;
        default:
            break;
    }
    pc += 2;
}

void VM::iinc()
{
    uint8_t index = *(ptr + pc);
    pc++;
    int8_t val = *(ptr + pc);
    pc++;

    toInteger(locals[index])->val += val;
    //((vmInteger*)locals[index])->val += val;
}

void VM::vm_goto()
{
    int16_t target = read16(ptr + pc);
    //std::cout << "TARGET: " << target << " PC " << pc << "\n";
    pc += target - 1;
    //std::cout << " TARGET PC " << pc << "\n";
}

void VM::lsub()
{
    vmLong *v1 = toLong(stack->pop());
    vmLong *v2 = toLong(stack->pop());
    //std::cout << " lsub " << v2->val <<  " " << v1->val <<  " " << (v2->val - v1->val) << "\n";
    stack->push(new vmLong(v2->val - v1->val));
}

void VM::ddiv()
{
    vmDouble *v1 = toDouble(stack->pop());
    vmDouble *v2 = toDouble(stack->pop());
    if (v1->val == 0) {
        throw "Divide by zero";
    }
    stack->push(new vmDouble(v2->val / v1->val));
}

void VM::dmul()
{
    vmDouble *v1 = toDouble(stack->pop());
    vmDouble *v2 = toDouble(stack->pop());
    stack->push(new vmDouble(v2->val * v1->val));
}

void VM::vm_new()
{
    int16_t idx = read16(ptr + pc);
    pc += 2;

    // FIXME
    vmConstantClass *classref = parseClassConstant(idx);
    vmConstantUtf8 *classname = parseRefUtf8(classref);
    //std::cout << " New class type " << classname->bytes << "\n";
    vmClass *c = loadClass(classname->str());
    //std::cout << " New class base " << c << "\n";
    stack->push(c->newInstance());
}

void VM::dup()
{
    vmObject *o = stack->peek();
    stack->push(o);
}

void VM::l2d()
{
    vmLong *l = toLong(stack->pop());
    stack->push(new vmDouble(l->val));
}
