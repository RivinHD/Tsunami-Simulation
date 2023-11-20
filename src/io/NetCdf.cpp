/**
 * @author Fabian Hofer, Vincent Gerlach
 *
 * Interface for netCDF files.
 **/

#include <cstdlib>
#include <iostream>
#include <string>
#include "../../include/io/NetCdf.h"

void tsunami_lab::io::NetCdf::checkNcErr( int i_err, std::string text ) {
    if( i_err ) {
        std::cerr << "Error: "
                  << nc_strerror( i_err )
                  << std::endl
                  << text << std::endl;
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
    checkNcErr( l_err, "create" );

    // define dimensions
    l_err = nc_def_dim( m_ncId,      // ncid
                        "time",      // name
                        NC_UNLIMITED,// len
                        &m_timeId ); // idp
    checkNcErr( l_err, "dimTime" );

    l_err = nc_def_dim( m_ncId,      // ncid
                        "x",         // name
                        m_nx,        // len
                        &m_dimXId ); // idp
    checkNcErr( l_err, "dimX" );

    l_err = nc_def_dim( m_ncId,      // ncid
                        "y",         // name
                        m_ny,        // len
                        &m_dimYId ); // idp
    checkNcErr( l_err, "dimY" );

    m_dimIds[0] = m_dimTimeId;
    m_dimIds[1] = m_dimYId;
    m_dimIds[2] = m_dimXId;

    l_err = nc_def_var( m_ncId,             // ncid
                        "time",             // name
                        NC_INT,             // xtype
                        1,                  // ndims
                        &m_dimTimeId,       // dimidsp
                        &m_timeId );        // varidp
    checkNcErr( l_err, "timeId" );

    l_err = nc_def_var( m_ncId,             // ncid
                        "totalHeight",      // name
                        NC_FLOAT,           // xtype
                        3,                  // ndims
                        m_dimIds,           // dimidsp
                        &m_totalHeightId ); // varidp
    checkNcErr( l_err, "totalHeight" );

    l_err = nc_def_var( m_ncId,             // ncid
                        "bathymetry",       // name
                        NC_FLOAT,           // xtype
                        3,                  // ndims
                        m_dimIds,           // dimidsp
                        &m_bathymetryId );  // varidp
    checkNcErr( l_err, "bathymetry" );

    l_err = nc_def_var( m_ncId,             // ncid
                        "momentumX",        // name
                        NC_FLOAT,           // xtype
                        3,                  // ndims
                        m_dimIds,           // dimidsp
                        &m_momentumXId );   // varidp
    checkNcErr( l_err, "momentumX" );

    l_err = nc_def_var( m_ncId,             // ncid
                        "momentumY",        // name
                        NC_FLOAT,           // xtype
                        3,                  // ndims
                        m_dimIds,           // dimidsp
                        &m_momentumYId );   // varidp
    checkNcErr( l_err, "momentumY" );

    // Add units attribute to the variable
    l_err = nc_put_att_text( m_ncId,
                             m_timeId,
                             "units",
                             7,
                             "seconds");
    checkNcErr(l_err, "seconds");

    l_err = nc_enddef( m_ncId ); // ncid
    checkNcErr( l_err, "enddef" );
}

tsunami_lab::io::NetCdf::~NetCdf()
{
    int l_err;
    // close file
    l_err = nc_close( m_ncId ); // ncid
    checkNcErr( l_err, "close" );
}

void tsunami_lab::io::NetCdf::write( const t_real *totalHeight,
                                     const t_real *bathymetry,
                                     const t_real *momentumX,
                                     const t_real *momentumY )
{
    int l_err;
    size_t start[3] = { m_time++, 0, 0 };
    size_t count[3] = { 1, m_ny, m_nx };
    ptrdiff_t stride[3] = { 1, 1, 1};
    ptrdiff_t map[3] = { 1, static_cast<ptrdiff_t>(m_stride), 1 };

    std::cout << m_ncId << ", " << m_totalHeightId << ", " << m_bathymetryId << ", " << m_momentumXId << ", " << m_momentumYId << std::endl;

    // write data
    l_err = nc_put_var_uint( m_ncId,            //
                             m_timeId,
                             &m_time );

    checkNcErr( l_err, "putTime" );

    l_err = nc_put_varm_float( m_ncId,          // ncid
                               m_totalHeightId, // varid
                               start,           // startp
                               count,           // countp
                               stride,          // stridep
                               map,             // imap
                               totalHeight );   // op
    checkNcErr( l_err, "putTotalHeight" );

    l_err = nc_put_varm_float( m_ncId,          // ncid
                               m_bathymetryId,  // varid
                               start,           // startp
                               count,           // countp
                               stride,          // stridep
                               map,             // imap
                               bathymetry );    // op
    checkNcErr( l_err, "putBathymetry" );

    l_err = nc_put_varm_float( m_ncId,          // ncid
                               m_momentumXId,   // varid
                               start,           // startp
                               count,           // countp
                               stride,          // stridep
                               map,             // imap
                               momentumX );     // op
    checkNcErr( l_err, "putMomentumX" );

    l_err = nc_put_varm_float( m_ncId,          // ncid
                               m_momentumYId,   // varid
                               start,           // startp
                               count,           // countp
                               stride,          // stridep
                               map,             // imap
                               momentumY );     // op
    checkNcErr( l_err, "putMomentumY" );

    std::cout << "finished writing to '" << m_filePath << "'" << std::endl
              << "use ncdump to view its contents" << std::endl;
}

