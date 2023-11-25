/**
 * @author Fabian Hofer, Vincent Gerlach
 *
 * Interface for netCDF files.
 **/

#ifndef TSUNAMISIMULATION_NETCDF_H
#define TSUNAMISIMULATION_NETCDF_H

#include "../constants.h"
#include <iostream>
#include <string>
#include <netcdf.h>
#include <vector>

namespace tsunami_lab
{
    namespace io
    {
        class NetCdf;
    }
}

/**
 * Supports either reading or writing to a netCDF file
*/
class tsunami_lab::io::NetCdf
{
private:

    //! indicates whether the netCdf is in read or write mode
    bool isReadMode;

    //! name of the netCDF file
    std::string m_filePath;

    //! number of cells in x dimension
    t_idx m_nx;

    //! number of cells in y dimension
    t_idx m_ny;

    //! scale in x dimension in meters
    t_idx m_scaleX;

    //! scale in y dimension in meters
    t_idx m_scaleY;

    //! stride length
    t_idx m_stride;

    //! time of write operation
    size_t m_time = 0;

    //! id of the netCDF file
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

    //! id of bathymetry
    int m_bathymetryId;

    //! id of momentumX
    int m_momentumXId;

    //! id of momentumY
    int m_momentumYId;

public:
    /**
     * The data type parsed from a netCDF file.
     * The given options contain the same value as the nc_type e.g. VarType::CHAR == NC_CHAR.
    */
    enum VarType
    {
        CHAR = NC_CHAR,
        SHORT = NC_SHORT,
        INT = NC_INT,
        FLOAT = NC_FLOAT,
        DOUBLE = NC_DOUBLE,
        UCHAR = NC_UBYTE,
        USHORT = NC_USHORT,
        UINT = NC_UINT,
        INT64 = NC_INT64,
        UINT64 = NC_UINT64,
        STRING = NC_STRING
    };

    /**
     * struct that stores the array/data with the corresponding type, length and stride
    */
    struct VarArray
    {
        //! the array containing the data
        void* array;

        //! type of the array
        VarType type;

        //! length of the array
        size_t length;

        //! stride of the array used for 2D arrays representation. If the array is 1D then the stride is the same as the length.
        size_t stride;
    };

private:

    //! stores all data arrays that have been read to delete them if the object is destroyed. The pointers will be replaced with nullptr.
    std::vector<VarArray*> readDataArrays;

public:

    /**
     * Check if command has worked.
     * @param i_err 0 if everything is ok, else 1
     * @param text text to log
     */
    static void checkNcErr( int i_err, std::string text );

    /**
     * Read-Only Constructor of NetCdf.
    */
    NetCdf();

    /**
     * Write-Only Constructor of NetCdf.
     *
     * @param filePath filepath of the netCDF file
     * @param l_nx len in x dimension
     * @param l_ny len in y dimension
     * @param l_scaleX the scale in x direction in meters
     * @param l_scaleY the scale in y direction in meters
     * @param l_stride the stride of the data-set to write
     */
    NetCdf( std::string filePath,
            t_idx l_nx,
            t_idx l_ny,
            t_real l_scaleX,
            t_real l_scaleY,
            t_idx l_stride );

    /**
     * Destructor of NetCdf.
     */
    ~NetCdf();

    /**
     * Write current time step to a netCDF file.
     *
     * @param simulationTime the current simulation time in seconds
     * @param totalHeight total heights of cells
     * @param bathymetry bathymetry of cells
     * @param momentumX momentum of cells in x direction
     * @param momentumY momentum of cells in y direction
     */
    void write( const t_real simulationTime,
                const t_real* totalHeight,
                const t_real* bathymetry,
                const t_real* momentumX,
                const t_real* momentumY );

    /**
     * read the data from the file at the given filepath and parse all given variables.
     * the outData array contains all the needed information (array, type, length, stride) to work with the data
     *
     * IMPORTANT the size of variableName and outData should match N
     *
     * @tparam N number of variables to parse
     * @param filepath the path to the netCDF file
     * @param variableName the names of the variables to parse
     * @param outData the parsed data for each variable with the type, length and stride
     * @param timeStep OPTIONAL start for data parsing. The timeStep uses the dimension directly therefore index values are required e.g. 0, 1, 2, ...
    */
    template <size_t N>
    void read( const char* filepath,
               const char* ( &variableName )[N],
               VarArray( &outData )[N],
               size_t timeStep = 0 )
    {
        // NOTE: this function needs to be in the header because a template is used

        if( !isReadMode )
        {
            std::cerr << "This netCdf object is not initialized in read mode. Write mode can only be used to wirte to a file." << std::endl;
            exit( 2 );
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
        for( size_t i = 0; i < N; i++ )
        {
            // get the variable
            const char* name = variableName[i];
            int varID;
            l_err = nc_inq_varid( ncID, name, &varID );
            checkNcErr( l_err, std::string( "readVarID:", name ) );

            // get the variable type
            nc_type varType;
            l_err = nc_inq_vartype( ncID, varID, &varType );
            checkNcErr( l_err, std::string( "readVarType:", name ) );
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
            readDataArrays.push_back( &outData[i] );
            l_err = nc_get_vara( ncID, varID, start, count, outData[i].array );

            // free memory
            delete[] varDims;
            delete[] start;
            delete[] count;
        }

        // close the file
        l_err = nc_close( ncID );
        checkNcErr( l_err, "closeFile" );

        return;
    }

    /**
     * read the data from the file at the given filepath and parse the one given variable.
     *
     * @param filepath filepath the path to the netCDF file
     * @param variableName  the names of the variable to parse
     * @param outData the parsed data with the type, length and stride
     * @param timeStep OPTIONAL start for data parsing. The timeStep uses the dimension directly therefore index values are required e.g. 0, 1, 2, ...
    */
    void read( const char* filepath,
               const char* variableName,
               VarArray( &outData )[1],
               size_t timeStep = 0 );
};

#endif // !TSUNAMISIMULATION_NETCDF_H