cmake_minimum_required(VERSION 3.16)

project(csnake C)

# Don't allow in-source builds.
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed. Please create a separate build directory.")
endif()

add_subdirectory(external/SDL EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL_image EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL_ttf EXCLUDE_FROM_ALL)

# Add all the source files in the src directory.
file(GLOB CSNAKE_SOURCES "src/*.c")
add_executable(csnake WIN32 ${CSNAKE_SOURCES})

target_compile_features(csnake PRIVATE c_std_99)
target_link_libraries(csnake PRIVATE SDL3::SDL3 PRIVATE SDL3::SDL3_image PRIVATE SDL3::SDL3_ttf)

if (WIN32)
    # Copy the SDL3 shared library to the output directory after building.
    add_custom_command(
        TARGET csnake POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:csnake>
        VERBATIM
    )
endif()
