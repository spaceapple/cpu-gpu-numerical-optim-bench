
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
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
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
            const FloatPrec lvl_resz_ratio);

    /*
    * record the template to be registered in images
    */
    Common::ErrCode
    set_template(
            const cimg_library::CImg<unsigned char> & i_ref_image,
            std::vector<FloatPrec> &                  i_annot_pts);

    /*
    * register the recorded template (set with 'set_template' in the input
    * image.
    */
    Common::ErrCode
    register_image(
            const cimg_library::CImg<unsigned char> & i_reg_image,
            std::vector<FloatPrec> &                  i_reginit_pts);

public: // public typedefs
    typedef Eigen::Matrix<FloatPrec, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
            MatrixNN;
    typedef Eigen::Matrix<FloatPrec, Eigen::Dynamic, 4, Eigen::RowMajor>
            MatrixN4;
    typedef Eigen::Matrix<FloatPrec, Eigen::Dynamic, 6, Eigen::RowMajor>
            MatrixN6;
    typedef Eigen::Matrix<FloatPrec, 1, Eigen::Dynamic, Eigen::RowMajor>
            RowVecN;
    typedef Eigen::Matrix<FloatPrec, 1, 6, Eigen::RowMajor>
            RowVec6;
private: // private typedefs
    typedef std::pair<uint32_t, uint32_t>                   ImDim;
    typedef std::vector<ImDim>                              LvlList_ImDim;
    typedef std::vector<RowVecN>                            LvlList_VecN;
    typedef std::vector<std::vector<FloatPrec> >            LvlList_StdN4;
    typedef std::vector<MatrixN4>                           LvlList_MatN4;
    typedef std::vector<cimg_library::CImg<unsigned char> > LvlList_Images;
private:
    uint32_t       m_nb_levels = 3;
    FloatPrec      m_lvl_resz_ratio = 0.5;
    ImDim          m_ref_imdim;
    LvlList_ImDim  m_lvl_imdims;
    LvlList_VecN   m_lvl_templates;
    LvlList_StdN4  m_lvl_Ws;
    LvlList_MatN4  m_lvl_Ws_eigen;
    LvlList_Images m_reg_im_pyr; // registration image resolution pyramid
private:
    // The following makes the copy contructor and the assignment operator
    // private to emulate a "non-copyable" class.
    DenseImageRegistrationSolver(DenseImageRegistrationSolver const &);
    DenseImageRegistrationSolver & operator = (DenseImageRegistrationSolver const &);
};

#include "dense_im_reg_cpu.inl.hpp"

#endif // #ifndef _DENSE_IM_REG_CPU_HPP



