
project(dense_im_reg_cpu_bench)
set(APP_NAME bench_dense_im_reg_cpu)

cmake_minimum_required(VERSION 3.0)

# Make the scripts available in the 'cmake' directory available for the
# 'include()' command,
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/cmake)

# Definition and compilation flags:
if(WIN32)
    # Windows specifics
    add_definitions(/Ot /arch:SSE2 /fp:fast /openmp)
    if(${BITSIZE_NUM} EQUAL 64) # 64 bits
        set(PROJECT_LINK_FLAGS_PROPERTY " /MACHINE:X64 /Ob2")
    else(${BITSIZE_NUM} EQUAL 32) # 32 bits
        set(PROJECT_LINK_FLAGS_PROPERTY " /MACHINE:X86 /Ob2")
    endif(${BITSIZE_NUM} EQUAL 64)
    set(PROJECT_LINK_FLAGS_PROPERTY "/OPT:NOREF")

elseif(MINGW OR UNIX)
    # Linux specifics
    set(DXYZ_RELEASE_OPTIM_FLAGS "-O2 -msse2 -fopenmp")
    set(DXYZ_RELWITHDEBINFO_OPTIM_FLAGS "-O2 -ggdb -msse2 -fopenmp -fno-omit-frame-pointer")
    set(DXYZ_DEBUG_OPTIM_FLAGS "-O0 -ggdb")
    set(DXYZ_COMPILE_FLAGS "-Werror -Wall -Wno-sign-compare -Wno-unused-local-typedefs -Wno-unknown-pragmas -fvisibility=hidden -fvisibility-inlines-hidden -Wno-unknown-warning-option")
    set(CMAKE_CXX_FLAGS_RELEASE ${DXYZ_RELEASE_OPTIM_FLAGS} CACHE STRING
        "Tune the release flags" FORCE)
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO ${DXYZ_RELWITHDEBINFO_OPTIM_FLAGS} CACHE STRING
        "Tune the RelWithDebInfo flags" FORCE)
    set(CMAKE_CXX_FLAGS_DEBUG ${DXYZ_DEBUG_OPTIM_FLAGS} CACHE STRING
        "Tune the RelWithDebInfo flags" FORCE)
    set(CMAKE_CXX_FLAGS ${DXYZ_COMPILE_FLAGS} CACHE STRING "Tune the always present flags" FORCE)

else()
    message(SEND_ERROR "Unknown platform.")
endif()

# find packages
set(EIGEN3_INCLUDE_DIR libs/eigen CACHE PATH "path to Eigen3")
set(CIMG_INCLUDE_DIR libs/CImg CACHE PATH "path to CImg")

set(appTarget_src
    src/dense_im_reg_cpu_bench.cpp
    )

set(MY_INCLUDE_DIRS
    ${EIGEN3_INCLUDE_DIR}
    ${CIMG_INCLUDE_DIR}
    )

target_include_directories(
    ${APP_NAME}
    ${MY_INCLUDE_DIRS}
    )

link_directories(
    )

add_executable(${APP_NAME}
    ${appTarget_src}
)

target_link_libraries(${APP_NAME}
)



