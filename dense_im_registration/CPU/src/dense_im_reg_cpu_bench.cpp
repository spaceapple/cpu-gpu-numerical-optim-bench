
/*
* Authors:
* Nicolas Stoiber (nicolas.stoi@gmail.com)
*
* 2016
*/

#include <iostream>
#include <cstdlib>

#include <CImg.h>

#include "dense_im_reg_cpu.hpp"

#define FLOATPREC float

int main(int argc, char ** argv)
{
    std::cout << "dense image registration benchmark (CPU) ..." << "\n" ;

    DenseImageRegistrationSolver<FLOATPREC> im_reg_solver;

    im_reg_solver.init();

    std::cout << "over and out." << "\n" ;
    return 0;
}



