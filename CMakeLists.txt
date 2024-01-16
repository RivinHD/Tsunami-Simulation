cmake_minimum_required(VERSION 3.10.0)
project(TsunamiSimulation VERSION 0.1.0 LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(IS_MULTI_CONFIG)
    message(NOTICE "Using multi-config generator. Compile with: cmake --build . --config [Debug|Release] --target <target>")
else()
    message(NOTICE "Using single-config generator. Generate with: cmake .. -DCMAKE_BUILD_TYPE=[Debug|Release]")
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release")
        message(WARNING "No Build type is set. Using Release!")
    endif()
endif()

message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

# find packages
find_package(NetCDF REQUIRED)
link_libraries(NetCDF::NetCDF)

# Options
option(ENABLE_TESTMODE "Adds the Test defintion to any build")
if(ENABLE_TESTMODE)
    add_compile_definitions(TSUNAMI_SIMULATION_TEST)
    message(STATUS "Starting in Test Mode")
endif()

option(DISABLE_IO "Disable the io of the simulation")
if(DISABLE_IO)
    add_compile_definitions(TSUNAMI_SIMULATION_DISABLE_IO)
    message(STATUS "Disabling IO")
endif()

option(REPORT "Generate a report if enabled")
message("Compiler: ${CMAKE_CXX_COMPILER_ID}")
if(REPORT)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
        if(WIN32)
            add_compile_options(/Qopt-report=5)
        else()
            add_compile_options(-qopt-report=5)
        endif()
        message(STATUS "Activeted Report")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options(-fopt-info-optimized=opt_gnu.optrpt)
        message(STATUS "Activeted Report")
    endif()
endif()

# Setup compile Flags
if(MSVC)
    message(FATAL_ERROR "This project cannot be compiled with MSVC due to https://developercommunity.visualstudio.com/t/using-c17-new-stdalign-val-tn-syntax-results-in-er/528320")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    add_compile_options(-Wall -Wextra -pthread -qopenmp)
    set(CMAKE_CXX_FLAGS_RELEASE "-O2" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0" CACHE STRING "" FORCE)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wall -Wextra -Wpedantic -pthread -fopenmp)
    set(CMAKE_CXX_FLAGS_RELEASE "-O2" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0" CACHE STRING "" FORCE)
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_VERBOSE_MAKEFILE on)

# Add source files
set(PROJECT_SOURCE_DIR "src")
set(PROJECT_INCLUDE_DIR "include")

set(SRC_FILES
        solvers/FWave
        solvers/Roe
        setups/MiddleStates1d
        setups/DamBreak1d
        setups/ShockShock1d
        setups/RareRare1d
        setups/SubcriticalFlow1d
        setups/SupercriticalFlow1d
        setups/TsunamiEvent1d
        setups/CircularDamBreak2d
        setups/ArtificialTsunami2d
        setups/TsunamiEvent2d
        patches/WavePropagation1d
        patches/WavePropagation2d
        io/NetCdf
        io/Csv
        amr/AMRCoreWavePropagation2d
)
set(SRC_FILES_IO
        setups/CheckPoint
        io/Stations
)
set(SRC_FILES_NO_TEST

)
set(TEST_FILES ${PROJECT_SOURCE_DIR}/tests.cpp)
set(MAIN_FILE ${PROJECT_SOURCE_DIR}/main.cpp)
set(MIDDLE_STATE_FILE ${PROJECT_SOURCE_DIR}/test_middle_states.cpp)

# Add the Resources. Added resources can be accessed relatively as if they were in the project directory.
set(PROJECT_RESOURCE_DIR "resources")
set(RESOURCE_FILES
        middle_states.csv
        middle_states.test.csv
        bathy_profile.csv
        config.json
        config.test.json
        ReadNetCDF.test.nc
        WriteNetCDF_S.test.txt
        WriteNetCDF_M.test.txt
        artificialtsunami_displ_1000.nc
        artificialtsunami_bathymetry_1000.nc
)

# Create SOURCE_FILES
foreach(file ${SRC_FILES})
    list(APPEND SOURCE_FILES
            ${PROJECT_SOURCE_DIR}/${file}.cpp
            ${PROJECT_INCLUDE_DIR}/${file}.h
    )
    list(APPEND TEST_FILES
            ${PROJECT_SOURCE_DIR}/${file}.test.cpp
    )
endforeach()
if(NOT DISABLE_IO)
    foreach(file ${SRC_FILES_IO})
        list(APPEND SOURCE_FILES
                ${PROJECT_SOURCE_DIR}/${file}.cpp
                ${PROJECT_INCLUDE_DIR}/${file}.h
        )
        list(APPEND TEST_FILES
                ${PROJECT_SOURCE_DIR}/${file}.test.cpp
        )
    endforeach()
endif(NOT DISABLE_IO)
foreach(file ${SRC_FILES_NO_TEST})
    list(APPEND SOURCE_FILES
            ${PROJECT_SOURCE_DIR}/${file}.cpp
            ${PROJECT_INCLUDE_DIR}/${file}.h
    )
endforeach()

# Copy the RESOURCE_FILES
foreach(file ${RESOURCE_FILES})
    configure_file(${PROJECT_RESOURCE_DIR}/${file} ${PROJECT_RESOURCE_DIR}/${file} COPYONLY)
endforeach()

# Set up such that your IDE organizes the files correctly
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})

