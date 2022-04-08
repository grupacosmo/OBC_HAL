find src lib -name *.cpp -o -name *.hpp | xargs clang-format -Werror --verbose --dry-run
sed -i 's/-fconcepts/ /g' build/compile_commands.json
run-clang-tidy -p build src lib -quiet