cmake --build build --target clean
cmake -B build --preset vcpkg -GNinja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
