.. role:: raw-html(raw)
    :format: html

.. _submissions_large_data_input_output:

5. Large Data Input and Output
==============================


5.1 NetCDF Output
-----------------

1. Installation of netCDF
^^^^^^^^^^^^^^^^^^^^^^^^^

``netCDF`` can be installed OS specific:

    **Windows**

        1. `Download <https://docs.unidata.ucar.edu/netcdf-c/current/winbin.html>_` and install the netCDF-C library and utilities

        2. Install as usual and make sure to ``add netCDF`` to the system ``PATH`` for all users

    **Linux and MAC**

        The easiest way to install netCDF is via `Homebrew <https://brew.sh/>_`. Run the following command in the terminal:

        .. code-block:: bash

            brew install netcdf


To use netCDF we have to include the directories and link the library to the whole project in the ``CMakeLists.txt``.
With ``find_package(netCDF REQUIRED)`` CMake will find the existing path and use it as reference.

    .. code-block:: bash

        find_package(netCDF REQUIRED)
        include_directories(${netCDF_INCLUDE_DIRS})
        link_libraries(${netCDF_LIBRARIES})

    Now we can use netCDF files in out project.


2. tsunami_lab::io:NetCdf::write
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

With the NetCdf constructor we set up our netCDF file. First we define our required dimensions:

.. code-block:: cpp
    :emphasize-lines: 23-45

    // Header: NetCdf.h
    // File:   NetCdf.cpp
    // Test:   NetCdf.test.cpp

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
                            "longitude", // name
                            m_nx,        // len
                            &m_dimXId ); // idp
        checkNcErr( l_err, "dimX" );

        l_err = nc_def_dim( m_ncId,      // ncid
                            "latitude",  // name
                            m_ny,        // len
                            &m_dimYId ); // idp
        checkNcErr( l_err, "dimY" );

        m_dimIds[0] = m_dimTimeId;
        m_dimIds[1] = m_dimYId;
        m_dimIds[2] = m_dimXId;
        [ ... ]


Next, we declare our variables:

.. code-block:: cpp

    [ ... ]
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
    [ ... ]


Now we have defined the global attributes and the units for a number of variables:

.. code-block:: cpp

    :emphasize-lines: 7, 15, 22, 29

    [ ... ]
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
    [ ... ]


Finally, in accordance with convention, we calculate the latitude and longitude values by converting metres to degrees and write them to our netCDF file:

.. code-block:: cpp

        [ ... ]
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


The create file will now look like this (only example):

.. code-block:: bash

    netcdf WriteNetCDF.test {
    dimensions:
        time = UNLIMITED ; // (20 currently)
        longitude = 10 ;
        latitude = 10 ;
    variables:
        float longitude(longitude) ;
            longitude:units = "degrees_east" ;
        float latitude(latitude) ;
            latitude:units = "degrees_north" ;
        float time(time) ;
            time:units = "seconds" ;
        float totalHeight(time, latitude, longitude) ;
        float bathymetry(time, latitude, longitude) ;
        float momentumX(time, latitude, longitude) ;
        float momentumY(time, latitude, longitude) ;

    // global attributes:
            :Conventions = "COARDS" ;
    data:

     longitude = 0, 0.009981249, 0.0199625, 0.02994375, 0.03992499, 0.04990624,
        0.05988749, 0.06986874, 0.07984999, 0.08983123 ;

     latitude = 0, 0.01004857, 0.02009715, 0.03014572, 0.0401943, 0.05024287,
        0.06029145, 0.07034002, 0.0803886, 0.09043717 ;


Now we are implementing a function which allows to **write** the current time step to the created netCDF file.
The function gets the current simulation time and the following values of the cells: total height, bathymetry,
momentum in x direction and momentum in y direction. The check ``isReadMode`` ensures that we are using a netCDF
writer and not a reader (netCDF reader constructor is empty).

.. code-block:: cpp
    :emphasize-lines: 11, 25-65

    // Header: NetCdf.h
    // File:   NetCdf.cpp
    // Test:   NetCdf.test.cpp

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