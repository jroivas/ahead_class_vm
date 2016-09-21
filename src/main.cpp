#include <iostream>
#include <classloader.h>

int main(int argc, char **argv)
{
    if (argc <= 1) {
        std::cout << "Usage: " << argv[0] << " classfile\n";
        return 1;
    }

    vmClassFile m(argv[1]);

    bool ok = m.parse();
    std::cout << "Parse? " << (ok ? "ok" :"fail") << "\n";
    std::cout << "Version " << m.major_version << " " << m.minor_version << "\n";
    std::cout << "Access flags " << m.access_flags << "\n";
    std::cout << "This " << m.this_class << "\n";
    std::cout << "Super " << m.super_class << "\n";
    std::cout << "Interfaces " << m.interfaces_count << "\n";
    std::cout << "Fields " << m.fields_count << "\n";
    std::cout << "Methods " << m.methods_count << "\n";
    std::cout << "Attrib " << m.attributes_count << "\n";
    for (auto me : m.methods) {
        uint16_t i = 1;
        vmConstantUtf8 *mu = dynamic_cast<vmConstantUtf8*>(m.constant_pool[me->name_index]);
        if (mu) {
            std::cout << "Method: " << mu->bytes << "\n";
        }
        for (auto a : me->attributes) {
            vmConstantUtf8 *u = dynamic_cast<vmConstantUtf8*>(m.constant_pool[a->name_index]);
            if (u != nullptr) {
                std::cout << "Section: " << u->bytes << "\n";
                //std::cout << i << " " << u->bytes << "\n";
                for (uint32_t l = 0; l< a->length; ++l) {
                    std::cout << " " << (unsigned int)a->info[l] << " ";
                }
                std::cout << "\n";
            }
            ++i;
        }
    }

    return 0;
}
