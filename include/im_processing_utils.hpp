
/*
* Authors:
* Nicolas Stoiber (nicolas.stoi@gmail.com)
*
* 2016
*/

#ifndef _IM_PROCESSING_UTILS_HPP
#define _IM_PROCESSING_UTILS_HPP

#include <CImg.h>

#include "errCodes.h"

#define MAX_TEMPLATE_DIMENSION 4000

namespace Common
{


/*
* Simple struct to represent image sizes
*/
template <typename IntPrec>
struct ImDim
{
public:
    ImDim() {}
    ImDim(IntPrec i_width, IntPrec i_height) : 
        m_width(i_width),
        m_height(i_height)
    {}
public:
    void
    set_dim(IntPrec i_width, IntPrec i_height) {
        m_width = i_width;
        m_height = i_height;
    }
public:
    inline IntPrec width() const { return m_width; }
    inline IntPrec height() const { return m_height; }
private:
    IntPrec m_width = 0;
    IntPrec m_height = 0;
};


/*
* Generate coeeficient to warp pixel to a quad grid (the template)
* What is generated here are the coefficients of a Nx4 matrix W (N: number of
* pixels in template), so that when the 4 vertices (Xvi, Yvi) of a quads are
* known in the current image, the warped coordinates of a given pixel of the
* template are (W * [Xv1, Xv2, Xv3, Xv4].T, W * [Yv1, Yv2, Yv3, Yv4].T)
* W is stored as a vector in raw-major fashion.
*/
template <typename FloatPrec>
ErrCode
generate_quad_warping_coeffs(
        uint32_t                 template_width,
        uint32_t                 template_height,
        std::vector<FloatPrec> & o_W)
{
    const int32_t N = template_width * template_height;
    o_W.resize(N * 4);

    if ((template_width < 2) && (template_width > MAX_TEMPLATE_DIMENSION)) {
        return InvalidTemplateDimension;
    }
    if ((template_height < 2) && (template_height > MAX_TEMPLATE_DIMENSION)) {
        return InvalidTemplateDimension;
    }

    // pixels in the template have coordinates X = [0 ... template_width-1],
    // Y = [0 ... template_height-1]
    const FloatPrec inv_x_sz = 1. / (FloatPrec)(template_width-1);
    const FloatPrec inv_y_sz = 1. / (FloatPrec)(template_height-1);
    const FloatPrec xA = 0.;
    const FloatPrec yA = 0.;
    const FloatPrec xB = (FloatPrec)(template_width - 1);
    // const FloatPrec yB = 0.;
    // const FloatPrec xC = (FloatPrec)(template_width - 1);
    // const FloatPrec yC = (FloatPrec)(template_height - 1);
    // const FloatPrec xD = 0.;
    const FloatPrec yD = (FloatPrec)(template_height - 1);
    for(uint32_t i_y = 0; i_y<template_height; ++i_y) {
        const FloatPrec yP = (FloatPrec)(i_y);
        const FloatPrec yD_yP = yD - yP;
        const FloatPrec yP_yA = yP - yA;

        for(uint32_t i_x = 0; i_x<template_width; ++i_x) {
            const uint32_t pixel_ind = i_y*template_width + i_x;
            const uint32_t w_write_index = 4 * pixel_ind;

            const FloatPrec xP = (FloatPrec)(i_x);
            const FloatPrec xB_xP = xB - xP;
            const FloatPrec xP_xA = xP - xA;

            o_W[w_write_index + 0] = (yD_yP*inv_y_sz) * (xB_xP*inv_x_sz);
            o_W[w_write_index + 1] = (yD_yP*inv_y_sz) * (xP_xA*inv_x_sz);
            o_W[w_write_index + 2] = (yP_yA*inv_y_sz) * (xP_xA*inv_x_sz);
            o_W[w_write_index + 3] = (yP_yA*inv_y_sz) * (xB_xP*inv_x_sz);
        }
    }

    return NoError;
}

/*
* perform quad warping based on precomputed interpolation coefficient from
* function 'generate_quad_warping_coeffs'
* WARNING: for performance reasons, o_warped_pixCoords must be already allocated
* with the right size (N*2)
* i_pts holds the 2D coordinates of the quad's vertices A,B,C,D, and so has size
* 8.
*/
template <typename FloatPrec>
ErrCode
apply_quad_warping(
        const std::vector<FloatPrec> & i_W,
        const std::vector<FloatPrec> & i_pts,
        std::vector<FloatPrec> &       o_warped_pixCoords)
{
    const uint32_t N = o_warped_pixCoords.size() / 4;

    const FloatPrec xA = i_pts[0*2 + 0];
    const FloatPrec yA = i_pts[0*2 + 1];
    const FloatPrec xB = i_pts[1*2 + 0];
    const FloatPrec yB = i_pts[1*2 + 1];
    const FloatPrec xC = i_pts[2*2 + 0];
    const FloatPrec yC = i_pts[2*2 + 1];
    const FloatPrec xD = i_pts[3*2 + 0];
    const FloatPrec yD = i_pts[3*2 + 1];

    for (uint32_t i_pix=0; i_pix<N; ++i_pix) {
        const FloatPrec wA = i_W[i_pix*4 + 0];
        const FloatPrec wB = i_W[i_pix*4 + 1];
        const FloatPrec wC = i_W[i_pix*4 + 2];
        const FloatPrec wD = i_W[i_pix*4 + 3];
        o_warped_pixCoords[i_pix*2 + 0] = wA*xA + wB*xB + wC*xC + wD*xD;
        o_warped_pixCoords[i_pix*2 + 1] = wA*yA + wB*yB + wC*yC + wD*yD;
    }

    return NoError;
}

template <typename FloatPrec>
FloatPrec
bilinear_pix_interp(
        const cimg_library::CImg<FloatPrec> & image,
        FloatPrec                             x,
        FloatPrec                             y)
{
    const uint32_t x0 = (uint32_t) std::floor(x);
    const uint32_t x1 = x0 + 1;
    const uint32_t y0 = (uint32_t) std::floor(y);
    const uint32_t y1 = y0 + 1;

    const FloatPrec a = image(x0, y0);
    const FloatPrec b = image(x1, y0);
    const FloatPrec c = image(x0, y1);
    const FloatPrec d = image(x1, y1);

    const FloatPrec wa = (x1-x)*(y1-y);
    const FloatPrec wb = (x-x0)*(y1-y);
    const FloatPrec wc = (x1-x)*(y-y0);
    const FloatPrec wd = (x-x0)*(y-y0);

    return a*wa + b*wb + c*wc + d*wd;
}


} // end namespace Common

#endif /* _IM_PROCESSING_UTILS_HPP *  * */




