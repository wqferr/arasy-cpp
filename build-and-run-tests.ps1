New-Item -ItemType "Directory" build
Set-Location build
cmake .. --preset vcpkg
cmake --build . --target unit_tests
