
/*
* Authors:
* Nicolas Stoiber (nicolas.stoi@gmail.com)
*
* 2016
*/

#ifndef _OPTIMIZATION_UTILS_HPP
#define _OPTIMIZATION_UTILS_HPP

#include <Eigen/Dense>
#include "errCodes.h"


namespace Common
{

// Gauss newton optimization step via Choleski LDLT factorization.
// Function for dense matrices.
template <typename ErrVecN,
          typename JacoMatNN,
          typename JTJMatNN,
          typename JTbVecN,
          typename OVecN>
ErrCode
gauss_newton_descent_step(
            ErrVecN &                                 i_err_vec,
            JacoMatNN &                               i_jaco_mat,
            JTJMatNN &                                i_JTJ_mat,
            JTbVecN &                                 i_JTb_vec,
            typename Eigen::MatrixBase<OVecN> &       o_delta_param)
{
    typedef Eigen::Matrix<typename OVecN::Scalar, Eigen::Dynamic, 1> ColVecN;

    i_JTJ_mat.noalias() = i_jaco_mat.transpose() * i_jaco_mat;

    Eigen::Map<const ColVecN> err_asColVec( &(i_err_vec(0)), i_err_vec.size() );
    i_JTb_vec.noalias() = i_jaco_mat.transpose() * err_asColVec;

    // solve:
    Eigen::Map<const ColVecN> JTb_asColVec( &(i_JTJ_mat(0)), i_JTb_vec.size() );
    o_delta_param = - i_JTJ_mat.ldlt().solve( JTb_asColVec );

    return NoError;
}


} // end namespace Common

#endif /* _OPTIMIZATION_UTILS_HPP *  * */