###################
##### Targets #####
###################

# Setup Testing
add_executable(test ${SOURCE_FILES})
target_sources(test PRIVATE ${TEST_FILES})
target_compile_definitions(test PRIVATE TSUNAMI_SIMULATION_TEST)

# Setup Testing + Sanitizer
add_executable(sanitize_test ${SOURCE_FILES})
target_sources(sanitize_test PRIVATE ${TEST_FILES})
target_compile_definitions(sanitize_test PRIVATE TSUNAMI_SIMULATION_TEST)

# Setup Testing for middle states
add_executable(test_middle_states ${SOURCE_FILES})
target_sources(test_middle_states PRIVATE ${MIDDLE_STATE_FILE})

# Setup Simulation
add_executable(simulation ${SOURCE_FILES})
target_sources(simulation PRIVATE ${MAIN_FILE})

# Setup Simulation + Sanitizer
add_executable(sanitize ${SOURCE_FILES})
target_sources(sanitize PRIVATE ${MAIN_FILE})

################################
##### Including submodules #####
################################

# Add Catch2
add_subdirectory(submodules/Catch2)
target_link_libraries(test PRIVATE Catch2::Catch2)
target_link_libraries(sanitize_test PRIVATE Catch2::Catch2)

# Add nlohmann
if(NOT DISABLE_IO)
    # Linking the json libary
    add_subdirectory(submodules/json)
    target_link_libraries(test PRIVATE nlohmann_json::nlohmann_json)
    target_link_libraries(sanitize_test PRIVATE nlohmann_json::nlohmann_json)
    target_link_libraries(test_middle_states PRIVATE nlohmann_json::nlohmann_json)
    target_link_libraries(simulation PRIVATE nlohmann_json::nlohmann_json)
    target_link_libraries(sanitize PRIVATE nlohmann_json::nlohmann_json)
endif(NOT DISABLE_IO)

# Add AMReX (https://amrex-codes.github.io/amrex/docs_html/BuildingAMReX.html#customization-options)
set(AMReX_SPACEDIM 2) # Dimension of AMReX build
set(AMReX_MPI NO) # Build with MPI support
set(AMReX_OMP YES) # Build with OpenMP support
set(AMReX_PRECISION SINGLE) # Set the precision of reals
set(AMReX_PARTICLES NO) # Build particle classes
set(AMReX_INSTALL NO) # Generate Install Targets
add_subdirectory(submodules/amrex)
target_link_libraries(test PRIVATE AMReX::amrex_2d)
target_link_libraries(sanitize_test PRIVATE AMReX::amrex_2d)
target_link_libraries(test_middle_states PRIVATE AMReX::amrex_2d)
target_link_libraries(simulation PRIVATE AMReX::amrex_2d)
target_link_libraries(sanitize PRIVATE AMReX::amrex_2d)

# Add mpi
# find_package(MPI REQUIRED)
# target_link_libraries(AMReX::amrex INTERFACE ${MPI_LIBRARIES})


# Init sanitizer
set_target_properties(sanitize
        PROPERTIES
        COMPILE_FLAGS "-g -fsanitize=float-divide-by-zero -fsanitize=bounds -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer"
        LINK_FLAGS "-g -fsanitize=address -fsanitize=undefined"
)
set_target_properties(sanitize_test
        PROPERTIES
        COMPILE_FLAGS "-g -fsanitize=float-divide-by-zero -fsanitize=bounds -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer"
        LINK_FLAGS "-g -fsanitize=address -fsanitize=undefined"
)

if(CMAKE_COMPILER_IS_GNUCC AND WIN32)
    message(AUTHOR_WARNING "MingW (aka g++ on Windows) has no sanitizers :/")
endif()
if(MSVC)
    message(AUTHOR_WARNING "Test with Catch2 does not work because of linking error :(")
endif()


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
