cmake_minimum_required(VERSION 3.16)

project(arcade C)

# Don't allow in-source builds.
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed. Please create a separate build directory.")
endif()

# Add the project's submodules.
add_subdirectory(external/SDL EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL_image EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL_ttf EXCLUDE_FROM_ALL)

file(GLOB ARCADE_SOURCES "src/*.c")

add_executable(arcade WIN32 ${ARCADE_SOURCES})

target_compile_features(arcade PRIVATE c_std_99)
target_link_libraries(arcade PRIVATE SDL3::SDL3 PRIVATE SDL3_image::SDL3_image PRIVATE SDL3_ttf::SDL3_ttf)

if (WIN32)
    add_custom_command(
        TARGET arcade POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:arcade>
        VERBATIM
    )

    add_custom_command(
        TARGET arcade POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3_image::SDL3_image-shared> $<TARGET_FILE_DIR:arcade>
        VERBATIM
    )

    add_custom_command(
        TARGET arcade POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3_ttf::SDL3_ttf-shared> $<TARGET_FILE_DIR:arcade>
        VERBATIM
    )
endif()
