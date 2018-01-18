# nuklear_wasm_bug_test
Test for nuklear gui runtime error which happens when compiling to wasm with optimizations enabled

### install & run with
```sh
git checkout --recursive https://github.com/Deins/nuklear_wasm_bug_test.git
emcc main.cpp -o optimized.html -s WASM=1 -std=c++14 -O3
emrun optimized.html
```
It should crash with runtime error. When removing -O3 it runs as expected.
