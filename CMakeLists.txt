cmake_minimum_required(VERSION 3.16)

project(slang C)

# Don't allow in-source builds.
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed. Please create a separate build directory.")
endif()

# Add the project's submodules.
add_subdirectory(external/SDL EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL_ttf EXCLUDE_FROM_ALL)

file(GLOB_RECURSE SLANG_SOURCES "src/*.c")

add_executable(slang WIN32 ${SLANG_SOURCES})

target_compile_features(slang PRIVATE c_std_99)
target_link_libraries(slang PRIVATE SDL3::SDL3 PRIVATE SDL3_ttf::SDL3_ttf)

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
