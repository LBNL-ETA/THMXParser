cmake_minimum_required(VERSION 3.15)

project( THMXParser VERSION 1.1.6 LANGUAGES CXX )
set(LIB_NAME ${PROJECT_NAME})

# Graceful local-sibling fallback: the `local` CMake preset sets
# FETCHCONTENT_SOURCE_DIR_<NAME> to ${sourceDir}/../<DepName> for each declared
# dependency. If a sibling working copy is missing, unset that variable here so
# FetchContent falls back to the declared remote instead of erroring out.
foreach(_lbnl_dep XMLPARSER GOOGLETEST)
    if(DEFINED FETCHCONTENT_SOURCE_DIR_${_lbnl_dep})
        if(NOT EXISTS "${FETCHCONTENT_SOURCE_DIR_${_lbnl_dep}}/CMakeLists.txt")
            message(STATUS
                "Local sibling for ${_lbnl_dep} not found at "
                "${FETCHCONTENT_SOURCE_DIR_${_lbnl_dep}}; falling back to remote.")
            unset(FETCHCONTENT_SOURCE_DIR_${_lbnl_dep} CACHE)
        endif()
    endif()
endforeach()

if(NOT "${CMAKE_CXX_STANDARD}")
	set(CMAKE_CXX_STANDARD 20)
endif()

if(NOT "${CMAKE_CXX_EXTENSIONS}")
	set(CMAKE_CXX_EXTENSIONS OFF)
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


