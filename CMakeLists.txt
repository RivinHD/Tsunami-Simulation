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
    add_compile_definitions(TSUNAMI_SIMULATION_DISABLE_IO)
    message(STATUS "Starting in Test Mode")
endif()

option(DISABLE_IO "Disable the io of the simulation")

if(DISABLE_IO)
    add_compile_definitions(TSUNAMI_SIMULATION_DISABLE_IO)
    message(STATUS "Disabling IO")
endif()

option(DISABLE_STATIONS "Disable the io of the stations but keep the simulation io")

if(DISABLE_STATIONS)
    add_compile_definitions(TSUNAMI_SIMULATION_DISABLE_STATION_IO)
    message(STATUS "Disabling Station IO")
endif()

option(REPORT "Generate a report if enabled")
message("Compiler: ${CMAKE_CXX_COMPILER_ID}")

if(REPORT)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
        if(WIN32)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Qopt-report=5")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -qopt-report=5")
        endif()

        message(STATUS "Activeted Report")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopt-info-optimized=opt_gnu.optrpt")
        message(STATUS "Activeted Report")
    endif()
endif()

# Setup compile Flags
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall /W3 /WL /EHsc")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_DEBUG "/Od" CACHE STRING "" FORCE)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pthread")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0" CACHE STRING "" FORCE)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror -pthread")
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
)


if(NOT DISABLE_IO)
    set(SRC_FILES ${SRC_FILES}
        setups/CheckPoint
    )
endif()
if(NOT DISABLE_STATIONS)
    set(SRC_FILES ${SRC_FILES}
        setups/CheckPoint
    )
endif()

set(SRC_FILES_NO_TEST

)
set(SRC_FILES_HEADER_ONLY
    io/ArgSetup
)
set(SRC_FILES_HEADER_ONLY_NO_TEST

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

foreach(file ${SRC_FILES_NO_TEST})
    list(APPEND SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/${file}.cpp
        ${PROJECT_INCLUDE_DIR}/${file}.h
    )
endforeach()

foreach(file ${SRC_FILES_HEADER_ONLY})
    list(APPEND SOURCE_FILES
        ${PROJECT_INCLUDE_DIR}/${file}.h
    )
    list(APPEND TEST_FILES
        ${PROJECT_INCLUDE_DIR}/${file}.test.h
    )
endforeach()

foreach(file ${SRC_FILES_HEADER_ONLY_NO_TEST})
    list(APPEND SOURCE_FILES
        ${PROJECT_INCLUDE_DIR}/${file}.h
    )
endforeach()

# Copy the RESOURCE_FILES
foreach(file ${RESOURCE_FILES})
    configure_file(${PROJECT_RESOURCE_DIR}/${file} ${PROJECT_RESOURCE_DIR}/${file} COPYONLY)
endforeach()

# Set up such that your IDE organizes the files correctly
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})

# Include embedded Libararies
# Catch2
add_subdirectory(submodules/Catch2)
# nlohmann_json
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")
set(JSON_MultipleHeaders OFF CACHE INTERNAL "")
add_subdirectory(submodules/json)

# Setup Testing
add_executable(test ${SOURCE_FILES})
target_link_libraries(test PRIVATE Catch2::Catch2)
target_link_libraries(test PRIVATE nlohmann_json::nlohmann_json)
target_sources(test PRIVATE ${TEST_FILES})
target_compile_definitions(test PRIVATE TSUNAMI_SIMULATION_TEST)

# Setup Testing + Sanitizer
add_executable(sanitize_test ${SOURCE_FILES})
target_link_libraries(sanitize_test PRIVATE Catch2::Catch2)
target_link_libraries(sanitize_test PRIVATE nlohmann_json::nlohmann_json)
target_sources(sanitize_test PRIVATE ${TEST_FILES})
target_compile_definitions(sanitize_test PRIVATE TSUNAMI_SIMULATION_TEST)

# Setup Testing for middle states
add_executable(test_middle_states ${SOURCE_FILES})
target_sources(test_middle_states PRIVATE ${MIDDLE_STATE_FILE})
target_link_libraries(test_middle_states PRIVATE nlohmann_json::nlohmann_json)

# Setup Simulation
add_executable(simulation ${SOURCE_FILES})
target_sources(simulation PRIVATE ${MAIN_FILE})
target_link_libraries(simulation PRIVATE nlohmann_json::nlohmann_json)

# Setup Simulation + Sanitizer
add_executable(sanitize ${SOURCE_FILES})
target_sources(sanitize PRIVATE ${MAIN_FILE})
target_link_libraries(simulation PRIVATE nlohmann_json::nlohmann_json)

# Init sanitizer
if(MSVC)
    # MSVC only has address sanitizer
    set_target_properties(sanitize
        PROPERTIES
        COMPILE_FLAGS "/fsanitize=address /Zi"
    )
    set_target_properties(sanitize_test
        PROPERTIES
        COMPILE_FLAGS "/fsanitize=address /Zi"
    )
else()
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
endif()

if(CMAKE_COMPILER_IS_GNUCC AND WIN32)
    message(AUTHOR_WARNING "MingW (aka g++ on Windows) has no sanitizers :/")
endif()

if(MSVC)
    message(AUTHOR_WARNING "Test with Catch2 does not work because of linking error :(")
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
