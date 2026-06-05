include_directories("${PROJECT_SOURCE_DIR}")

set(PROJECT_TEST_NAME ${LIB_NAME}-test)

if(DOWNLOAD_GTEST STREQUAL ON)
    if(MSVC)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif()

    if(NOT TARGET gtest)
        include(FetchContent)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.17.0
            GIT_SHALLOW TRUE
        )
        FetchContent_MakeAvailable(googletest)
    endif()
endif()

add_executable(${PROJECT_TEST_NAME}
        read_cs_03.unit.cpp
        read_cma_example.cpp
        main.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_TEST_NAME} gmock_main ${LIB_NAME})

target_compile_features(${PROJECT_TEST_NAME} PRIVATE cxx_std_17)

add_test(NAME ${PROJECT_TEST_NAME}-runner COMMAND ${PROJECT_TEST_NAME} "${CMAKE_CURRENT_LIST_DIR}")
