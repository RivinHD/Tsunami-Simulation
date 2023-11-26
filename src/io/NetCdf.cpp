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

void tsunami_lab::io::NetCdf::_read( const char* filepath,
                                     const char** variableName,
                                     VarArray* outData,
                                     size_t timeStep,
                                     size_t size )
{
    if( !isReadMode )
    {
        std::cerr << "This netCdf object is not initialized in read mode. Write mode can only be used to wirte to a file." << std::endl;
        exit( 2 );
    }

    for( size_t i = 0; i < size; i++ )
    {
        if( outData[i].array != nullptr )
        {
            outData[i].~VarArray();
        }
    }

    int l_err;

    // open the file
    int ncID;
    l_err = nc_open( filepath, NC_NOWRITE, &ncID );
    checkNcErr( l_err, "readFile" );

    // check of lon, longitude, x, X and get the corresponding dimension size
    int lonID;
    const char* lonNames[] = { "lon", "longitude", "x", "X" };
    int lonNameSize = sizeof( lonNames ) / sizeof( char* );
    int lonNameIndex = 0;
    do
    {
        l_err = nc_inq_dimid( ncID, lonNames[lonNameIndex], &lonID );
    } while( l_err && ++lonNameIndex < lonNameSize );
    checkNcErr( l_err, "readLongitude" );

    size_t lonSize;
    l_err = nc_inq_dimlen( ncID, lonID, &lonSize );
    checkNcErr( l_err, "readLongitudeSize" );


    // check of lat, latitude, y, Y and get the corresponding dimension size
    int latID;
    const char* latNames[] = { "lat", "latitude", "y", "Y" };
    int latNameSize = sizeof( latNames ) / sizeof( char* );
    int latNameIndex = 0;
    do
    {
        l_err = nc_inq_dimid( ncID, latNames[latNameIndex], &latID );
    } while( l_err && ++latNameIndex < latNameSize );
    checkNcErr( l_err, "readLatitude" );

    size_t latSize;
    l_err = nc_inq_dimlen( ncID, latID, &latSize );
    checkNcErr( l_err, "readLatitudeSize" );

    // check of time, date, t, T and get the corresponding dimension size
    int timeID = -1;
    size_t timeSize = 1;
    if( timeStep != 0 )
    {
        const char* timeNames[] = { "time", "date", "t", "T" };
        int timeNameSize = sizeof( timeNames ) / sizeof( char* );
        int timeNameIndex = 0;
        do
        {
            l_err = nc_inq_dimid( ncID, timeNames[timeNameIndex], &timeID );
        } while( l_err && ++timeNameIndex < timeNameSize );
        checkNcErr( l_err, "readTime" );

        l_err = nc_inq_dimlen( ncID, timeID, &timeSize );
        checkNcErr( l_err, "readTimeSize" );

        if( timeStep >= timeSize )
        {
            std::cerr << "ERROR: The Timestep can not be higher than the available time dimensions in this file (" << filepath << ")" << std::endl;
            exit( 2 );
        }
    };

    // get the variables with their data
    for( size_t i = 0; i < size; i++ )
    {
        // get the variable
        const char* name = variableName[i];
        int varID;
        l_err = nc_inq_varid( ncID, name, &varID );
        checkNcErr( l_err, "readVarID" );

        // get the variable type
        nc_type varType;
        l_err = nc_inq_vartype( ncID, varID, &varType );
        checkNcErr( l_err, "readVarType" );
        if( varType == NC_NAT )
        {
            std::cerr << "ERROR: The parsed type of the variable is NAT (Not a Type)" << variableName[i] << std::endl;
            exit( 2 );
        }
        else if( varType == NC_BYTE )
        {
            outData[i].type = VarType::CHAR;
        }
        else
        {
            outData[i].type = static_cast<VarType>( varType );
        }

        // get the number of dimensions included used by the variable
        int varDimCount;
        l_err = nc_inq_varndims( ncID, varID, &varDimCount );
        checkNcErr( l_err, "readVarDimCount" );

        if( varDimCount < 1 )
        {
            std::cerr << "The given variable (" << variableName[i] << ") does not have any dimensions" << std::endl;
            exit( 2 );
        }

        // get the dimensions id's from the variable
        int* varDims = new int[varDimCount];
        l_err = nc_inq_vardimid( ncID, varID, varDims );
        checkNcErr( l_err, "readVarDimCount" );


        // parse the dimensions and calculate length, count and start
        size_t length = 1;
        size_t stride = 1;
        size_t* start = new size_t[varDimCount]{ 0 };
        size_t* count = new size_t[varDimCount];
        std::fill_n( count, varDimCount, 1 );

        if( varDimCount >= 2 )
        {
            // COARDS standard require the dimension order T, Z, Y, X
            int i = varDimCount - 4;
            i *= ( i >= 0 );

            while( i < varDimCount )
            {
                if( varDims[i] == timeID )
                {
                    start[i] = timeStep;
                    break;
                }
                i++;
            }

            if( varDims[varDimCount - 2] == latID )
            {
                count[varDimCount - 2] = latSize;
                length *= latSize;
            }
        }

        if( varDims[varDimCount - 1] == latID )
        {
            count[varDimCount - 1] = latSize;
            length *= latSize;
            stride = latSize;
        }
        else if( varDims[varDimCount - 1] == lonID )
        {
            count[varDimCount - 1] = lonSize;
            length *= lonSize;
            stride = lonSize;
        }

        // Warning for small size
        if( length <= 1 )
        {
            const char* reset = "\033[0m";
            const char* yellow = "\033[33;49m";

            std::cout << yellow << "WARNING: one or less values were read from the variable " << variableName[i] << reset << std::endl;
        }

        // initialize the data storage for the variable
        outData[i].length = length;
        outData[i].stride = stride;
        switch( outData[i].type )
        {
            case VarType::CHAR:
                outData[i].array = new char[length];
                break;
            case VarType::SHORT:
                outData[i].array = new short[length];
                break;
            case VarType::INT:
                outData[i].array = new int[length];
                break;
            case VarType::FLOAT:
                outData[i].array = new float[length];
                break;
            case VarType::DOUBLE:
                outData[i].array = new double[length];
                break;
            case VarType::UCHAR:
                outData[i].array = new unsigned char[length];
                break;
            case VarType::USHORT:
                outData[i].array = new unsigned short[length];
                break;
            case VarType::UINT:
                outData[i].array = new unsigned int[length];
                break;
            case VarType::INT64:
                outData[i].array = new int64_t[length];
                break;
            case VarType::UINT64:
                outData[i].array = new uint64_t[length];
                break;
            case VarType::STRING:
                outData[i].array = new std::string[length];
                break;
        }
        l_err = nc_get_vara( ncID, varID, start, count, outData[i].array );

        // free memory
        delete[] varDims;
        delete[] start;
        delete[] count;
    }

    // close the file
    l_err = nc_close( ncID );
    checkNcErr( l_err, "closeFile" );
}

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
                                 t_idx l_stride,
                                 bool useSpherical )
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

    l_err = nc_def_dim( m_ncId,                           // ncid
                        useSpherical ? "longitude" : "X", // name
                        m_nx,                             // len
                        &m_dimXId );                      // idp
    checkNcErr( l_err, "dimX" );

    l_err = nc_def_dim( m_ncId,                           // ncid
                        useSpherical ? "latitude" : "Y",  // name
                        m_ny,                             // len
                        &m_dimYId );                      // idp            
    checkNcErr( l_err, "dimY" );

    m_dimIds[0] = m_dimTimeId;
    m_dimIds[1] = m_dimYId;
    m_dimIds[2] = m_dimXId;

    l_err = nc_def_var( m_ncId,                            // ncid
                        useSpherical ? "longitude" : "X",  // name
                        NC_FLOAT,                          // xtype
                        1,                                 // ndims
                        &m_dimXId,                         // dimidsp
                        &m_xId );                          // varidp
    checkNcErr( l_err, "longitude" );

    l_err = nc_def_var( m_ncId,                           // ncid
                        useSpherical ? "latitude" : "Y",  // name
                        NC_FLOAT,                         // xtype
                        1,                                // ndims
                        &m_dimYId,                        // dimidsp
                        &m_yId );                         // varidp
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
                        2,                  // ndims
                        m_dimIds + 1,       // dimidsp
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
                             "COARDS" );
    checkNcErr( l_err, "coards" );

    // Add units attribute to the variable
    l_err = nc_put_att_text( m_ncId,
                             m_timeId,
                             "units",
                             7,
                             "seconds" );
    checkNcErr( l_err, "seconds" );

    l_err = nc_put_att_text( m_ncId,
                             m_xId,
                             "units",
                             useSpherical ? 12 : 6,
                             useSpherical ? "degrees_east" : "meters" );
    checkNcErr( l_err, "degrees_east" );

    l_err = nc_put_att_text( m_ncId,
                             m_yId,
                             "units",
                             useSpherical ? 13 : 6,
                             useSpherical ? "degrees_north" : "meters" );
    checkNcErr( l_err, "degrees_north" );

    l_err = nc_put_att_text( m_ncId,
                             m_totalHeightId,
                             "units",
                             6,
                             "meters" );
    checkNcErr( l_err, "metersTotalHeight" );

    l_err = nc_put_att_text( m_ncId,
                             m_bathymetryId,
                             "units",
                             6,
                             "meters" );
    checkNcErr( l_err, "metersBathymetry" );

    l_err = nc_put_att_text( m_ncId,
                             m_momentumXId,
                             "units",
                             6,
                             "meters" );
    checkNcErr( l_err, "metersMomentumX" );

    l_err = nc_put_att_text( m_ncId,
                             m_momentumYId,
                             "units",
                             6,
                             "meters" );
    checkNcErr( l_err, "metersMomentumY" );

    l_err = nc_enddef( m_ncId ); // ncid
    checkNcErr( l_err, "enddef" );

    // write longitude and latitude
    t_real stepLat = m_scaleY / ( l_ny - 1 );
    t_real stepLon = m_scaleX / ( l_nx - 1 );
    if( useSpherical )
    {
        t_real maxLat = m_scaleY / t_real( 110574 );
        t_real maxLon = m_scaleX / ( 111320 * std::cos( maxLat * M_PI / 180 ) );
        stepLat = maxLat / ( l_ny - 1 );
        stepLon = maxLon / ( l_nx - 1 );
    }
    t_real* lat = new t_real[l_ny];
    t_real* lon = new t_real[l_nx];

    for( size_t i = 0; i < l_ny; i++ )
    {
        lat[i] = i * stepLat;
    }
    for( size_t i = 0; i < l_nx; i++ )
    {
        lon[i] = i * stepLon;
    }

    l_err = nc_put_var_float( m_ncId,          // ncid
                              m_yId,    // varid
                              lat );           // op
    checkNcErr( l_err, "putLatitude" );

    l_err = nc_put_var_float( m_ncId,         // ncid
                              m_xId,  // varid
                              lon );          // op
    checkNcErr( l_err, "putLongitude" );

    delete[] lat;
    delete[] lon;
}

