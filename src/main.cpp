#include <iostream>
#include <classloader.h>

int main(int argc, char **argv)
{
    if (argc <= 1) {
        std::cout << "Usage: " << argv[0] << " classfile\n";
        return 1;
    }
    vmClassFile m(argv[1]);
    return 0;
}
