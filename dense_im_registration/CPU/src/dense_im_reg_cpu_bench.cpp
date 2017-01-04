
/*
* Authors:
* Nicolas Stoiber (nicolas.stoi@gmail.com)
*
* 2016
*/

#include <iostream>
#include <cstdlib>
#include <stdint.h>
#include <string>

#include <CImg.h>

#include "dense_im_reg_cpu.hpp"

#define FLOATPREC float

static const int nb_expected_args = 4;

void
print_usage()
{
    std::cout << "=========================================================\n";
    std::cout << "Usage:\n";
    std::cout << "---------------------------------------------------------\n";
    std::cout << nb_expected_args << " arguments expected." << "\n";
    std::cout << "./bench_dense_im_reg_cpu "
        "1_ref_im_path "
        "2_ref_im_annot_info "
        "3_reg_im_path "
        "4_reg_im_init_info "
        << "\n";
    std::cout << "=========================================================\n";
}

int main(int argc, char ** argv)
{
    std::cout << "dense image registration benchmark (CPU) ..." << "\n" ;

    DenseImageRegistrationSolver<FLOATPREC> im_reg_solver;

    if ((argc-1) != nb_expected_args) {
        std::cerr << "Error: " << argc << " args instead of " << nb_expected_args << ".\n";
        print_usage();
        exit(-1);
    }

    // parse arguments
    uint32_t arg_ind = 1;
    std::string ref_im_path( argv[arg_ind] );
    arg_ind++;
    std::string ref_im_annot_info_path( argv[arg_ind] );
    arg_ind++;
    std::string reg_im_path( argv[arg_ind] );
    arg_ind++;
    std::string ref_im_init_info_path( argv[arg_ind] );
    arg_ind++;

    // load images
    cimg_library::CImg<unsigned char> ref_im_raw(ref_im_path.c_str());
    cimg_library::CImg<unsigned char> reg_im_raw(reg_im_path.c_str());
    cimg_library::CImg<unsigned char> ref_im_gray;
    cimg_library::CImg<unsigned char> reg_im_gray;

    // convert to grayscale
    switch(ref_im_raw.spectrum())
    {
        case 1: ref_im_gray = ref_im_raw; break;
        case 3: ref_im_gray = ref_im_raw.get_RGBtoYCbCr().get_channel(0); break;
    }
    switch(reg_im_raw.spectrum())
    {
        case 1: reg_im_gray = reg_im_raw; break;
        case 3: reg_im_gray = reg_im_raw.get_RGBtoYCbCr().get_channel(0); break;
    }

    im_reg_solver.init();

    cimg_library::CImgDisplay ref_im_cimgDisp(ref_im_gray,"reference image");
    cimg_library::CImgDisplay reg_im_cimgDisp(reg_im_gray,"registration image");

    while (!ref_im_cimgDisp.is_closed() || !reg_im_cimgDisp.is_closed()) {
        cimg_library::cimg::sleep(10);
    }

    std::cout << "over and out." << "\n" ;
    return 0;
}



