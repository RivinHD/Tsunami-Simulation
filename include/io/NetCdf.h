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

    //! number of cells in x dimension when not using cell combination
    t_idx m_singleCellnx = 1;

    //! number of cells in y dimension when not using cell combination
    t_idx m_singleCellny = 1;

    //! number of cells in x dimension
    t_idx m_nx = 1;

    //! number of cells in y dimension
    t_idx m_ny = 1;

    //! number of cells to average several neighboring cells of the computational grid into one cell
    t_idx m_k = 1;

    //! number of cells to divide after summation
    t_idx m_divideK2 = 1;

    //! scale in x dimension in meters
    t_idx m_scaleX = 1;

    //! scale in y dimension in meters
    t_idx m_scaleY = 1;

    //! stride length when not using cell combination
    t_idx m_singleCellStride = 1;

    //! stride length when using cell combination
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

    //! id of writeCount
    int m_writeCountId = -1;

    //! id of hMax
    int m_hMaxID = -1;

    //! indicates whether this is a checkpoint
    bool isCheckpoint = false;

    //! the current command line only used if this is a checkpoint
    const char* commandLine = "";

public:
    /**
     * The data type parsed from a netCDF file.
     * The given options contain the same value as the nc_type e.g. VarType::CHAR == NC_CHAR.
    */
    enum VarType
    {
        NONE = 0,
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
        VarType type = VarType::NONE;

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
     * If the timeStep does not matter or should be ignored set it to zero.
     *
     * @param filepath the path to the netCDF file
     * @param variableName the names of the variables to parse
     * @param outData the parsed data for each variable with the type, length and stride. If the VarArray is destroyed the read data 'VarArray::array' is destroyed to.
     * @param timeStep start of data parsing. The timeStep uses the dimension directly therefore index values are required e.g. 0, 1, 2, ... . Can also be negativ to read from the back.
     * @param size the size of outData and variableName
    */
    void _read( const char* filepath,
                const char** variableName,
                VarArray* outData,
                long long int timeStep,
                size_t size );

    /**
     * Write current time step to a netCDF file.
     *
     * @param simulationTime the current simulation time in seconds
     * @param totalHeight total heights of cells
     * @param momentumX momentum of cells in x direction
     * @param momentumY momentum of cells in y direction
     * @param nx len in x dimension
     * @param ny len in y dimension
     * @param stride the stride of the data-set to write
     * @param writeCount the current simulation internal write count  (only used for checkpoint)
     */

    void _write( const t_real simulationTime,
                 const t_real* totalHeight,
                 const t_real* momentumX,
                 const t_real* momentumY,
                 const t_idx nx,
                 const t_idx ny,
                 const t_idx stride,
                 const t_idx writeCount );

    /**
     * Write-Only Constructor of NetCdf.
     *
     * @param filePath filepath of the netCDF file
     * @param l_nx len in x dimension
     * @param l_ny len in y dimension
     * @param l_k number of cells to average several neighboring cells of the computational grid into one cell
     * @param l_scaleX the scale in x direction in meters
     * @param l_scaleY the scale in y direction in meters
     * @param l_stride the stride of the data-set to write
     * @param bathymetry bathymetry data to write if no bathymetry should be written pass a nullptr
     * @param useSpherical use spherical Longitude & Latitude in degrees for the X and Y Axis instead of meters
     * @param useMomenta if true also create variables for momentumX and momentumY and enable writing to these
     * @param commandLine the current input of the commandLine as a string and if empty the writer does not write checkpoint data. Is not used if the file is not a checkpoint.
     * @param hMax the current hMax. Is not used if the file is not a checkpoint.
     */
    NetCdf( std::string filePath,
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
            t_real hMax );

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
     * Write-Only Constructor of NetCdf for the continuation from a checkpoint.
     *
     * @param writeStep the writeStep of the checkpoint
     * @param filePath filepath of the netCDF file
     * @param l_nx len in x dimension
     * @param l_ny len in y dimension
     * @param l_k number of cells to average several neighboring cells of the computational grid into one cell
     * @param l_scaleX the scale in x direction in meters
     * @param l_scaleY the scale in y direction in meters
     * @param l_stride the stride of the data-set to write
     * @param bathymetry bathymetry data to write if no bathymetry should be written pass a nullptr
     * @param useSpherical use spherical Longitude & Latitude in degrees for the X and Y Axis instead of meters
     */
    NetCdf( t_idx writeStep,
            std::string filePath,
            t_idx l_nx,
            t_idx l_ny,
            t_idx l_k,
            t_real l_scaleX,
            t_real l_scaleY,
            t_idx l_stride,
            const t_real* bathymetry,
            bool useSpherical );

    /**
     * Write-Only Constructor of NetCdf.
     *
     * @param filePath filepath of the netCDF file
     * @param l_nx len in x dimension
     * @param l_ny len in y dimension
     * @param l_k number of cells to average several neighboring cells of the computational grid into one cell
     * @param l_scaleX the scale in x direction in meters
     * @param l_scaleY the scale in y direction in meters
     * @param l_stride the stride of the data-set to write
     * @param bathymetry bathymetry data to write if no bathymetry should be written pass a nullptr
     * @param useSpherical use spherical Longitude & Latitude in degrees for the X and Y Axis instead of meters
     * @param useMomenta if true also create variables for momentumX and momentumY and enable writing to these
     */
    NetCdf( std::string filePath,
            t_idx l_nx,
            t_idx l_ny,
            t_idx l_k,
            t_real l_scaleX,
            t_real l_scaleY,
            t_idx l_stride,
            const t_real* bathymetry,
            bool useSpherical,
            bool useMomenta );

    /**
     * Write-Only Constructor of NetCdf for Checkpoints.
     *
     * @param filePath filepath of the netCDF file
     * @param l_nx len in x dimension
     * @param l_ny len in y dimension
     * @param l_scaleX the scale in x direction in meters
     * @param l_scaleY the scale in y direction in meters
     * @param l_stride the stride of the data-set to write
     * @param bathymetry bathymetry data to write
     * @param commandLine the current input of the commandLine as a string. Is not used if the file is not a checkpoint.
     * @param hMax the current hMax. Is not used if the file is not a checkpoint.
     * @param totalHeight total heights of cells
     * @param momentumX momentum of cells in x direction
     * @param momentumY momentum of cells in y direction
     * @param simulationTime the current simulation time in seconds
     * @param writeCount the current simulation internal write count
     */
    NetCdf( std::string filePath,
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
            const t_idx writeCount );

    /**
     * Destructor of NetCdf.
     */
    ~NetCdf();

    /**
     * Averages the input arrays so that l_k cells are combined into one cell.
     *
     * @param simulationTime the current simulation time in seconds
     * @param totalHeight total heights of cells
     * @param momentumX momentum of cells in x direction
     * @param momentumY momentum of cells in y direction
     */

    void averageSeveral( const t_real simulationTime,
                         const t_real* totalHeight,
                         const t_real* momentumX,
                         const t_real* momentumY );

    /**
     * Write current time step to a netCDF file.
     *
     * @param simulationTime the current simulation time in seconds
     * @param totalHeight total heights of cells
     * @param momentumX momentum of cells in x direction
     * @param momentumY momentum of cells in y direction
     */

    void write( const t_real simulationTime,
                const t_real* totalHeight,
                const t_real* momentumX,
                const t_real* momentumY );

    /**
     * read the data from the file at the given filepath and parse all given variables.
     * the outData array contains all the needed information (array, type, length, stride) to work with the data.
     * If the timeStep does not matter or should be ignored set it to zero.
     *
     * IMPORTANT the size of variableName and outData should match N
     *
     * @tparam N number of variables to parse
     * @param filepath the path to the netCDF file
     * @param variableName the names of the variables to parse
     * @param outData the parsed data for each variable with the type, length and stride. If the VarArray is destroyed the read data 'VarArray::array' is destroyed to.
     * @param timeStep OPTIONAL start of data parsing. The timeStep uses the dimension directly therefore index values are required e.g. 0, 1, 2, ... . Can also be negativ to read from the back.
    */
    template <size_t N>
    void read( const char* filepath,
               const char* ( &variableName )[N],
               VarArray( &outData )[N],
               long long int timeStep = 0 )
    {
        // NOTE: this function needs to be in the header because a template is used
        _read( filepath, variableName, outData, timeStep, N );
    }

    /**
     * read the data from the file at the given filepath and parse the one given variable.
     * If the timeStep does not matter or should be ignored set it to zero.
     *
     * @param filepath filepath the path to the netCDF file
     * @param variableName  the names of the variable to parse
     * @param outData the parsed data with the type, length and stride. If the VarArray is destroyed the read data 'VarArray::array' is destroyed to.
     * @param timeStep OPTIONAL start of data parsing. The timeStep uses the dimension directly therefore index values are required e.g. 0, 1, 2, ... . Can also be negativ to read from the back.
    */
    void read( const char* filepath,
               const char* variableName,
               VarArray& outData,
               long long int timeStep = 0 );
};

#endif // !TSUNAMISIMULATION_NETCDF_H