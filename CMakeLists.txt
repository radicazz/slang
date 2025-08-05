cmake_minimum_required(VERSION 3.20)

project(csnake C)

# SDL has been added as a submodule, so we can include it directly.
add_subdirectory(external/SDL EXCLUDE_FROM_ALL)

# Add all the source files in the src directory.
file(GLOB SOURCES "src/*.c")
add_executable(csnake WIN32 ${SOURCES})

target_compile_features(csnake PRIVATE c_std_99)
target_link_libraries(csnake PRIVATE SDL3::SDL3)
target_include_directories(csnake PRIVATE external/SDL/include)

if (WIN32)
    # Copy the SDL3 shared library to the output directory after building.
    add_custom_command(
        TARGET csnake POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:csnake>
        VERBATIM
    )
endif()
