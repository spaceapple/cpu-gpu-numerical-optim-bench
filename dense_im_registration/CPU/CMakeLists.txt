
set(APP_NAME bench_dense_im_reg_cpu)

set(appTarget_src
    src/dense_im_reg_cpu_bench.cpp
    )

add_executable(${APP_NAME}
    ${appTarget_src}
)

target_include_directories(
    ${APP_NAME} PUBLIC
    ${EIGEN3_INCLUDE_DIR}
    ${CIMG_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

# link_directories(
#     )
#
# target_link_libraries(${APP_NAME}
# )



