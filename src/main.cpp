#include <iostream>
#include <classloader.h>

int main(int argc, char **argv)
{
    if (argc <= 1) {
        std::cout << "Usage: " << argv[0] << " classfile\n";
        return 1;
    }

    vmClassFile m(argv[1]);

    std::cout << "Parse? " << (m.parse()? "ok" :"fail") << "\n";
    std::cout << "Version " << m.major_version << " " << m.minor_version << "\n";

    return 0;
}
