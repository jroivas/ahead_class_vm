#include <iostream>
#include <classloader.h>
#include <cstring>
#include <algorithm>
#include <vm.h>

#include <lib/runtime.h>
#include <vm_object.h>
/*
#include <lib/stringbuilder.h>
#include <lib/printstream.h>
*/

int main(int argc, char **argv)
{
    if (argc <= 1) {
        std::cout << "Usage: " << argv[0] << " classfile\n";
        return 1;
    }

    vmClassFile m(argv[1]);

    bool ok = m.parse();
    if (!ok) {
        std::cout << "ERROR: Parsing of " << argv[1] << " failed!\n";
        return 2;
    }
    std::cout << "/*\n";
    std::cout << "Parse? " << (ok ? "ok" :"fail") << "\n";
    std::cout << "Version " << m.major_version << " " << m.minor_version << "\n";
    std::cout << "Access flags " << m.access_flags << "\n";
    std::cout << "This " << m.this_class << "\n";
    vmConstantClass *this_class = static_cast<vmConstantClass*>(m.constant_pool[m.this_class]);
    vmConstantUtf8 *this_name = this_class ? static_cast<vmConstantUtf8*>(m.constant_pool[this_class->index]) : nullptr;
    std::cout << "This class " << (this_name ? this_name->str(): "null") << "\n";
    std::cout << "Super " << m.super_class << "\n";
    std::cout << "Interfaces " << m.interfaces_count << "\n";
    std::cout << "Fields " << m.fields_count << "\n";
    std::cout << "Methods " << m.methods_count << "\n";
    std::cout << "Attrib " << m.attributes_count << "\n";
    std::cout << "*/\n";
    /*
    */
    std::cout << "#include <runtime.h>\n";
    /*
    std::cout << "#include <vm_stack.h>\n";
    std::cout << "#include <dynload.h>\n";
    std::cout << "#include <time.h>\n";
    */
    bool foundMain = false;

    vmStack *st = new vmStack();
    VM *vm = new VM(&m, st);
    // Init
    std::vector<vmClass*> preload;
    preload.push_back(new StringBuilder());
    preload.push_back(new PrintStream());
    preload.push_back(new ClassLangString());
    preload.push_back(new LangObject());

    std::cout << vm->prefixClass(this_name->str());

    for (auto f : m.fields) {
        vmConstantUtf8 *type = static_cast<vmConstantUtf8*>(m.constant_pool[f->desc_index]);
        vmConstantUtf8 *name = static_cast<vmConstantUtf8*>(m.constant_pool[f->name_index]);
        if (type && name) {
            std::cout << typeToNative(parseType(type->str())) << " " << name->str() << ";\n";
        }
    }

    for (auto me : m.methods) {
        uint16_t i = 1;
        vmConstantUtf8 *mu = static_cast<vmConstantUtf8*>(m.constant_pool[me->name_index]);
        if (mu) {
            if (mu->str() == "main") foundMain = true;
            /*
            std::cerr << "Method: " << mu->str() << "\n";
            */
        }
        for (auto a : me->attributes) {
            vmConstantUtf8 *u = static_cast<vmConstantUtf8*>(m.constant_pool[a->name_index]);
            if (u != nullptr) {
                //std::cout << " Section: " << u->str() << "\n";
                //if (strncmp((const char*)u->str(), "Code", 4) == 0) {
                if (u->str().substr(0, 4) == "Code") {
                    vmCodeAttribute *code = new vmCodeAttribute(a);
                    /*
                    std::cout << "CL: " << code->code_length << "\n";
                    for (uint32_t l = 0; l< code->code_length; ++l) {
                        std::cout << " " << std::hex << (unsigned int)code->code[l];
                    }
                    std::cout << std::dec << "\n";
                    */
                    //vm->execute(code);
                    try {
                        std::cout << vm->transcompile(mu->str(), code);
                    }
                    catch (const char *e) {
                        std::cerr << "ERROR: Can't compile: " << e << "\n";
                        exit(1);
                    }
                    catch (std::string e) {
                        std::cerr << "ERROR: Can't compile: " << e << "\n";
                        exit(1);
                    }
                } else {
                    //std::cout << i << " " << u->bytes << "\n";
                    std::cout << "/*\n";
                    for (uint32_t l = 0; l< a->length; ++l) {
                        std::cout << " " << std::hex << (unsigned int)a->info[l];
                    }
                    std::cout << "\n*/\n";
                }
            }
            ++i;
        }
    }
    std::cout << vm->postClass(this_name->str());

    (void)preload;
    std::sort(vm->loadstack.begin(), vm->loadstack.end());
    auto last_class = std::unique(vm->loadstack.begin(), vm->loadstack.end());

    if (foundMain) {

    std::cout << "int main(int argc, char **argv)\n{\n";
    std::cout << "    int ret = 0;\n";
    for (auto i = vm->loadstack.begin(); i < last_class; ++i) {
        std::string name = solveClassObjectName(*i);
        if (!name.empty()) {
            std::cout << "    new " + name + "(); // " + *i + "\n";
        }
    }
    std::cout << "    try {\n";
    std::vector<std::string> namespaces =vm->parseClassPackage(this_name->str());
    std::string namespaceStr = "";
    for (auto ns : namespaces) {
        if (!namespaceStr.empty()) namespaceStr += "::";
        namespaceStr += ns;
    }
    std::string classname =vm->parseClassName(this_name->str());
    std::cout << "        " + classname + " main_class;\n";
    std::cout << "        main_class.class_main();\n";
    std::cout << "    }\n";
    std::cout << "    catch (const char* m) {\n";
    std::cout << "        std::cerr << \"ERROR: \" << m << std::endl;\n";
    std::cout << "    }\n";
    std::cout << "    catch (std::string m) {\n";
    std::cout << "        std::cerr << \"ERROR: \" << m << std::endl;\n";
    std::cout << "    }\n";
    //std::cout << "    malloc_stats_print(NULL, NULL, NULL);\n";
    std::cout << "    return ret;\n";
    std::cout << "}\n";
    }

    return 0;
}
