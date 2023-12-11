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
#include <cstring>
#include <limits>
#include "../../include/io/NetCdf.h"

void tsunami_lab::io::NetCdf::_read( const char* filepath,
                                     const char** variableName,
                                     VarArray* outData,
                                     long long int timeStep,
                                     size_t size )
{
    if( !isReadMode )
    {
        std::cerr << "This netCdf object is not initialized in read mode. Write mode can only be used to write to a file." << std::endl;
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

    // check of time, date, t, T and get the corresponding dimension size
    int timeID = -1;
    size_t timeSize = 1;

    const char* timeNames[] = { "time", "date", "t", "T" };
    int timeNameSize = sizeof( timeNames ) / sizeof( char* );
    int timeNameIndex = 0;
    do
    {
        l_err = nc_inq_dimid( ncID, timeNames[timeNameIndex], &timeID );
    } while( l_err && ++timeNameIndex < timeNameSize );

    if( timeStep != 0 )
    {
        checkNcErr( l_err, "readTime" );

        l_err = nc_inq_dimlen( ncID, timeID, &timeSize );
        checkNcErr( l_err, "readTimeSize" );

        // also support reading array from back with negativ values
        timeStep = ( timeSize + timeStep ) % timeSize;

        if( timeStep < 0 || static_cast<size_t>( timeStep ) >= timeSize )
        {
            std::cerr << "ERROR: The Timestep can not be higher than the available time dimensions in this file (" << filepath << ")" << std::endl;
            exit( 2 );
        }
    }
    else if( l_err == NC_ENOTVAR )
    {
        timeID = -1;
    }

    // get the variables with their data
    for( size_t i = 0; i < size; i++ )
    {
        // get the variable
        const char* name = variableName[i];
        int varID;
        l_err = nc_inq_varid( ncID, name, &varID );
        if( l_err == NC_ENOTVAR ) // skip not found variables with WARNING
        {
            std::cerr << "WARNING: Could not find a variable with name: " << name << ". Writing nothing at index" << i << std::endl;
            continue;
        }
        checkNcErr( l_err, "readVarID" );

        // get the variable type
        nc_type varType;
        l_err = nc_inq_vartype( ncID, varID, &varType );
        checkNcErr( l_err, "readVarType" );
        if( varType == NC_NAT )
        {
            std::cerr << "ERROR: The parsed type of the variable is NAT (Not a Type)" << name << std::endl;
            exit( 2 );
        }
        else if( varType == NC_BYTE )
        {
            outData[i].type = VarType::UCHAR;
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
            std::cerr << "The given variable (" << name << ") does not have any dimensions" << std::endl;
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

        for( int i = 0; i < varDimCount - 1; i++ )
        {
            int varDim = varDims[i];
            if( varDim == timeID )
            {
                start[i] = timeStep;
                continue;
            }

            size_t dimLength = 1;
            l_err = nc_inq_dimlen( ncID,
                                   varDims[i],
                                   &dimLength );
            checkNcErr( l_err, "getDimLen" );
            length *= dimLength;
            count[i] = dimLength;
        }
        if( varDimCount >= 1 )
        {
            l_err = nc_inq_dimlen( ncID,
                                   varDims[varDimCount - 1],
                                   &stride );
            checkNcErr( l_err, "getDimLen" );
            length *= stride;
            count[varDimCount - 1] = stride;
        }

        // Warning for small size
        if( length <= 0 )
        {
            const char* reset = "\033[0m";
            const char* yellow = "\033[33;49m";

            std::cout << yellow << "WARNING: less than one value were read from the variable " << variableName[i] << reset << std::endl;
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
            case VarType::NONE:
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

void tsunami_lab::io::NetCdf::_write( const t_real simulationTime,
                                      const t_real* totalHeight,
                                      const t_real* momentumX,
                                      const t_real* momentumY,
                                      const t_idx nx,
                                      const t_idx ny,
                                      const t_idx stride,
                                      const t_idx writeCount )
{

    if( isReadMode )
    {
        std::cerr << "This netCdf object is not initialized in write mode. Read mode can only be used to read from files." << std::endl;
        exit( 2 );
    }

    int l_err;
    size_t startNC[3] = { m_time, 0, 0 };
    size_t countNC[3] = { 1, ny, nx };
    ptrdiff_t strideNC[3] = { 1, 1, 1 };
    ptrdiff_t mapNC[3] = { 1, static_cast<ptrdiff_t>( stride ), 1 };
    size_t indexNC[1] = { m_time }; // index should be same as current time dimension

    // write data
    l_err = nc_put_var1_float( m_ncId,              // ncid
                               m_timeId,            // varid
                               indexNC,               // indexp
                               &simulationTime );   // op
    checkNcErr( l_err, "putTime" );

    if( totalHeight != nullptr )
    {
        l_err = nc_put_varm_float( m_ncId,          // ncid
                                   m_totalHeightId, // varid
                                   startNC,         // startp
                                   countNC,         // countp
                                   strideNC,        // stridep
                                   mapNC,           // imap
                                   totalHeight );   // op
        checkNcErr( l_err, "putTotalHeight" );
    }

    if( momentumX != nullptr && m_momentumXId >= 0 )
    {
        l_err = nc_put_varm_float( m_ncId,          // ncid
                                   m_momentumXId,   // varid
                                   startNC,         // startp
                                   countNC,         // countp
                                   strideNC,        // stridep
                                   mapNC,           // imap
                                   momentumX );     // op
        checkNcErr( l_err, "putMomentumX" );
    }

    if( momentumY != nullptr && m_momentumYId >= 0 )
    {
        l_err = nc_put_varm_float( m_ncId,          // ncid
                                   m_momentumYId,   // varid
                                   startNC,         // startp
                                   countNC,         // countp
                                   strideNC,        // stridep
                                   mapNC,           // imap
                                   momentumY );     // op
        checkNcErr( l_err, "putMomentumY" );
    }

    if( m_writeCountId >= 0 )
    {
        indexNC[0] = 0;
        int intWriteCount = static_cast<int>( writeCount );
        l_err = nc_put_var1_int( m_ncId,
                                 m_writeCountId,
                                 indexNC,
                                 &intWriteCount );
        checkNcErr( l_err, "putWriteCount" );
    }

#ifndef TSUNAMI_SIMULATION_TEST
    std::cout << " writing to '" << m_filePath << "'" << std::endl;
#endif //!TSUNAMI_SIMULATION_TEST
    ++m_time;

    nc_sync( m_ncId );
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

tsunami_lab::io::NetCdf::NetCdf( t_idx writeStep,
                                 std::string filePath,
                                 t_idx l_nx,
                                 t_idx l_ny,
                                 t_idx l_k,
                                 t_real l_scaleX,
                                 t_real l_scaleY,
                                 t_idx l_stride,
                                 const t_real* bathymetry,
                                 bool useSpherical )
    :isReadMode( false ), isCheckpoint( false )
{
    m_time = writeStep;
    m_filePath = filePath;
    m_singleCellnx = l_nx;
    m_singleCellny = l_ny;
    m_nx = l_nx / l_k;
    m_ny = l_ny / l_k;
    m_k = l_k;
    m_divideK2 = 1 / (m_k * m_k);
    m_scaleX = l_scaleX;
    m_scaleY = l_scaleY;
    m_singleCellStride = l_stride;
    m_stride = m_nx;
    int l_err;

    // set up netCDF-file
    l_err = nc_open( filePath.c_str(),
                     NC_WRITE,
                     &m_ncId );
    checkNcErr( l_err, "open" );

    l_err = nc_inq_dimid( m_ncId,
                          "time",
                          &m_dimTimeId );
    checkNcErr( l_err, "getDimTime" );

    l_err = nc_inq_dimid( m_ncId,
                          useSpherical ? "longitude" : "X",
                          &m_dimXId );
    checkNcErr( l_err, "getDimX" );

    l_err = nc_inq_dimid( m_ncId,
                          useSpherical ? "latitude" : "Y",
                          &m_dimYId );
    checkNcErr( l_err, "getDimY" );

    l_err = nc_inq_varid( m_ncId,
                          useSpherical ? "longitude" : "X",
                          &m_xId );
    checkNcErr( l_err, "getVarX" );

    l_err = nc_inq_varid( m_ncId,
                          useSpherical ? "latitude" : "Y",
                          &m_yId );
    checkNcErr( l_err, "getVarY" );

    l_err = nc_inq_varid( m_ncId,
                          "time",
                          &m_timeId );
    checkNcErr( l_err, "getVarTime" );

    l_err = nc_inq_varid( m_ncId,
                          "totalHeight",
                          &m_totalHeightId );
    checkNcErr( l_err, "getVarTotalHeight" );

    if( bathymetry != nullptr )
    {
        l_err = nc_inq_varid( m_ncId,
                              "bathymetry",
                              &m_bathymetryId );
        checkNcErr( l_err, "getVarBathymetry" );
    }


    l_err = nc_inq_varid( m_ncId,
                          "momentumX",
                          &m_momentumXId );
    if( l_err == NC_ENOTVAR )
    {
        m_momentumXId = -1;
    }
    else
    {
        checkNcErr( l_err, "getVarMomentumX" );
    }

    l_err = nc_inq_varid( m_ncId,
                          "momentumY",
                          &m_momentumYId );
    if( l_err == NC_ENOTVAR )
    {
        m_momentumYId = -1;
    }
    else
    {
        checkNcErr( l_err, "getVarMomentumY" );
    }
}

tsunami_lab::io::NetCdf::NetCdf( std::string filePath,
                                 t_idx l_nx,
                                 t_idx l_ny,
                                 t_idx l_k,
                                 t_real l_scaleX,
                                 t_real l_scaleY,
                                 t_idx l_stride,
                                 const t_real* bathymetry,
                                 bool useSpherical,
                                 bool useMomenta,
                                 const char* commandLine,
                                 t_real hMax )
    : isReadMode( false ), isCheckpoint( commandLine[0] != '\0' ), commandLine( commandLine )
{
    m_filePath = filePath;
    m_singleCellnx = l_nx;
    m_singleCellny = l_ny;
    m_nx = l_nx / l_k;
    m_ny = l_ny / l_k;
    m_k = l_k;
    m_divideK2 = 1 / (m_k * m_k);
    m_scaleX = l_scaleX;
    m_scaleY = l_scaleY;
    m_singleCellStride = l_stride;
    m_stride = m_nx;
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

    int strDimID = -1;
    if( isCheckpoint )
    {
        l_err = nc_def_dim( m_ncId,
                            "strDim",
                            strlen( commandLine ) + 1,
                            &strDimID );
        checkNcErr( l_err, "strDim" );
    }

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

    if( bathymetry != nullptr )
    {
        l_err = nc_def_var( m_ncId,             // ncid
                            "bathymetry",       // name
                            NC_FLOAT,           // xtype
                            2,                  // ndims
                            m_dimIds + 1,       // dimidsp
                            &m_bathymetryId );  // varidp
        checkNcErr( l_err, "bathymetry" );
    }

    if( useMomenta )
    {
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
    }

    int commandLineID = -1;
    if( isCheckpoint )
    {
        l_err = nc_def_var( m_ncId,
                            "commandLine",
                            NC_CHAR,
                            1,
                            &strDimID,
                            &commandLineID );
        checkNcErr( l_err, "commandLine" );

        l_err = nc_def_var( m_ncId,
                            "writeCount",
                            NC_INT,
                            1,
                            &m_dimTimeId,
                            &m_writeCountId );
        checkNcErr( l_err, "writeCount" );

        l_err = nc_def_var( m_ncId,
                            "hMax",
                            NC_FLOAT,
                            1,
                            &m_dimTimeId,
                            &m_hMaxID );
        checkNcErr( l_err, "hMax" );
    }

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


    if( useMomenta )
    {
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
    }

    l_err = nc_enddef( m_ncId ); // ncid
    checkNcErr( l_err, "enddef" );

    // write longitude and latitude
    t_idx xCount = l_nx - 1;
    t_idx yCount = l_ny - 1;
    if( xCount <= 0 )
    {
        xCount = 1;
    }
    if( yCount <= 0 )
    {
        yCount = 1;
    }
    t_real stepLat = m_scaleY / static_cast<tsunami_lab::t_real>( yCount );
    t_real stepLon = m_scaleX / static_cast<tsunami_lab::t_real>( xCount );
    if( useSpherical )
    {
        t_real maxLat = m_scaleY / t_real( 110574 );
        t_real maxLon = m_scaleX / ( 111320 * std::cos( maxLat * M_PI / 180 ) );
        stepLat = maxLat / static_cast<tsunami_lab::t_real>( yCount );
        stepLon = maxLon / static_cast<tsunami_lab::t_real>( xCount );
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

    l_err = nc_put_var_float( m_ncId,       // ncid
                              m_yId,        // varid
                              lat );        // op
    checkNcErr( l_err, "putLatitude" );

    l_err = nc_put_var_float( m_ncId,       // ncid
                              m_xId,        // varid
                              lon );        // op
    checkNcErr( l_err, "putLongitude" );

    delete[] lat;
    delete[] lon;

    size_t start[3] = { m_time, 0, 0 };
    size_t count[3] = { 1, m_ny, m_nx };
    ptrdiff_t stride[3] = { 1, 1, 1 };
    ptrdiff_t map[3] = { 1, static_cast<ptrdiff_t>( m_stride ), 1 };
    size_t index[1] = { 0 };
    if( bathymetry != nullptr )
    {
        t_idx l_size = m_nx * m_ny;
        t_idx l_index = 0;
        t_idx l_k2 = m_k * m_k;

        t_real l_avgBathymetry = 0;
        t_real* l_bathymetry = new t_real[l_size]{ 0 };

        for( t_idx y = 0; y < m_singleCellny; y += m_k )
        {
            for( t_idx x = 0; x < m_singleCellnx; x += m_k )
            {
                for( t_idx i_y = y; i_y < y + m_k; i_y++ )
                {
                    for( t_idx i_x = x; i_x < x + m_k; i_x++ )
                    {
                        l_avgBathymetry += bathymetry[( i_y * m_singleCellStride ) + i_x];
                    }
                }
                // write combined cell to arrays
                l_bathymetry[l_index] = l_avgBathymetry / l_k2;
                l_index++;
                // reset average values
                l_avgBathymetry = 0;
            }
        }

        l_err = nc_put_varm_float( m_ncId,          // ncid
                                   m_bathymetryId,  // varid
                                   start + 1,       // startp
                                   count + 1,       // countp
                                   stride + 1,      // stridep
                                   map + 1,         // imap
                                   l_bathymetry );  // op
        checkNcErr( l_err, "putBathymetry" );

        delete[] l_bathymetry;
    }

    if( isCheckpoint )
    {
        start[0] = 0;
        count[0] = std::strlen( commandLine ) + 1;
        l_err = nc_put_vara( m_ncId,
                             commandLineID,
                             start,
                             count,
                             commandLine );
        checkNcErr( l_err, "putCommandLine" );

        index[0] = 0;
        l_err = nc_put_var1_float( m_ncId,
                                   m_hMaxID,
                                   index,
                                   &hMax );
        checkNcErr( l_err, "putHMax" );
    }
}

tsunami_lab::io::NetCdf::NetCdf( std::string filePath,
                                 t_idx l_nx,
                                 t_idx l_ny,
                                 t_idx l_k,
                                 t_real l_scaleX,
                                 t_real l_scaleY,
                                 t_idx l_stride,
                                 const t_real* bathymetry,
                                 bool useSpherical,
                                 bool useMomenta )
    : NetCdf( filePath, l_nx, l_ny, l_k, l_scaleX, l_scaleY, l_stride, bathymetry, useSpherical, useMomenta, "", 1 )
{
    m_divideK2 = 1 / (m_k * m_k);
}

tsunami_lab::io::NetCdf::NetCdf( std::string filePath,
                                 t_idx l_nx,
                                 t_idx l_ny,
                                 t_real l_scaleX,
                                 t_real l_scaleY,
                                 t_idx l_stride,
                                 const t_real* bathymetry,
                                 const char* commandLine,
                                 t_real hMax,
                                 const t_real* totalHeight,
                                 const t_real* momentumX,
                                 const t_real* momentumY,
                                 t_real simulationTime,
                                 const t_idx writeCount )
    : NetCdf( filePath, l_nx, l_ny, 1, l_scaleX, l_scaleY, l_stride, bathymetry, false, true, commandLine, hMax )
{
    _write( simulationTime, totalHeight, momentumX, momentumY, l_nx, l_ny, l_stride, writeCount );
}

tsunami_lab::io::NetCdf::~NetCdf()
{
    if( !isReadMode )
    {
        int l_err;
        // close file
        l_err = nc_close( m_ncId ); // ncid
        checkNcErr( l_err, "close" );
#ifndef TSUNAMI_SIMULATION_TEST
        std::cout << "finished writing to '" << m_filePath << "'. Use ncdump to view its contents." << std::endl;
#endif //!TSUNAMI_SIMULATION_TEST
    }
}

void tsunami_lab::io::NetCdf::write( const t_real simulationTime,
                                     const t_real* totalHeight,
                                     const t_real* momentumX,
                                     const t_real* momentumY )
{
    _write( simulationTime,
            totalHeight,
            momentumX,
            momentumY,
            m_singleCellnx,
            m_singleCellny,
            m_singleCellStride,
            0 );
}

void tsunami_lab::io::NetCdf::read( const char* filepath,
                                    const char* variableName,
                                    VarArray& outData,
                                    long long int timeStep )
{
    _read( filepath, &variableName, &outData, timeStep, 1 );
}

void tsunami_lab::io::NetCdf::averageSeveral( const t_real simulationTime,
                                              const t_real* totalHeight,
                                              const t_real* momentumX,
                                              const t_real* momentumY )
{
    t_idx l_size = m_nx * m_ny;
    t_idx l_index = 0;

    t_real l_avgHeight = 0;
    t_real l_avgMomentumX = 0;
    t_real l_avgMomentumY = 0;

    t_real* l_totalHeight = new t_real[l_size]{ 0 };
    t_real* l_momentumX = new t_real[l_size]{ 0 };
    t_real* l_momentumY = new t_real[l_size]{ 0 };

    for( t_idx y = 0; y < m_singleCellny; y += m_k )
    {
        for( t_idx x = 0; x < m_singleCellnx; x += m_k )
        {
            for( t_idx i_y = y; i_y < y + m_k; i_y++ )
            {
                for( t_idx i_x = x; i_x < x + m_k; i_x++ )
                {
                    l_avgHeight += totalHeight[( i_y * m_singleCellStride ) + i_x];
                    l_avgMomentumX += momentumX[( i_y * m_singleCellStride ) + i_x];
                    l_avgMomentumY += momentumY[( i_y * m_singleCellStride ) + i_x];
                }
            }
            // write combined cell to arrays
            l_totalHeight[l_index] = l_avgHeight * m_divideK2;
            l_momentumX[l_index] = l_avgMomentumX * m_divideK2;
            l_momentumY[l_index] = l_avgMomentumY * m_divideK2;
            l_index++;
            // reset average values
            l_avgHeight = 0;
            l_avgMomentumX = 0;
            l_avgMomentumY = 0;
        }
    }

    _write( simulationTime, l_totalHeight, l_momentumX, l_momentumY, m_nx, m_ny, m_stride, 0 );

    delete[] l_totalHeight;
    delete[] l_momentumX;
    delete[] l_momentumY;
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
        case VarType::NONE:
            break;
    }
}
