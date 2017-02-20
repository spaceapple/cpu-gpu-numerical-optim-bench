
/*
* Authors:
* Nicolas Stoiber (nicolas.stoi@gmail.com)
*
* 2016
*/

#ifndef _IMAGE_PYR_HPP
#define _IMAGE_PYR_HPP

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
    template <typename IntPrec>
    init(
            const ImDim<IntPrec> & i_input_im_sz,
            std::vector<FloatPrec> i_abs_rsz_ratios);

    init(
            const cimg_library::CImg<FloatPrec> & i_input_im,
            std::vector<FloatPrec> i_abs_rsz_ratios);

    ErrCode
    acq_image(
            const cimg_library::CImg<FloatPrec> & i_input_im);
public: // get-sets
    const cimg_library::CImg<FloatPrec> & operator [](uint32_t i_lvl) const {
        return m_lvl_images[i_lvl];
    }
    const cimg_library::CImg<FloatPrec> & get_grad(uint32_t i_lvl) const {
        return m_lvl_gradims[i_lvl];
    }
private: // members
    LvlList_Images         m_lvl_images;
    LvlList_Images         m_lvl_gradims;
    std::vector<FloatPrec> m_abs_sz_ratios;
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
ImagePyr<FloatPrec>::init(
        const ImDim<IntPrec> & i_input_im_sz,
        std::vector<FloatPrec> i_abs_rsz_ratios)
{
    m_abs_rsz_ratios = i_abs_rsz_ratios;
    const uint32_t nb_levels = i_abs_rsz_ratios.size();
    m_lvl_images.resize(nb_levels);
    m_lvl_gradims.resize(nb_levels);
    for (uint32_t i_lvl=0; i_lvl < nb_levels; ++i_lvl)
    {
        const uint32_t lvl_width = i_input_im_sz.width() * i_abs_rsz_ratios[i_lvl];
        const uint32_t lvl_height = i_input_im_sz.height() * i_abs_rsz_ratios[i_lvl];
        m_lvl_images.resize(lvl_width, lvl_height);
        m_lvl_gradims.resize(lvl_width, lvl_height);
    }
}

template <typename FloatPrec>
ImagePyr<FloatPrec>::init(
        const cimg_library::CImg<FloatPrec> & i_input_im,
        std::vector<FloatPrec> i_abs_rsz_ratios) :
{
    init(ImDim(i_input_im.width(), i_input_im.height()), i_abs_rsz_ratios);
    acq_image(i_input_im);
}

template <typename FloatPrec>
ErrCode
ImagePyr<FloatPrec>::~acq_image(
        const cimg_library::CImg<FloatPrec> & i_input_im)
{
    const uint32_t nb_levels = m_abs_rsz_ratios.size();
    for (uint32_t i_lvl=0; i_lvl < nb_levels; ++i_lvl)
    {
        m_lvl_images[i_lvl]
        m_lvl_gradims[i_lvl]
    }
    return NoError;
}




} // end namespace Common

#endif // #ifndef _IMAGE_PYR_HPP



