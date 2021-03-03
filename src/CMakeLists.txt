set( CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON )

add_library( ${LIB_NAME} 
		thmxParser.hpp
		thmxParser.cpp)

target_link_libraries(${LIB_NAME} PUBLIC xmlParser)

if(NOT "${fPIC}")
  set(fPIC ON)
endif()

if(fPIC)
  set_property(TARGET ${LIB_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()


if (CMAKE_COMPILER_IS_GNUCC)
    target_compile_options(${LIB_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)    
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${LIB_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif()
if (MSVC)
    target_compile_options(${LIB_NAME} PRIVATE /W4 /WX)
	set_source_files_properties(thmxParser.cpp PROPERTIES COMPILE_FLAGS /wd4996) # Disable MSVC warning that sscanf may be unsafe
endif()


target_compile_features(${LIB_NAME} PRIVATE cxx_std_17)
