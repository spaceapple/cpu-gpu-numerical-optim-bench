
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

#define EIGEN_DEFAULT_TO_ROW_MAJOR // comment out this line to use ColMajor
// ****************************************************************************

#undef Success //(X11 and Eigen both define the Success macro)

// match our own structure orders with Eigen internal's
#ifdef EIGEN_DEFAULT_TO_ROW_MAJOR
    #define MY_STORAGE_ORDER Eigen::RowMajor
#else
    #define MY_STORAGE_ORDER Eigen::ColMajor
#endif

#include <Eigen/Dense>

#include <vector>

#include <CImg.h>

#include "errCodes.h"

#include "im_processing_utils.hpp"
#include "optimization_utils.hpp"


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
    * i_normz_factor is a normalization factor to map image values to a favorable
    * floating point range (typically [0., 1.]
    */
    Common::ErrCode
    set_template(
            const cimg_library::CImg<unsigned char> & i_ref_image,
            std::vector<FloatPrec> &                  i_annot_pts,
            FloatPrec                                 i_normz_factor = 1./255.);

    /*
    * register the recorded template (set with 'set_template' in the input
    * image.
    */
    Common::ErrCode
    register_image(
            const cimg_library::CImg<unsigned char> & i_reg_image,
            uint32_t                                  i_nb_iterations,
            std::vector<FloatPrec> &                  io_reg_pts);

    /*
    * get the level templates as images (mainly for debug purposes)
    */
    Common::ErrCode
    get_template_image(
            std::vector<cimg_library::CImg<unsigned char> > & o_lvl_templims) const;

    /*
    * get nb levels
    */
    inline uint32_t nb_levels() const {return m_nb_levels;}

public: // public typedefs
    typedef Eigen::Matrix<FloatPrec, Eigen::Dynamic, Eigen::Dynamic, MY_STORAGE_ORDER>
            MatrixNN;
    typedef Eigen::Matrix<FloatPrec, Eigen::Dynamic, 2, MY_STORAGE_ORDER>
            MatrixN2;
    typedef Eigen::Matrix<FloatPrec, Eigen::Dynamic, 4, MY_STORAGE_ORDER>
            MatrixN4;
    typedef Eigen::Matrix<FloatPrec, Eigen::Dynamic, 6, MY_STORAGE_ORDER>
            MatrixN6;
    typedef Eigen::Matrix<FloatPrec, 1, Eigen::Dynamic, MY_STORAGE_ORDER>
            VecN;
    typedef Eigen::Matrix<FloatPrec, 1, 6, MY_STORAGE_ORDER>
            Vec6;
    typedef Eigen::Matrix<FloatPrec, 4, 2, MY_STORAGE_ORDER>
            Matrix42;
private: // private typedefs
    typedef Common::ImDim<uint32_t>                         ImDim;
    typedef std::vector<ImDim>                              LvlList_ImDim;
    typedef std::vector<uint32_t>                           LvlList_Ind;
    typedef std::vector<uint32_t>                           LvlList_Size;
    typedef std::vector<FloatPrec>                          LvlList_Ratio;
    typedef std::vector<VecN>                               LvlList_VecN;
    typedef std::vector<std::vector<FloatPrec> >            LvlList_StdN4;
    typedef std::vector<MatrixN4>                           LvlList_MatN4;
    typedef std::vector<MatrixN2>                           LvlList_MatN2;
    typedef std::vector<cimg_library::CImg<unsigned char> > LvlList_Images;
    typedef std::vector<MatrixNN>                           LvlList_MatNN;
// public:
private:
    bool             m_is_init = false;
    bool             m_template_is_set = false;
    uint32_t         m_nb_levels = 3;
    FloatPrec        m_lvl_resz_ratio = 0.5;
    FloatPrec        m_normz_factor = 1./255.;
    ImDim            m_ref_imdim;
    LvlList_Ratio    m_lvl_abs_resz_ratio;
    LvlList_ImDim    m_lvl_templdims;
    LvlList_VecN     m_lvl_templates;
    LvlList_StdN4    m_lvl_Ws;
    LvlList_MatN4    m_lvl_Ws_eigen;
    LvlList_MatN2    m_lvl_gridpts_eigen;
    VecN             m_mr_template;
    LvlList_Ind      m_lvl_err_start_inds;
    LvlList_Size     m_lvl_err_size;
    // solver containers
    VecN             m_delta_vars;
    LvlList_VecN     m_lvl_errs;
    LvlList_MatNN    m_lvl_jacos;
    LvlList_MatNN    m_lvl_jTj;
    LvlList_VecN     m_lvl_jTb;
    VecN             m_mr_errs; // mr: multi resolution
    MatrixNN         m_mr_jaco;
    MatrixNN         m_mr_jTj;
    VecN             m_mr_jTb;
    VecN             m_curr_pts;
    LvlList_Matrix42 m_lvl_annot_pts;
    Common::ImagePyr m_curr_img_pyr;
private:
    // The following makes the copy contructor and the assignment operator
    // private to emulate a "non-copyable" class.
    DenseImageRegistrationSolver(DenseImageRegistrationSolver const &);
    DenseImageRegistrationSolver & operator = (DenseImageRegistrationSolver const &);
private:
    /*
    * compute multi-resolution pixel error vector for a given configuration of
    * points
    */
    void
    compute_multires_pix_error(
            const Matrix42 & i_pts,
            VecN &           o_mr_pix_err);

    /*
    * compute pixel error vector for a given configuration of
    * points for a given resoltion level
    */
    template <typename OVecNType>
    void
    compute_lvl_pix_error(
            const VecN & i_pts,
            uint32_t     i_lvl,
            OVecNType      & o_lvl_pix_err);

    /*
    * compute multi-resolution pixel jacobian matrix for a given configuration of
    * points
    */
    void
    compute_multires_pix_jacobian(
            const VecN & i_pts,
            MatN &       o_mr_pix_err);

    /*
    * compute pixel jacobian matrix for a given configuration of
    * points for a given resoltion level
    */
    template <typename OMatNN>
    void
    compute_lvl_pix_jacobian(
            const VecN & i_pts,
            uint32_t     i_lvl,
            OMatNNType & o_lvl_pix_jaco);
};

#include "dense_im_reg_cpu.inl.hpp"

#endif // #ifndef _DENSE_IM_REG_CPU_HPP



