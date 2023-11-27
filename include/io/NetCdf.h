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
    std::string m_filePath = "";

    //! number of cells in x dimension
    t_idx m_nx = 1;

    //! number of cells in y dimension
    t_idx m_ny = 1;

    //! scale in x dimension in meters
    t_idx m_scaleX = 1;

    //! scale in y dimension in meters
    t_idx m_scaleY = 1;

    //! stride length
    t_idx m_stride = 1;

    //! time of write operation
    size_t m_time = 0;

    //! id of the netCDF file
    int m_ncId = -1;

    //! id of time dimension
    int m_dimTimeId = -1;

    //! id of x dimension
    int m_dimXId = -1;

    //! id of longitude
    int m_xId = -1;

    //! id of y dimension
    int m_dimYId = -1;

    //! id of latitude
    int m_yId = -1;

    //! id of time
    int m_timeId = -1;

    //! id of total height
    int m_totalHeightId = -1;

    //! id of bathymetry
    int m_bathymetryId = -1;

    //! id of momentumX
    int m_momentumXId = -1;

    //! id of momentumY
    int m_momentumYId = -1;

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
        void* array = nullptr;

        //! type of the array
        VarType type = VarType::INT;

        //! length of the array
        size_t length = 0;

        //! stride of the array used for 2D arrays representation. If the array is 1D then the stride is the same as the length.
        size_t stride = 0;

        /**
         * Destructor of VarArray to delete the allocated array
        */
        ~VarArray();
    };

private:

    /**
     * read the data from the file at the given filepath and parse all given variables.
     * the outData array contains all the needed information (array, type, length, stride) to work with the data
     *
     *
     * @param filepath the path to the netCDF file
     * @param variableName the names of the variables to parse
     * @param outData the parsed data for each variable with the type, length and stride. If the VarArray is destroyed the read data 'VarArray::array' is destroyed to.
     * @param timeStep start of data parsing. The timeStep uses the dimension directly therefore index values are required e.g. 0, 1, 2, ...
     * @param size the size of outData and variableName
    */
    void _read( const char* filepath,
                const char** variableName,
                VarArray* outData,
                size_t timeStep,
                size_t size );

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
     * @param useSpherical use spherical Longitude & Latitude in degrees for the X and Y Axis instead of meters
     */
    NetCdf( std::string filePath,
            t_idx l_nx,
            t_idx l_ny,
            t_real l_scaleX,
            t_real l_scaleY,
            t_idx l_stride,
            bool useSpherical = true );

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
     * @param outData the parsed data for each variable with the type, length and stride. If the VarArray is destroyed the read data 'VarArray::array' is destroyed to.
     * @param timeStep OPTIONAL start of data parsing. The timeStep uses the dimension directly therefore index values are required e.g. 0, 1, 2, ...
    */
    template <size_t N>
    void read( const char* filepath,
               const char* ( &variableName )[N],
               VarArray( &outData )[N],
               size_t timeStep = 0 )
    {
        // NOTE: this function needs to be in the header because a template is used
        _read( filepath, variableName, outData, timeStep, N );
    }

    /**
     * read the data from the file at the given filepath and parse the one given variable.
     *
     * @param filepath filepath the path to the netCDF file
     * @param variableName  the names of the variable to parse
     * @param outData the parsed data with the type, length and stride. If the VarArray is destroyed the read data 'VarArray::array' is destroyed to.
     * @param timeStep OPTIONAL start of data parsing. The timeStep uses the dimension directly therefore index values are required e.g. 0, 1, 2, ...
    */
    void read( const char* filepath,
               const char* variableName,
               VarArray& outData,
               size_t timeStep = 0 );
};

#endif // !TSUNAMISIMULATION_NETCDF_H