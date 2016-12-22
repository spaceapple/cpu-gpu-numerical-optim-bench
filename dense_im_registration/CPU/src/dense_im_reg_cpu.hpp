
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

template <typename FloatPrec>
struct DenseImageRegistrationSolver
{
public:
    DenseImageRegistrationSolver() {}
    ~DenseImageRegistrationSolver() {}
public:
    void
    init()
    {
    }
private:
    // The following makes the copy contructor and the assignment operator
    // private to emulate a "non-copyable" class.
    DenseImageRegistrationSolver(DenseImageRegistrationSolver const &);
    DenseImageRegistrationSolver & operator = (DenseImageRegistrationSolver const &);
};


#endif // #ifndef _DENSE_IM_REG_CPU_HPP



