
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
#include <fstream>

#include <CImg.h>

#include "errCodes.h"
#include "annot_info_handling.hpp"

#include "dense_im_reg_cpu.hpp"

#define FLOATPREC float

#define BEVERBOSE true

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

    // parse input arguments
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
    cimg_library::CImg<unsigned char> ref_im_asis(ref_im_path.c_str());
    cimg_library::CImg<unsigned char> reg_im_asis(reg_im_path.c_str());
    cimg_library::CImg<unsigned char> ref_im_gray;
    cimg_library::CImg<unsigned char> reg_im_gray;
    cimg_library::CImg<unsigned char> ref_im_colorDisp;
    cimg_library::CImg<unsigned char> reg_im_colorDisp;
    // convert to grayscale if not grayscale already
    switch(ref_im_asis.spectrum())
    {
        case 1:
            ref_im_gray = ref_im_asis;
            ref_im_colorDisp.assign(
                    ref_im_gray.width(), ref_im_gray.height(), 1, 3);
            std::cerr << "debug: "<< ref_im_colorDisp.spectrum() << "\n";
            ref_im_colorDisp.draw_image(0, 0, 0, 0, ref_im_gray);
            ref_im_colorDisp.draw_image(0, 0, 0, 1, ref_im_gray);
            ref_im_colorDisp.draw_image(0, 0, 0, 2, ref_im_gray);
            std::cerr << "debug: "<< ref_im_colorDisp.spectrum() << "\n";
            // ref_im_colorDisp.YCbCrtoRGB();
            break;
        case 3:
            ref_im_colorDisp = ref_im_asis;
            ref_im_gray = ref_im_asis.get_RGBtoYCbCr().get_channel(0);
            break;
    }
    switch(reg_im_asis.spectrum())
    {
        case 1:
            reg_im_gray = reg_im_asis;
            reg_im_colorDisp.assign(
                    reg_im_gray.width(), reg_im_gray.height(), 1, 3);
            reg_im_colorDisp.draw_image(0, 0, 0, 0, reg_im_gray);
            reg_im_colorDisp.draw_image(0, 0, 0, 1, reg_im_gray);
            reg_im_colorDisp.draw_image(0, 0, 0, 2, reg_im_gray);
            // reg_im_colorDisp.YCbCrtoRGB();
            break;
        case 3:
            reg_im_colorDisp = reg_im_asis;
            reg_im_gray = reg_im_asis.get_RGBtoYCbCr().get_channel(0);
            break;
    }

    // parse ref annotation info
    Common::ErrCode curr_errCode = Common::NoError;
    std::vector<FLOATPREC> annot_pts;
    curr_errCode = Common::parse_annot_info(
            ref_im_annot_info_path,
            annot_pts);
    if (curr_errCode != Common::NoError) {
        std::cerr << "Error parsing ref im annotation info (error " <<
            curr_errCode << ")." << ".\n";
        exit(-1);
    }
    if (BEVERBOSE) {
        // Debug: printout parsed ref annot coordinates
        std::cout << "ref annot info: ";
        for (int i =0; i<8; ++i)
            std::cout << annot_pts[i] << ", ";
        std::cout << "\n";
    }

    // parse reg annotation info
    std::vector<FLOATPREC> reginit_pts;
    curr_errCode = Common::parse_annot_info(
            ref_im_init_info_path,
            reginit_pts);
    if (curr_errCode != Common::NoError) {
        std::cerr << "Error parsing reg im init info (error " <<
            curr_errCode << ")." << ".\n";
        exit(-1);
    }
    if (BEVERBOSE) {
        // Debug: printout parsed ref annot coordinates
        std::cout << "ref annot info: ";
        for (int i =0; i<8; ++i)
            std::cout << reginit_pts[i] << ", ";
        std::cout << "\n";
    }




    im_reg_solver.init();






    std::cerr << "debug: "<< ref_im_colorDisp.spectrum() << "\n";

    // overlay annotation duads on display images
    const uint8_t ref_dot_draw_color[] = { 20, 255, 20 };
    const uint8_t reg_dot_draw_color[] = { 30, 30, 255 };
    for ( uint32_t pts_ind = 0; pts_ind< 4; ++pts_ind) {
        ref_im_colorDisp.draw_point(
                annot_pts[2*pts_ind + 0], annot_pts[2*pts_ind + 1],
                ref_dot_draw_color);
        reg_im_colorDisp.draw_point(
                reginit_pts[2*pts_ind + 0], reginit_pts[2*pts_ind + 1],
                reg_dot_draw_color);
        uint32_t orig_ind = pts_ind;
        uint32_t dest_ind = (pts_ind+1) % 4;
        ref_im_colorDisp.draw_line(
                annot_pts[2*orig_ind + 0], annot_pts[2*orig_ind + 1],
                annot_pts[2*dest_ind + 0], annot_pts[2*dest_ind + 1],
                ref_dot_draw_color);
        reg_im_colorDisp.draw_line(
                reginit_pts[2*orig_ind + 0], reginit_pts[2*orig_ind + 1],
                reginit_pts[2*dest_ind + 0], reginit_pts[2*dest_ind + 1],
                reg_dot_draw_color);
    }

    cimg_library::CImgDisplay ref_im_cimgDisp(ref_im_colorDisp,"reference image");
    cimg_library::CImgDisplay reg_im_cimgDisp(reg_im_colorDisp,"registration image");

    while (!ref_im_cimgDisp.is_closed() || !reg_im_cimgDisp.is_closed()) {
        cimg_library::cimg::sleep(10);
    }

    std::cout << "over and out." << "\n" ;
    return 0;
}



