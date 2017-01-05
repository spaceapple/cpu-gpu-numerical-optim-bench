
/*
* Authors:
* Nicolas Stoiber (nicolas.stoi@gmail.com)
*
* 2016
*/

#ifndef _ANNOT_INFO_HANDLING_HPP
#define _ANNOT_INFO_HANDLING_HPP

#include <sstream>
#include <vector>

#include "errCodes.h"

namespace Common
{

/*
* Convert a string into a numerical value
* input string
* output numerical value.
*/
template<typename T>
T str2val(
        const std::string & i_str)
{
	std::istringstream iss( i_str );
    iss.imbue(std::locale("C")); // enforce C locale
	T val;
	iss >> val;
	return val;
}


/*
* Processes a line read from txt.
* Returns if the line has data content in it (not comment) and the read x and
* y elements
*/
template <typename FloatPrec>
inline
ErrCode
read_x_y_in_line(
        const std::string & i_line,
        char                i_delim,
        bool &              o_line_has_data,
        FloatPrec &         o_x,
        FloatPrec &         o_y)

{
    o_line_has_data = false;
    if (i_line.at(0) == ' ') { // line starts with space: error
        std::cerr << "Leading space on line. This is ambiguous. Exiting.\n";
        return AnnotInfoParsingError;
    }
    if (i_line.at(0) == '#') {
        // line starts with comment: ignore line content and leave
        return NoError;
    }

    // line looks valid, let's read x and y from it.
    std::stringstream ss(i_line);
    std::string item;
    std::getline(ss, item, i_delim);
    o_x = str2val<FloatPrec>(item);
    std::getline(ss, item, i_delim);
    o_y = str2val<FloatPrec>(item);
    o_line_has_data = true;
    return NoError;
}

/*
* Parse annotation file (txt file) storing coordinates of the 4 2D-points
* that draw a quadrilateral around the texture to register (for the reference
* image) or the initial tracking quad position (for the registration image).
* The functions returns the 2D-points in output variable o_pts in following
* fashion: x0, y0, x1, y1, ..., y3
* The annotation file may contain comment lines (starting with the '#'
* character.
*/
template <typename FloatPrec>
ErrCode
parse_annot_info(
        const std::string &          i_filename,
        std::vector<FloatPrec> &     o_pts,
        char                         delim = ' ')
{
    const uint32_t nb_points = 4;
    const uint32_t pts_dim = 2;

    std::ifstream txtFile;
    o_pts.resize(pts_dim * nb_points);

    txtFile.open(i_filename.c_str(), std::ifstream::in);
    if ( !txtFile.is_open() )
    {
        std::cerr << "cannot open file " << i_filename << " for reading.\n";
        return IOCantOpenFile;
    }

    uint32_t line_ind = 0;
    uint32_t pts_ind = 0;
    bool line_has_data = false;
    ErrCode errCode = NoError;
    for (std::string line; std::getline(txtFile, line); )
    {
        errCode = read_x_y_in_line<FloatPrec>(line, delim, line_has_data,
                o_pts[pts_dim * pts_ind + 0], o_pts[pts_dim * pts_ind + 1]);
        if (errCode != NoError) {
            return AnnotInfoParsingError;
        }

        if (line_has_data) {
            // line had valid x, y data in it -now stored in o_pts, we move to
            // the next point.
            pts_ind++;
        }
        line_ind++;
    }
    return NoError;
}

} // end namespace Common

#endif /* _ANNOT_INFO_HANDLING_HPP *  * */


