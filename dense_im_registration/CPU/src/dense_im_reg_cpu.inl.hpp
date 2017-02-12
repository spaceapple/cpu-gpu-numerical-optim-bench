
/*
* Authors:
* Nicolas Stoiber (nicolas.stoi@gmail.com)
*
* 2016
*/

/*
* Template member functions definition for dense_im_reg_cpu.inl.hpp
*/

template <typename FloatPrec>
Common::ErrCode
eigen_quad_warping_fromstd(
        const std::vector<FloatPrec> &                               i_W,
        typename DenseImageRegistrationSolver<FloatPrec>::MatrixN4 & o_W_eigen)
{
    const uint32_t N = i_W.size() / 4;
    o_W_eigen.resize(N, 4);
    typedef typename DenseImageRegistrationSolver<FloatPrec>::MatrixN4 MatN4;
    typedef typename Eigen::Map<const MatN4> MatrixN4_CstMap;
    // copy elements from i_W into o_W_eigen:
    o_W_eigen = MatrixN4_CstMap(&(i_W[0]), N, 4);
    return Common::NoError;
}

template <typename FloatPrec,
          typename VecNType>
Common::ErrCode
warp_grid(
        const cimg_library::CImg<FloatPrec> &
                i_image,
        const typename DenseImageRegistrationSolver<FloatPrec>::MatrixN2 &
                i_grid_coords,
        VecNType &
                o_pix_values)
{
    const uint32_t nb_pix = i_grid_coords.rows();
    for (uint32_t i_pixind=0; i_pixind<nb_pix; ++i_pixind) {
        o_pix_values(i_pixind) = Common::bilinear_pix_interp(
                i_image, i_grid_coords(i_pixind, 0), i_grid_coords(i_pixind, 1));
    }
    return Common::NoError;
}

template <typename FloatPrec>
Common::ErrCode
DenseImageRegistrationSolver<FloatPrec>::init(
        const uint32_t  template_width,
        const uint32_t  template_height,
        const uint32_t  nb_levels,
        const FloatPrec lvl_resz_ratio)
{
    Common::ErrCode errCode = Common::NoError;

    // compute resolution pyramid templates resolutions
    m_nb_levels = nb_levels;
    m_lvl_resz_ratio = lvl_resz_ratio;
    m_lvl_abs_resz_ratio.resize(nb_levels);
    m_lvl_templdims.resize(nb_levels);
    m_lvl_templates.resize(nb_levels);
    m_lvl_Ws.resize(nb_levels);
    m_lvl_Ws_eigen.resize(nb_levels);
    m_lvl_gridpts_eigen.resize(nb_levels);
    m_lvl_annot_pts.resize(nb_levels);

    float this_lvl_ratio = 1.0;
    for (uint32_t i_lvl = 0; i_lvl<nb_levels; ++i_lvl) {
        // warning: silent floor() due to float->int casting
        const uint32_t lvl_template_width = template_width * this_lvl_ratio;
        const uint32_t lvl_template_height = template_width * this_lvl_ratio;

        m_lvl_templates[i_lvl].resize(lvl_template_width * lvl_template_height);
        m_lvl_templdims[i_lvl].set_dim(lvl_template_width, lvl_template_height);

        errCode = Common::generate_quad_warping_coeffs(
                lvl_template_width, lvl_template_height, m_lvl_Ws[i_lvl]);
        if (errCode != Common::NoError) { return errCode; }
        errCode = eigen_quad_warping_fromstd(
                m_lvl_Ws[i_lvl], m_lvl_Ws_eigen[i_lvl]);
        if (errCode != Common::NoError) { return errCode; }

        const uint32_t nb_pix_in_templates = m_lvl_Ws_eigen[i_lvl].rows();
        m_lvl_gridpts_eigen[i_lvl].resize(nb_pix_in_templates, 2);

        m_lvl_abs_resz_ratio[i_lvl] = this_lvl_ratio;

        // ratio for next level:
        this_lvl_ratio *= lvl_resz_ratio;
    }

    m_is_init = true;
    return Common::NoError;
}


