# octray
### (Octree ray casting)

## Build instructions
clone this repo with recursive submodules: `git clone https://github.com/connortynan/octray --recurse-submodules`

follow instructions in `libs/glad/README.md` to generate glad source files

build and run:
```
mkdir build && cd build 
cmake ../ 
make
bin/octree
```


## TODO:
 - Experiment with GPGPU to parallelize for multiple rays
 - Refactor all visualization code to make main more readable
