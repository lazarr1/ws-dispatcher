cmake -S . -B release_build -DCMAKE_BUILD_TYPE=Release
cmake --build release_build
ln -s release_build/compile_commands.json .
