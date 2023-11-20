/**
 * @author Fabian Hofer, Vincent Gerlach
 *
 * Interface for netCDF files.
 **/

#include <cstdlib>
#include <iostream>
#include <string>
#include "../../include/io/NetCdf.h"

void tsunami_lab::io::NetCdf::checkNcErr( int i_err ) {
    if( i_err ) {
        std::cerr << "Error: "
                  << nc_strerror( i_err )
                  << std::endl;
        exit(2);
    }
}

tsunami_lab::io::NetCdf::NetCdf( std::string filePath,
                                 t_idx l_nx,
                                 t_idx l_ny,
                                 t_idx l_stride)
{
    m_filePath = filePath;
    m_nx = l_nx;
    m_ny = l_ny;
    m_stride = l_stride;
    int l_err;
    int m_dimIds[3];

    // set up netCDF-file
    l_err = nc_create( filePath.c_str(), // path
                       NC_CLOBBER,       // cmode
                       &m_ncId );        // ncidp
    checkNcErr( l_err );

    // define dimensions
    l_err = nc_def_dim( m_ncId,      // ncid
                        "time",      // name
                        NC_UNLIMITED,// len
                        &m_dimXId ); // idp
    checkNcErr( l_err );

    l_err = nc_def_dim( m_ncId,      // ncid
                        "x",         // name
                        l_nx,        // len
                        &m_dimXId ); // idp
    checkNcErr( l_err );

    l_err = nc_def_dim( m_ncId,      // ncid
                        "y",         // name
                        l_ny,        // len
                        &m_dimYId ); // idp
    checkNcErr( l_err );

    m_dimIds[0] = m_timeId;
    m_dimIds[1] = m_dimYId;
    m_dimIds[2] = m_dimXId;

    l_err = nc_def_var( m_ncId,             // ncid
                        "totalHeight",      // name
                        NC_FLOAT,           // xtype
                        3,                  // ndims
                        m_dimIds,           // dimidsp
                        &m_totalHeightId ); // varidp
    checkNcErr( l_err );

    l_err = nc_def_var( m_ncId,             // ncid
                        "bathymetry",      // name
                        NC_FLOAT,           // xtype
                        3,                  // ndims
                        m_dimIds,           // dimidsp
                        &m_bathymetryId );  // varidp
    checkNcErr( l_err );

    l_err = nc_def_var( m_ncId,             // ncid
                        "momentumX",      // name
                        NC_FLOAT,           // xtype
                        3,                  // ndims
                        m_dimIds,           // dimidsp
                        &m_momentumXId );   // varidp
    checkNcErr( l_err );

    l_err = nc_def_var( m_ncId,             // ncid
                        "momentumY",      // name
                        NC_FLOAT,           // xtype
                        3,                  // ndims
                        m_dimIds,           // dimidsp
                        &m_momentumYId );   // varidp
    checkNcErr( l_err );

    l_err = nc_enddef( m_ncId ); // ncid
    checkNcErr( l_err );
}

void tsunami_lab::io::NetCdf::write( t_real *totalHeight,
                                     t_real *bathymetry,
                                     t_real *momentumX,
                                     t_real *momentumY )
{
    int l_err;
    size_t start[3] = { 0 };
    size_t count[3] = { m_nx*m_ny, m_nx*m_ny, m_nx*m_ny };
   ptrdiff_t stride[3] = { static_cast<ptrdiff_t>(m_stride),
                           static_cast<ptrdiff_t>(m_stride),
                           static_cast<ptrdiff_t>(m_stride) };

    // write data
    l_err = nc_put_vars_float( m_ncId,          // ncid
                               m_totalHeightId, // varid
                               start,           // startp
                               count,           // countp
                               stride,          // stridep
                               totalHeight );   // op
    checkNcErr( l_err );

    l_err = nc_put_vars_float( m_ncId,          // ncid
                               m_bathymetryId,  // varid
                               start,           // startp
                               count,           // countp
                               stride,          // stridep
                               bathymetry );    // op
    checkNcErr( l_err );

    l_err = nc_put_vars_float( m_ncId,          // ncid
                               m_momentumXId,   // varid
                               start,           // startp
                               count,           // countp
                               stride,          // stridep
                               momentumX );     // op
    checkNcErr( l_err );

    l_err = nc_put_vars_float( m_ncId,          // ncid
                               m_momentumYId,   // varid
                               start,           // startp
                               count,           // countp
                               stride,          // stridep
                               momentumY ); // op
    checkNcErr( l_err );

    // close file
    l_err = nc_close( m_ncId ); // ncid
    checkNcErr( l_err );

    std::cout << "finished writing to '" << m_filePath << "'" << std::endl
              << "use ncdump to view its contents" << std::endl;
}

