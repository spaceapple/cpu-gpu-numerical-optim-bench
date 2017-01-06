
/*
* Authors:
* Nicolas Stoiber (nicolas.stoi@gmail.com)
*
* 2016
*/

#ifndef _DENSE_IM_REG_CPU_HPP
#define _DENSE_IM_REG_CPU_HPP

// ************************** EIGEN SPECIFIC DEFS *****************************
// setting the flag below could help see if there are hidden allocations.
//#define EIGEN_NO_AUTOMATIC_RESIZING
//#define EIGEN_RUNTIME_NO_MALLOC
#ifdef EIGEN_RUNTIME_NO_MALLOC
#define malloc_allowed(v) Eigen::internal::set_is_malloc_allowed(v)
#else
#define malloc_allowed(v)
#endif

#define EIGEN_DEFAULT_TO_ROW_MAJOR
// ****************************************************************************

#undef Success //(X11 and Eigen both define the Success macro)
#include <Eigen/Dense>

#include <vector>

#include <CImg.h>

#include "errCodes.h"


template <typename FloatPrec>
struct DenseImageRegistrationSolver
{
public:
    DenseImageRegistrationSolver() {}
    ~DenseImageRegistrationSolver() {}
public:
    /*
    * initialize registration solver
    * This preallocates most internal variables used in registration
    */
    Common::ErrCode
    init(
            const uint32_t  template_width,
            const uint32_t  template_height,
            const uint32_t  nb_levels,
            const FloatPrec lvl_resz_ratio)
    {

        // compute resolution pyramid templates resolutions
        m_nb_levels = nb_levels;
        m_lvl_resz_ratio = lvl_resz_ratio;
        m_lvl_imdims.resize(nb_levels);

        float this_lvl_ratio = 1.0;
        for (uint32_t i_lvl = 0; i_lvl<nb_levels; ++i_lvl) {
            // warning: silent floor() due to float->int casting
            const uint32_t lvl_template_width = template_width * this_lvl_ratio;
            const uint32_t lvl_template_height = template_width * this_lvl_ratio;
            m_lvl_imdims[i_lvl] = std::make_pair(
                    lvl_template_width, lvl_template_height);

            this_lvl_ratio *= lvl_resz_ratio;
        }

        return Common::NoError;
    }

    /*
    * record the template to be registered in images
    */
    Common::ErrCode
    set_template(
            const cimg_library::CImg<unsigned char> & i_ref_image,
            std::vector<FloatPrec> &                  i_annot_pts)
    {
        m_ref_imdim = std::make_pair(i_ref_image.width(), i_ref_image.height());

        return Common::NoError;
    }

    /*
    * register the recorded template (set with 'set_template' in the input
    * image.
    */
    Common::ErrCode
    register_image(
            const cimg_library::CImg<unsigned char> & i_reg_image,
            std::vector<FloatPrec> &                  i_reginit_pts)
    {
        return Common::NoError;
    }
private: // typedefs
    typedef Eigen::Matrix<FloatPrec, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
            MatrixNN;
    typedef Eigen::Matrix<FloatPrec, Eigen::Dynamic, 6, Eigen::RowMajor>
            MatrixN6;
    typedef Eigen::Matrix<FloatPrec, 1, Eigen::Dynamic, Eigen::RowMajor>
            RowVecN;
    typedef Eigen::Matrix<FloatPrec, 1, 6, Eigen::RowMajor>
            RowVec6;
    typedef std::pair<uint32_t, uint32_t>                   ImDim;
    typedef std::vector<ImDim>                              LvlList_ImDim;
    typedef std::vector<RowVecN>                            LvlList_VecN;
    typedef std::vector<cimg_library::CImg<unsigned char> > LvlList_Images;
private:
    uint32_t       m_nb_levels = 3;
    FloatPrec      m_lvl_resz_ratio = 0.5;
    ImDim          m_ref_imdim;
    LvlList_ImDim  m_lvl_imdims;
    LvlList_VecN   m_lvl_templates;
    LvlList_Images m_reg_im_pyr; // registration image resolution pyramid
private:
    // The following makes the copy contructor and the assignment operator
    // private to emulate a "non-copyable" class.
    DenseImageRegistrationSolver(DenseImageRegistrationSolver const &);
    DenseImageRegistrationSolver & operator = (DenseImageRegistrationSolver const &);
};


#endif // #ifndef _DENSE_IM_REG_CPU_HPP



