/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * IO-routines for writing a snapshot as Comma Separated Values (CSV).
 **/
#ifndef TSUNAMI_LAB_IO_CSV
#define TSUNAMI_LAB_IO_CSV

#include "../constants.h"
#include <cstring>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

namespace tsunami_lab
{
	namespace io
	{
		class Csv;
	}
}

/**
 * IO-routines for writing a snapshot as Comma Separated Values (CSV).
*/
class tsunami_lab::io::Csv
{
public:

	/**
	 * Writes the data as CSV to the given stream.
	 *
	 * @param i_dxy cell width in x- and y-direction.
	 * @param i_nx number of cells in x-direction.
	 * @param i_ny number of cells in y-direction.
	 * @param i_stride stride of the data arrays in y-direction (x is assumed to be stride-1).
	 * @param i_h water height of the cells; optional: use nullptr if not required.
	 * @param i_hu momentum in x-direction of the cells; optional: use nullptr if not required.
	 * @param i_hv momentum in y-direction of the cells; optional: use nullptr if not required.
	 * @param i_b the bathymetry at x-position; optional: use nullptr if not required.
	 * @param i_hTotal the sum of water height and bathymetry; optional: use nullptr if not required.
	 * @param io_stream stream to which the CSV-data is written.
	 **/
	static void write( t_real i_dxy,
					   t_idx i_nx,
					   t_idx i_ny,
					   t_idx i_stride,
					   t_real const* i_h,
					   t_real const* i_hu,
					   t_real const* i_hv,
					   t_real const* i_b,
					   t_real const* i_hTotal,
					   std::ostream& io_stream );

	/**
	 * get's the next parsed value pair from the middle_state.csv file stream
	 *
	 * @param stream file stream of middle_state.csv
	 * @param o_hLeft ouput the height left
	 * @param o_hRight ouput the height right
	 * @param o_huLeft output the momentum left
	 * @param o_huRight output the momentum right
	 * @param o_hStar output the computed height
	 * @return success
	 */
	static bool next_middle_states( std::ifstream& stream,
									t_real& o_hLeft,
									t_real& o_hRight,
									t_real& o_huLeft,
									t_real& o_huRight,
									t_real& o_hStar );
};

#endif