cmake_minimum_required(VERSION 3.15)

Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.4.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)

enable_testing()

add_executable(mspc_test
        channel_tests.cpp
        )

target_compile_options(mspc_test PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
        )

target_include_directories(mspc_test PUBLIC
        ${CMAKE_SOURCE_DIR}
        )

target_link_libraries(mspc_test
        PRIVATE
        Catch2::Catch2WithMain
        mpsc)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

catch_discover_tests(mspc_test
        channel_tests.cpp
        )