tsunami_lab::io::NetCdf::~NetCdf()
{
    if( !isReadMode )
    {
        int l_err;
        // close file
        l_err = nc_close( m_ncId ); // ncid
        checkNcErr( l_err, "close" );

        std::cout << "finished writing to '" << m_filePath << "'. Use ncdump to view its contents." << std::endl;
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
                               start + 1,       // startp
                               count + 1,       // countp
                               stride + 1,      // stridep
                               map + 1,         // imap
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

void tsunami_lab::io::NetCdf::read( const char* filepath,
                                    const char* variableName,
                                    VarArray& outData,
                                    size_t timeStep )
{
    _read( filepath, &variableName, &outData, timeStep, 1 );
}

tsunami_lab::io::NetCdf::VarArray::~VarArray()
{
    switch( type )
    {
        case VarType::CHAR:
            delete[] static_cast<char*>( array );
            break;
        case VarType::SHORT:
            delete[] static_cast<short*>( array );
            break;
        case VarType::INT:
            delete[] static_cast<int*>( array );
            break;
        case VarType::FLOAT:
            delete[] static_cast<float*>( array );
            break;
        case VarType::DOUBLE:
            delete[] static_cast<double*>( array );
            break;
        case VarType::UCHAR:
            delete[] static_cast<unsigned char*>( array );
            break;
        case VarType::USHORT:
            delete[] static_cast<unsigned short*>( array );
            break;
        case VarType::UINT:
            delete[] static_cast<unsigned int*>( array );
            break;
        case VarType::INT64:
            delete[] static_cast<int64_t*>( array );
            break;
        case VarType::UINT64:
            delete[] static_cast<uint64_t*>( array );
            break;
        case VarType::STRING:
            delete[] static_cast<std::string*>( array );
            break;
    }
}
