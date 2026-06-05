cmake_minimum_required(VERSION 3.15)

project( THMXParser VERSION 1.1.7 LANGUAGES CXX )
set(LIB_NAME ${PROJECT_NAME})

# Local development mode. The `local` preset sets LBNL_LOCAL_SIBLINGS=ON (a cache
# variable, so it cascades into FetchContent sub-builds). When it is on, prefer a
# sibling working copy (../<Name>) of each of THIS project's direct dependencies,
# so the whole dependency graph can be built from local disk during development.
# A dependency whose sibling is not checked out falls back to its declared remote,
# independently. Propagation: a sibling dependency that also honors this flag wires
# up its own siblings in turn. When the flag is off (e.g. the `default` preset),
# nothing here runs and every dependency comes from its remote.
# Note: each pair is "<folder>=<FETCHCONTENT var suffix>"; they differ for XMLParser
# (folder) vs XMLPARSER (its xmlParser FetchContent_Declare name, uppercased).
if(LBNL_LOCAL_SIBLINGS)
    foreach(_sibling "XMLParser=XMLPARSER" "googletest=GOOGLETEST")
        string(REPLACE "=" ";" _sibling "${_sibling}")
        list(GET _sibling 0 _sibling_dir)
        list(GET _sibling 1 _sibling_var)
        if(NOT FETCHCONTENT_SOURCE_DIR_${_sibling_var}
           AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../${_sibling_dir}/CMakeLists.txt")
            set(FETCHCONTENT_SOURCE_DIR_${_sibling_var}
                "${CMAKE_CURRENT_SOURCE_DIR}/../${_sibling_dir}")
            message(STATUS "[local] ${_sibling_dir} -> ${FETCHCONTENT_SOURCE_DIR_${_sibling_var}}")
        endif()
    endforeach()
endif()

if(NOT "${CMAKE_CXX_STANDARD}")
	set(CMAKE_CXX_STANDARD 20)
endif()

if(NOT "${CMAKE_CXX_EXTENSIONS}")
	set(CMAKE_CXX_EXTENSIONS OFF)
endif()

# Enable MSVC parallel (multi-processor) compilation for all targets and
# dependencies defined below. Placed before add_subdirectory/FetchContent so
# every later target inherits it (add_compile_options only affects later targets).
if(MSVC)
    add_compile_options(/MP)
endif()

include(GNUInstallDirs)
if(NOT "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
endif()

if(NOT "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
endif()

if(NOT "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
endif()

# Check to see if this project is included in another via add_subdirectoru
# If it is, do not download GTest since it will create errors in the client
# if the client also uses GTest
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(DOWNLOAD_GTEST ON)
else()
    set(DOWNLOAD_GTEST OFF)
endif()

include(FetchContent)

set(xmlParser_Branch "v1.0.5")

if(NOT TARGET xmlParser)
    FetchContent_Declare(
        xmlParser
        GIT_REPOSITORY https://github.com/LBNL-ETA/XMLParser.git
        GIT_TAG ${xmlParser_Branch}
    )

    FetchContent_MakeAvailable(xmlParser)
endif()

add_subdirectory( src )

target_include_directories(${LIB_NAME}
    PUBLIC 
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

Option(BUILD_THMX_Parser_Tests "Build tests for thmx file parsing." ON)

if(BUILD_THMX_Parser_Tests)
	enable_testing()
	add_subdirectory( test )

	# Set the path to the test directory
    set(TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test")
    target_compile_definitions(THMXParser-test PRIVATE TEST_DATA_DIR="${TEST_DATA_DIR}")
endif()


