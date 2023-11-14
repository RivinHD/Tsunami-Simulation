cmake_minimum_required(VERSION 3.1.0)
project(TsunamiSimulation VERSION 0.1.0 LANGUAGES C CXX)

message("####################################")
message("### Tsunami Lab                  ###")
message("###                              ###")
message("### https://scalable.uni-jena.de ###")
message("####################################")

# Setup compile Flags
if(MSVC)
    set(CMAKE_CXX_FLAGS "/Wall /W3 /WL /EHsc ")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2")
    set(CMAKE_CXX_FLAGS_DEBUG "/Od")
else()
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
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
    patches/WavePropagation1d
    patches/WavePropagation2d
    io/Csv
    io/Stations
)
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
    bathy_profile.csv
    config.json
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

add_subdirectory(submodules/Catch2)
include_directories(SYSTEM submodules/Catch2/single_include)

# Setup Testing
add_executable(test ${SOURCE_FILES})
target_link_libraries(test PRIVATE Catch2::Catch2)
target_sources(test PRIVATE ${TEST_FILES})

# Setup Testing + Sanitizer
add_executable(sanitize_test ${SOURCE_FILES})
target_link_libraries(sanitize_test PRIVATE Catch2::Catch2)
target_sources(sanitize_test PRIVATE ${TEST_FILES})

# Setup Testing for middle states
add_executable(test_middle_states ${SOURCE_FILES})
target_sources(test_middle_states PRIVATE ${MIDDLE_STATE_FILE})

# Setup Build
add_executable(simulation ${SOURCE_FILES})
target_sources(simulation PRIVATE ${MAIN_FILE})

# Setup Build + Sanitizer
add_executable(sanitize ${SOURCE_FILES})
target_sources(sanitize PRIVATE ${MAIN_FILE})

# Init sanitizer
if (MSVC)
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
