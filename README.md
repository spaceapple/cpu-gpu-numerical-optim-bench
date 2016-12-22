# CPU GPU Numerical Optimization Benchmark

Benchmark code to compare popular CPU and GPU computing libraries on numerical optimization problems.

## Dependencies

- CMake (version 3.0 and above). https://cmake.org
- CImg (CeCILL-C License, LGPL-like). http://cimg.eu
- Eigen 3 (MPL2 License). http://eigen.tuxfamily.org
- ArrayFire (BSD 3-Clause License). http://arrayfire.com

## Build Instructions

Inside your forked repository of cpu-gpu-numerical-optim-bench:

    mkdir build
    cd build
    cmake .. -D...
    make

By default, dependencies will be looked up in the *libs* folder at the root of
the repository. The build system will look for:

- *repo_root*/libs/eigen
- *repo_root*/libs/CImg
- *repo_root*/libs/ArrayFire

Optionally, you can modify the path where the libraries are be looked for.
Simply add one or more of the following command line arguments to cmake:

    -DEIGEN3_INCLUDE_DIR="path to my eigen3 lib folder"
    -DEIGEN3_INCLUDE_DIR="path to my Cimg lib folder"
    -DARRAYFIRE_INCLUDE_DIR="path to my ArrayFire lib folder"

or edit the corresponding field in cmake's GUI tool.

## Data

### Dense Image Registration Benchmark

The images originate from the dataset created for the DAISY paper:

**DAISY: An Efficient Dense Descriptor Applied to Wide Baseline Stereo**
Engin Tola, Vincent Lepetit, Pascal Fua
IEEE Transactions on Pattern Analysis and Machine Intelligence
Vol. 32, Nr. 5, pp. 815 - 830, May 2010
URL: http://cvlab.epfl.ch/software/daisy



