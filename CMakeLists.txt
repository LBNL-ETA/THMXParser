cmake_minimum_required(VERSION 3.11)

project( THMXParser VERSION 1.1.0 LANGUAGES CXX )
set(LIB_NAME ${PROJECT_NAME})

if(NOT "${CMAKE_CXX_STANDARD}")
	set(CMAKE_CXX_STANDARD 14)
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

if(NOT TARGET xmlParser)
    FetchContent_Declare(
        xmlParser
        GIT_REPOSITORY https://github.com/LBNL-ETA/XMLParser.git
        GIT_TAG "v1.0.1"
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


