/**
 * @author Fabian Hofer, Vincent Gerlach
 *
 * Interface for netCDF files.
 **/

#include "../constants.h"
#include <iostream>
#include <string>
#include <netcdf.h>

namespace tsunami_lab
{
    namespace io
    {
        class NetCdf;
    }
}

class tsunami_lab::io::NetCdf
{
private:
    //! name of the netCDF file
    std::string m_filePath;

    //! number of cells in x dimension
    t_idx m_nx;

    //! number of cells in y dimension
    t_idx m_ny;

    //! stride length
    t_idx m_stride;

    //! time of write operation
    unsigned int m_time = 0;

    //! nc id
    int m_ncId;

    //! id of time dimension
    int m_dimTimeId;

    //! id of x dimension
    int m_dimXId;

    //! id of y dimension
    int m_dimYId;

    //! id of time
    int m_timeId;

    //! id of total height
    int m_totalHeightId;

    //! id of bahtymetry
    int m_bathymetryId;

    //! id of momentumX
    int m_momentumXId;

    //! id of momentumY
    int m_momentumYId;

public:
    /**
     * Check if command has worked.
     * @param i_err 0 if everything is ok, else 1
     * @param text text to log
     */
    void checkNcErr( int i_err, std::string text );

    /**
     * Constructor of NetCdf.
     *
     * @param filePath filepath of the netCDF file
     * @param l_nx len in x dimension
     * @param l_ny len in y dimension
     * @param
     */
    NetCdf( std::string filePath,
            t_idx l_nx,
            t_idx l_ny,
            t_idx l_stride);

    /**
     * Destructor of NetCdf.
     */
    ~NetCdf();

    /**
     * Write current time step to a netCDF file.
     *
     * @param totalHeight total heights of cells
     * @param bathymetry bathymetry of cells
     * @param momentumX momentum of cells in x direction
     * @param momentumY momentum of cells in y direction
     */
    void write( const t_real *totalHeight,
                const t_real *bathymetry,
                const t_real *momentumX,
                const t_real *momentumY );
};