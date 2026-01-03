cmake -S . -B debug_build -DCMAKE_BUILD_TYPE=Debug
cmake --build debug_build
ln -s debug_build/compile_commands.json .
