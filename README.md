# Ahead Class VM

Target is to create compiler, which translates JVM (Java Virtual Machine) runnable class files to C++ code.

Traditional Java VM is either intrepreted or JIT (Just-in-time) compiled.
Aim of this tool is to translate generate class file to C++ code which can be compiled
to native binary ahead of time.

These C++ files are then compiled to native code with any compatible C++ compiler.
This way we hopefully can get native, and faster applications.
While doing the translation, we may apply some optimizations on generated code.

Provides library for implementing basic structures and system classes,
as well as translator itself.


## Building

First install dependencies (for example on Ubuntu/Debian):

    sudo apt-get install libgc-dev libjemalloc-dev

You need [meson](http://mesonbuild.com/) and [ninja](https://ninja-build.org/).

    mkdir build
    cd build
    meson ..
    ninja


## Translating applications

After that you should have `src/ahead_class_vm` binary, which is the translator.

Next you need some class files. There's included one in `examples` folder.
Compile it with java compliler:

    cd ../examples
    javac hello.java
    cd ../build

    # Rebuilds translator and generates C++ code, and compiles it with g++
    ../tools/translate.sh ../examples/hello.class

    # Run translated application
    ./hello


## State

Ahead Class VM is still experimental, and currently implements only basic functionality.
Missing many opcodes, support for multiple class files, etc.


## LICENSE

MIT, see [LICENSE](LICENSE).