template <typename FloatPrec>
Common::ErrCode
DenseImageRegistrationSolver<FloatPrec>::set_template(
        const cimg_library::CImg<unsigned char> & i_ref_image,
        std::vector<FloatPrec> &                  i_annot_pts,
        FloatPrec                                 i_normz_factor)
{
    if (!m_is_init) {
        return Common::SolverNotInitialized;
    }

    m_normz_factor = i_normz_factor;
    m_ref_imdim = ImDim(i_ref_image.width(), i_ref_image.height());

    typedef typename Eigen::Map<const Matrix42> Matrix42_CstMap;
    Matrix42_CstMap annot_pts_eigen(&(i_annot_pts[0]), 4, 2);

    cimg_library::CImg<FloatPrec> ref_image_float(i_ref_image);
    ref_image_float *= m_normz_factor;
    cimg_library::CImg<FloatPrec> lvl_image_float;

    // initialize container member variables
    const uint32_t nb_vars = 4 * 2; // 4 corner points of 2 coordinates each
    uint32_t nb_mr_err_comp = 0;
    m_curr_pts.resize(nb_vars);
    m_lvl_errs.resize(m_nb_levels);
    m_lvl_jacos.resize(m_nb_levels);
    m_lvl_jTj.resize(m_nb_levels);
    m_lvl_jTb.resize(m_nb_levels);

    for (uint32_t i_lvl = 0; i_lvl<m_nb_levels; ++i_lvl)
    {
        m_lvl_annot_pts[i_lvl] = m_lvl_abs_resz_ratio[i_lvl] * annot_pts_eigen;

        m_lvl_gridpts_eigen[i_lvl] = m_lvl_Ws_eigen[i_lvl] * m_lvl_annot_pts[i_lvl];

        const uint32_t lvl_ref_image_width =
                m_lvl_abs_resz_ratio[i_lvl] * i_ref_image.width();
        const uint32_t lvl_ref_image_height =
                m_lvl_abs_resz_ratio[i_lvl] * i_ref_image.height();
        lvl_image_float = ref_image_float.get_resize(
                lvl_ref_image_width, lvl_ref_image_height);

        warp_grid(
                lvl_image_float,
                m_lvl_gridpts_eigen[i_lvl],
                m_lvl_templates[i_lvl]);

        const uint32_t nb_lvl_err_comp = m_lvl_templdims[i_lvl].size();
        m_lvl_errs[i_lvl].resize(nb_lvl_err_comp);
        m_lvl_jacos[i_lvl].resize(nb_lvl_err_comp, nb_vars);
        m_lvl_jTj[i_lvl].resize(nb_vars, nb_vars);
        m_lvl_jTb[i_lvl].resize(nb_vars);
        nb_mr_err_comp += nb_lvl_err_comp;
    }

    m_mr_template.resize(nb_mr_err_comp);
    m_mr_errs.resize(nb_mr_err_comp);
    m_mr_jaco.resize(nb_mr_err_comp, nb_vars);
    m_mr_jTj.resize(nb_mr_err_comp, nb_vars);
    m_mr_jTb.resize(nb_vars);
    m_delta_vars.resize(nb_vars);

    for (uint32_t i_lvl = 0; i_lvl<m_nb_levels; ++i_lvl)
    {
        // copy level templates in mr template
        m_mr_template.segment() = m_lvl_templates[i_lvl];
    }

    m_template_is_set = true;
    return Common::NoError;
}

