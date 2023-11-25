/**
 * @author Fabian Hofer, Vincent Gerlach
 *
 * Interface for netCDF files.
 **/

#include <cstdlib>
#include <iostream>
#include <string>
#include <cmath>
#include <cstdint>
#include "../../include/io/NetCdf.h"

void tsunami_lab::io::NetCdf::checkNcErr( int i_err, std::string text )
{
    if( i_err )
    {
        std::cerr << "Error at "
            << text << ": "
            << nc_strerror( i_err )
            << std::endl;
        exit( 2 );
    }
}

tsunami_lab::io::NetCdf::NetCdf()
    : isReadMode( true )
{
}

tsunami_lab::io::NetCdf::NetCdf( std::string filePath,
                                 t_idx l_nx,
                                 t_idx l_ny,
                                 t_real l_scaleX,
                                 t_real l_scaleY,
                                 t_idx l_stride )
    : isReadMode( false )
{
    m_filePath = filePath;
    m_nx = l_nx;
    m_ny = l_ny;
    m_scaleX = l_scaleX;
    m_scaleY = l_scaleY;
    m_stride = l_stride;
    int l_err;
    int m_dimIds[3];

    // set up netCDF-file
    l_err = nc_create( filePath.c_str(), // path
                       NC_CLOBBER,       // cmode
                       &m_ncId );        // ncidp
    checkNcErr( l_err, "create" );

    // define dimensions
    l_err = nc_def_dim( m_ncId,         // ncid
                        "time",         // name
                        NC_UNLIMITED,   // len
                        &m_dimTimeId ); // idp
    checkNcErr( l_err, "dimTime" );

    l_err = nc_def_dim( m_ncId,      // ncid
                        "longitude",         // name
                        m_nx,        // len
                        &m_dimXId ); // idp
    checkNcErr( l_err, "dimX" );

    l_err = nc_def_dim( m_ncId,      // ncid
                        "latitude",         // name
                        m_ny,        // len
                        &m_dimYId ); // idp
    checkNcErr( l_err, "dimY" );

    m_dimIds[0] = m_dimTimeId;
    m_dimIds[1] = m_dimYId;
    m_dimIds[2] = m_dimXId;

    l_err = nc_def_var( m_ncId,             // ncid
                        "longitude",        // name
                        NC_FLOAT,           // xtype
                        1,                  // ndims
                        &m_dimXId,          // dimidsp
                        &m_longitudeId );   // varidp
    checkNcErr( l_err, "longitude" );

    l_err = nc_def_var( m_ncId,             // ncid
                        "latitude",         // name
                        NC_FLOAT,           // xtype
                        1,                  // ndims
                        &m_dimYId,          // dimidsp
                        &m_latitudeId );    // varidp
    checkNcErr( l_err, "latitude" );

    l_err = nc_def_var( m_ncId,             // ncid
                        "time",             // name
                        NC_FLOAT,           // xtype
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

    // global attribute
    l_err = nc_put_att_text( m_ncId,
                             NC_GLOBAL,
                             "Conventions",
                             6,
                             "COARDS");
    checkNcErr(l_err, "coards");

    // Add units attribute to the variable
    l_err = nc_put_att_text( m_ncId,
                             m_timeId,
                             "units",
                             7,
                             "seconds" );
    checkNcErr( l_err, "seconds" );

    l_err = nc_put_att_text( m_ncId,
                             m_longitudeId,
                             "units",
                             12,
                             "degrees_east" );
    checkNcErr( l_err, "degrees_east" );

    l_err = nc_put_att_text( m_ncId,
                             m_latitudeId,
                             "units",
                             13,
                             "degrees_north" );
    checkNcErr( l_err, "degrees_north" );

    l_err = nc_enddef( m_ncId ); // ncid
    checkNcErr( l_err, "enddef" );

    // write longitude and latitude
    t_real maxLat = m_scaleY / t_real(110574);
    t_real maxLon = m_scaleX / (111320 * std::cos(maxLat * M_PI / 180));
    t_real stepLat = maxLat / (l_ny - 1);
    t_real stepLon = maxLon / (l_nx - 1);
    t_real* lat = new t_real[l_ny];
    t_real* lon = new t_real[l_nx];

    for(size_t i = 0; i < l_ny; i++)
    {
        lat[i] = i * stepLat;
        std::cout << i * stepLat << std::endl;
    }
    for(size_t i = 0; i < l_nx; i++)
    {
        lon[i] = i * stepLon;
    }

    l_err = nc_put_var_float( m_ncId,          // ncid
                              m_latitudeId,    // varid
                              lat );           // op
    checkNcErr( l_err, "putLatitude" );

    l_err = nc_put_var_float( m_ncId,         // ncid
                              m_longitudeId,  // varid
                              lon );          // op
    checkNcErr( l_err, "putLongitude" );

    std::cout << "finished writing to " << m_filePath << std::endl
        << "Use ncdump to view its contents" << std::endl;
}

tsunami_lab::io::NetCdf::~NetCdf()
{
    if( isReadMode )
    {
        for( size_t i = 0; i < readDataArrays.size(); i++ )
        {
            switch( readDataArrays[i]->type )
            {
                case VarType::CHAR:
                    delete[] static_cast<char*>( readDataArrays[i]->array );
                    break;
                case VarType::SHORT:
                    delete[] static_cast<short*>( readDataArrays[i]->array );
                    break;
                case VarType::INT:
                    delete[] static_cast<int*>( readDataArrays[i]->array );
                    break;
                case VarType::FLOAT:
                    delete[] static_cast<float*>( readDataArrays[i]->array );
                    break;
                case VarType::DOUBLE:
                    delete[] static_cast<double*>( readDataArrays[i]->array );
                    break;
                case VarType::UCHAR:
                    delete[] static_cast<unsigned char*>( readDataArrays[i]->array );
                    break;
                case VarType::USHORT:
                    delete[] static_cast<unsigned short*>( readDataArrays[i]->array );
                    break;
                case VarType::UINT:
                    delete[] static_cast<unsigned int*>( readDataArrays[i]->array );
                    break;
                case VarType::INT64:
                    delete[] static_cast<int64_t*>( readDataArrays[i]->array );
                    break;
                case VarType::UINT64:
                    delete[] static_cast<uint64_t*>( readDataArrays[i]->array );
                    break;
                case VarType::STRING:
                    delete[] static_cast<std::string*>( readDataArrays[i]->array );
                    break;
            }
            readDataArrays[i]->array = nullptr;
        }
        readDataArrays.clear();
    }
    else
    {
        int l_err;
        // close file
        l_err = nc_close( m_ncId ); // ncid
        checkNcErr( l_err, "close" );
    }
}

void tsunami_lab::io::NetCdf::write( const t_real simulationTime,
                                     const t_real* totalHeight,
                                     const t_real* bathymetry,
                                     const t_real* momentumX,
                                     const t_real* momentumY )
{
    if( isReadMode )
    {
        std::cerr << "This netCdf object is not initialized in write mode. Read mode can only be used to read from files." << std::endl;
        exit( 2 );
    }

    int l_err;
    size_t start[3] = { m_time, 0, 0 };
    size_t count[3] = { 1, m_ny, m_nx };
    ptrdiff_t stride[3] = { 1, 1, 1 };
    ptrdiff_t map[3] = { 1, static_cast<ptrdiff_t>( m_stride ), 1 };
    size_t index[1] = { m_time }; // index should be same as current time dimension

    // write data
    l_err = nc_put_var1_float( m_ncId,              // ncid
                               m_timeId,            // varid
                               index,               // indexp
                               &simulationTime );   // op
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

    std::cout << " writing to '" << m_filePath << "'" << std::endl;

    ++m_time;
}

void tsunami_lab::io::NetCdf::read( const char* filepath, const char* variableName, VarArray( &outData )[1], size_t timeStep )
{
    const char* variable[1] = { variableName };
    read( filepath, variable, outData, timeStep );
}
