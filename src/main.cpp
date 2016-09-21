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

    return 0;
}