template <typename FloatPrec>
Common::ErrCode
DenseImageRegistrationSolver<FloatPrec>::get_template_image(
        std::vector<cimg_library::CImg<unsigned char> > & o_lvl_templims) const
{
    if (!m_is_init) {
        return Common::SolverNotInitialized;
    }
    if (!m_template_is_set) {
        return Common::TemplateNotSet;
    }

    o_lvl_templims.resize(m_nb_levels);
    cimg_library::CImg<FloatPrec> tmp_lvl_im;
    for (uint32_t i_lvl = 0; i_lvl<m_nb_levels; ++i_lvl) {
        const uint32_t lvl_templ_width = m_lvl_templdims[i_lvl].width();
        const uint32_t lvl_templ_height = m_lvl_templdims[i_lvl].height();
        const uint32_t nb_pix = lvl_templ_width * lvl_templ_height;
        tmp_lvl_im.resize(lvl_templ_width, lvl_templ_height);
        // CImg structure have no padding, so we can make a direct memory copy
        // of the full image data from eigen to CImg
        memcpy(tmp_lvl_im.data(),
                &(m_lvl_templates[i_lvl](0)),
                nb_pix*sizeof(FloatPrec));
        tmp_lvl_im /= m_normz_factor;
        o_lvl_templims[i_lvl] = tmp_lvl_im;
    }
    return Common::NoError;
}


template <typename FloatPrec>
Common::ErrCode
DenseImageRegistrationSolver<FloatPrec>::register_image(
        const cimg_library::CImg<unsigned char> & i_reg_image,
        uint32_t                                  i_nb_iterations,
        std::vector<FloatPrec> &                  io_reg_pts)
{
    if (!m_is_init) {
        return Common::SolverNotInitialized;
    }
    if (!m_template_is_set) {
        return Common::TemplateNotSet;
    }

    // generate level image
    TODO

    typedef typename Eigen::Map<VecN> VecN_Map;
    VecN_Map io_reg_pts_asVecN(&(io_reg_pts[0]), m_delta_vars.size());
    m_curr_pts = io_reg_pts_eigen;
    Mat42_Map curr_pts_asMat42(&(m_curr_pts[0]), 4, 2);

    for (uint32_t i_i = 0; i_i< i_nb_iterations; ++i_i)
    {
        // compute error
        compute_multires_pix_error(curr_pts_asMat42, m_mr_errs);

        // compute error jacobian
        compute_multires_pix_jacobian(m_curr_pts, m_mr_jaco);

        // optimization step
        Common::gauss_newton_descent_step(
                    m_mr_errs, m_mr_jaco,
                    m_mr_jTj, m_mr_jTb,
                    m_delta_vars);

        m_curr_pts += m_delta_vars;

    }

    io_reg_pts_eigen = m_delta_vars;

    return Common::NoError;
}


template <typename FloatPrec>
void
DenseImageRegistrationSolver<FloatPrec>::compute_multires_pix_error(
            const Matrix42 & i_pts,
            VecN &           o_mr_pix_err)
{
    for (uint32_t i_lvl = 0; i_lvl<m_nb_levels; ++i_lvl)
    {
        m_lvl_annot_pts = m_lvl_abs_resz_ratio[i_lvl] * i_pts;

        m_lvl_gridpts_eigen[i_lvl] = m_lvl_Ws_eigen[i_lvl] * m_lvl_annot_pts[i_lvl];

        warp_grid(
                lvl_image_float,
                m_lvl_gridpts_eigen[i_lvl],
                o_mr_pix_err.segment(,));

    }
    o_mr_pix_err -= m_mr_template;
}


template <typename FloatPrec>
void
DenseImageRegistrationSolver<FloatPrec>::compute_lvl_pix_error(
            const VecN & i_pts,
            uint32_t        i_lvl,
            VecN &       o_mr_pix_err)
{
}


template <typename FloatPrec>
void
DenseImageRegistrationSolver<FloatPrec>::compute_multires_pix_jacobian(
            const VecN & i_pts,
            MatrixNN &   o_mr_pix_err)
{
}


template <typename FloatPrec>
void
DenseImageRegistrationSolver<FloatPrec>::compute_lvl_pix_jacobian(
            const VecN & i_pts,
            uint32_t     i_lvl,
            MatrixNN &   o_mr_pix_err)
{
}



