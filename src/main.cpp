#include <iostream>
#include <classloader.h>
#include <cstring>
#include <vm.h>

#include <lib/stringbuilder.h>
#include <lib/printstream.h>

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
    std::cout << "Parse? " << (ok ? "ok" :"fail") << "\n";
    std::cout << "Version " << m.major_version << " " << m.minor_version << "\n";
    std::cout << "Access flags " << m.access_flags << "\n";
    std::cout << "This " << m.this_class << "\n";
    std::cout << "Super " << m.super_class << "\n";
    std::cout << "Interfaces " << m.interfaces_count << "\n";
    std::cout << "Fields " << m.fields_count << "\n";
    std::cout << "Methods " << m.methods_count << "\n";
    std::cout << "Attrib " << m.attributes_count << "\n";
    /*
    */
    vmStack *st = new vmStack();
    VM *vm = new VM(&m, st);
    // Init
    std::vector<vmClass*> preload;
    preload.push_back(new StringBuilder());
    preload.push_back(new PrintStream());
    preload.push_back(new ClassLangString());

    for (auto me : m.methods) {
        uint16_t i = 1;
        vmConstantUtf8 *mu = static_cast<vmConstantUtf8*>(m.constant_pool[me->name_index]);
        if (mu) {
            std::cout << "Method: " << mu->str() << "\n";
        }
        /*
        */
        for (auto a : me->attributes) {
            vmConstantUtf8 *u = static_cast<vmConstantUtf8*>(m.constant_pool[a->name_index]);
            if (u != nullptr) {
                std::cout << " Section: " << u->str() << "\n";
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
                    std::cout << vm->transcompile(mu->str(), code);
                } else {
                    //std::cout << i << " " << u->bytes << "\n";
                    for (uint32_t l = 0; l< a->length; ++l) {
                        std::cout << " " << std::hex << (unsigned int)a->info[l];
                    }
                    std::cout << "\n";
                }
            }
            ++i;
        }
    }
    (void)preload;

    return 0;
}
