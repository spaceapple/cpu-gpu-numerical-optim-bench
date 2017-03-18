
/*
* Authors:
* Nicolas Stoiber (nicolas.stoi@gmail.com)
*
* 2016
*/

#ifndef _IMAGE_PYR_HPP
#define _IMAGE_PYR_HPP

#include <vector>
#include <CImg.h>

#include "errCodes.h"

#include "im_processing_utils.hpp"

namespace Common
{



template <typename FloatPrec>
struct ImagePyr
{
public:
    typedef std::vector<cimg_library::CImg<FloatPrec> > LvlList_Images;
public: // con(de)structors
    ImagePyr();
    ~ImagePyr();
public: // methods
    /*
    * initialize ImagePyr object's data structure by explicitely providing the
    * dimension of the original image and the absolute resizing ratios.
    */
    template <typename IntPrec>
    ErrCode
    init(
            const ImDim<IntPrec> &   i_input_im_sz,
            std::vector<FloatPrec> & i_abs_rsz_ratios);

    /*
    * initialize ImagePyr object's data structure by directly providing the
    * image we wish to acquire.
    */
    ErrCode
    init(
            const cimg_library::CImg<FloatPrec> & i_input_im,
            std::vector<FloatPrec> &              i_abs_rsz_ratios);

    /*
    * fill image pyramid data with this image.
    */
    ErrCode
    acq_image(
            const cimg_library::CImg<FloatPrec> & i_input_im);
public: // get-sets
    /*
    * get i-th level image
    */
    const cimg_library::CImg<FloatPrec> & operator [](uint32_t i_lvl) const {
        return m_lvl_images[i_lvl];
    }
    /*
    * get i-th level gradients
    */
    const cimg_library::CImg<FloatPrec> & get_x_grad(uint32_t i_lvl) const {
        return m_lvl_xgrad_ims[i_lvl];
    }
    const cimg_library::CImg<FloatPrec> & get_y_grad(uint32_t i_lvl) const {
        return m_lvl_ygrad_ims[i_lvl];
    }
    /*
    * get nb of levels
    */
    const uint32_t & get_nb_levels() const {
        return m_abs_rsz_ratios.size();
    }
private: // members
    LvlList_Images         m_lvl_images;
    LvlList_Images         m_lvl_xgrad_ims;
    LvlList_Images         m_lvl_ygrad_ims;
    std::vector<FloatPrec> m_abs_rsz_ratios;
};


template <typename FloatPrec>
ImagePyr<FloatPrec>::ImagePyr()
{
}

template <typename FloatPrec>
ImagePyr<FloatPrec>::~ImagePyr()
{
}

template <typename FloatPrec>
template <typename IntPrec>
ErrCode
ImagePyr<FloatPrec>::init(
        const ImDim<IntPrec> &   i_input_im_sz,
        std::vector<FloatPrec> & i_abs_rsz_ratios)
{
    m_abs_rsz_ratios = i_abs_rsz_ratios;
    const uint32_t nb_levels = i_abs_rsz_ratios.size();
    m_lvl_images.resize(nb_levels);
    m_lvl_xgrad_ims.resize(nb_levels);
    m_lvl_ygrad_ims.resize(nb_levels);
    for (uint32_t i_lvl=0; i_lvl < nb_levels; ++i_lvl)
    {
        const uint32_t lvl_width = i_input_im_sz.width() * i_abs_rsz_ratios[i_lvl];
        const uint32_t lvl_height = i_input_im_sz.height() * i_abs_rsz_ratios[i_lvl];
        m_lvl_images.resize(lvl_width, lvl_height);
        m_lvl_xgrad_ims.resize(lvl_width, lvl_height);
        m_lvl_ygrad_ims.resize(lvl_width, lvl_height);
    }
    return NoError;
}

template <typename FloatPrec>
ErrCode
ImagePyr<FloatPrec>::init(
        const cimg_library::CImg<FloatPrec> & i_input_im,
        std::vector<FloatPrec> &              i_abs_rsz_ratios)
{
    ErrCode err_code = NoError;
    err_code = init(
            ImDim<uint32_t>(i_input_im.width(), i_input_im.height()),
            i_abs_rsz_ratios);
    if (err_code != NoError) {return err_code;}
    err_code = acq_image(i_input_im);
    return err_code;
}

template <typename FloatPrec>
ErrCode
ImagePyr<FloatPrec>::acq_image(
        const cimg_library::CImg<FloatPrec> & i_input_im)
{
    const uint32_t nb_levels = m_abs_rsz_ratios.size();
    for (uint32_t i_lvl=0; i_lvl < nb_levels; ++i_lvl)
    {
        const uint32_t lvl_width =
            i_input_im.width() * m_abs_rsz_ratios[i_lvl];
        const uint32_t lvl_height =
            i_input_im.height() * m_abs_rsz_ratios[i_lvl];
        m_lvl_images[i_lvl] = i_input_im.get_resize(lvl_width, lvl_height);
        const cimg_library::CImgList<FloatPrec> grad_tmp =
                m_lvl_images[i_lvl].get_gradient("xy", 2);
        m_lvl_xgrad_ims[i_lvl] = grad_tmp(0);
        m_lvl_ygrad_ims[i_lvl] = grad_tmp(1);

        // DEBUG KILLME:
        std::string lvl_save_im_name = std::string("impyr_lvl") + std::string(".png");
        m_lvl_images[i_lvl].save(lvl_save_im_name.c_str());
    }
    return NoError;
}




} // end namespace Common

#endif // #ifndef _IMAGE_PYR_HPP



