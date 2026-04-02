cmake_minimum_required(VERSION 3.16)

project(slang VERSION 0.1.6 LANGUAGES C)

enable_testing()

option(SLANG_ENABLE_WARNINGS "Enable compiler warnings for slang targets" ON)
option(SLANG_ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer for debug builds" OFF)

# Don't allow in-source builds.
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed. Please create a separate build directory.")
endif()

function(slang_apply_project_options target_name)
    target_compile_features(${target_name} PRIVATE c_std_99)

    if(SLANG_ENABLE_WARNINGS)
        if(MSVC)
            target_compile_options(${target_name} PRIVATE /W4 /permissive-)
        else()
            target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
        endif()
    endif()

    if(SLANG_ENABLE_SANITIZERS)
        if(MSVC)
            message(WARNING "SLANG_ENABLE_SANITIZERS is not supported with MSVC and will be ignored")
        elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
            target_compile_options(${target_name} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
            target_link_options(${target_name} PRIVATE -fsanitize=address,undefined)
        else()
            message(WARNING "SLANG_ENABLE_SANITIZERS only applies to Debug builds")
        endif()
    endif()
endfunction()

function(slang_configure_test test_name)
    if(SLANG_ENABLE_SANITIZERS AND NOT MSVC)
        set_tests_properties(${test_name} PROPERTIES ENVIRONMENT "ASAN_OPTIONS=detect_leaks=0")
    endif()
endfunction()

# Add the project's submodules.
add_subdirectory(external/SDL EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL_ttf EXCLUDE_FROM_ALL)

file(GLOB_RECURSE SLANG_SOURCES CONFIGURE_DEPENDS "src/*.c")

add_executable(slang WIN32 ${SLANG_SOURCES})

slang_apply_project_options(slang)
target_link_libraries(slang PRIVATE SDL3::SDL3 PRIVATE SDL3_ttf::SDL3_ttf)

add_executable(dynamic_array_tests
    tests/dynamic_array_tests.c
    src/utils/dynamic_array.c
)

target_include_directories(dynamic_array_tests PRIVATE src)
slang_apply_project_options(dynamic_array_tests)
target_link_libraries(dynamic_array_tests PRIVATE SDL3::SDL3)
add_test(NAME dynamic_array_tests COMMAND dynamic_array_tests)
slang_configure_test(dynamic_array_tests)

add_executable(vector_tests
    tests/vector_tests.c
    src/utils/vector.c
)

target_include_directories(vector_tests PRIVATE src)
slang_apply_project_options(vector_tests)
target_link_libraries(vector_tests PRIVATE SDL3::SDL3)
add_test(NAME vector_tests COMMAND vector_tests)
slang_configure_test(vector_tests)

# Copy assets folder over to build directory for game assets.
add_custom_command(
    TARGET slang POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:slang>/assets"
)

if(WIN32)
    add_custom_command(
        TARGET slang POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:slang>
        VERBATIM
    )

    add_custom_command(
        TARGET slang POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3_ttf::SDL3_ttf-shared> $<TARGET_FILE_DIR:slang>
        VERBATIM
    )
endif()
