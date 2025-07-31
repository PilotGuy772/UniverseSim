cmake_minimum_required(VERSION 3.31)
project(UniverseSim)


###!!#####################!!###
## !! ATTENTION COMPILERS !! ##
###!!#####################!!###
# Below, see variables you MUST SET APPROPRIATELY for your compiler and environment.

# Platform you're compiling FOR. This must be one of "osx", "linux", or "windows". Windows is not supported officially, but *should* work.
set(PLATFORM osx)
# Shader profile types to compile for. These must be selected for your system and graphics API. Use any selection of "glsl", "essl", "spirv", "metal".
# Which shader type is used in actual execution is determined by the graphics API and platform.
set(SHADER_PLATFORMS metal)



set(CMAKE_CXX_STANDARD 20)

##################
## DEPENDENCIES ##
##################
include(FetchContent)

FetchContent_Declare(
        bgfx.cmake
        GIT_REPOSITORY https://github.com/bkaradzic/bgfx.cmake
        GIT_TAG e8a92ffbbe4801e5597cf37978d7a6c715052133
)

FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.2.18
)

FetchContent_MakeAvailable(SDL3)
FetchContent_MakeAvailable(bgfx.cmake)

########################
## SHADER COMPILATION ##
########################

set(SHADERC_EXECUTABLE ${bgfx.cmake_BINARY_DIR}/cmake/bgfx/shaderc)
set(SHADER_SOURCE_DIR ${CMAKE_SOURCE_DIR}/shaders)
set(SHADER_OUTPUT_BASE_DIR ${CMAKE_BINARY_DIR}/include/generated/shaders)
set(VARYING_DEF ${SHADER_SOURCE_DIR}/varying.def.sc)

set(COMPUTE_SHADERS
    ${SHADER_SOURCE_DIR}/compute/gravity.sc
    ${SHADER_SOURCE_DIR}/compute/verlet_position.sc
    ${SHADER_SOURCE_DIR}/compute/verlet_velocity.sc
)

set(VERTEX_SHADERS
    # none yet
)

set(FRAGMENT_SHADERS
    # none yet
)

# Platforms to generate headers for

message(STATUS "ShaderC Executable: ${SHADERC_EXECUTABLE}")
function(compile_shader_list shader_type shader_files out_list_var)
    message(STATUS "Compiling shaders of type: ${shader_type}")

    foreach(shader IN LISTS ${shader_files})
        get_filename_component(shader_name ${shader} NAME)
        string(REPLACE ".sc" "" base_name ${shader_name})

        foreach(profile IN LISTS SHADER_PLATFORMS)
            set(output_dir ${SHADER_OUTPUT_BASE_DIR}/${profile})
            file(MAKE_DIRECTORY ${output_dir})

            set(output_file ${output_dir}/${shader_type}_${base_name}.bin.h)

            message(STATUS "Setting up shader compilation: ${shader} -> ${output_file} (platform: ${profile})")
            add_custom_command(
                    OUTPUT ${output_file}
                    COMMAND ${SHADERC_EXECUTABLE}
                    -f ${shader}
                    -o ${output_file}
                    --type ${shader_type}
                    --platform ${PLATFORM}
                    --profile ${profile}
                    --varyingdef ${VARYING_DEF}
                    -i ${bgfx.cmake_SOURCE_DIR}/bgfx/src
                    --bin2c
                    DEPENDS ${shader} ${VARYING_DEF}
                    COMMENT "Compiling ${shader_name} for ${profile} (${shader_type})"
                    VERBATIM
            )

            list(APPEND compiled_headers ${output_file})
        endforeach()
    endforeach()
    set(${out_list_var} ${compiled_headers} PARENT_SCOPE)
endfunction()

compile_shader_list("compute"  COMPUTE_SHADERS COMPUTE_HEADERS)
compile_shader_list("vertex"   VERTEX_SHADERS   VERTEX_HEADERS)
compile_shader_list("fragment" FRAGMENT_SHADERS FRAGMENT_HEADERS)

add_custom_target(compile_shaders ALL DEPENDS
        ${COMPUTE_HEADERS}
        ${VERTEX_HEADERS}
        ${FRAGMENT_HEADERS}
)

###########################
## PROJECT CONFIGURATION ##
###########################

add_library(UniverseSim_lib
        src/Core/Application.cpp
        src/Core/Application.hpp
        src/Core/ApplicationConfig.hpp
        src/GPU/ComputeBridge.cpp
        src/GPU/ComputeBridge.hpp
        ${COMPUTE_SHADERS}
        ${VERTEX_SHADERS}
        ${FRAGMENT_SHADERS}
        src/Simulation/EntityManager.cpp
        src/Simulation/EntityManager.hpp
        src/GPU/BufferManager.cpp
        src/GPU/BufferManager.hpp
)

add_executable(UniverseSim src/main.cpp)
add_dependencies(UniverseSim compile_shaders)

##########################
## LINKING AND INCLUDES ##
##########################

target_include_directories(UniverseSim_lib PUBLIC
        ${SDL2_SOURCE_DIR}/include
        ${bgfx.cmake_SOURCE_DIR}/bgfx/include
        ${bgfx.cmake_SOURCE_DIR}/bx/include
        ${bgfx.cmake_SOURCE_DIR}/bimg/include
        ${SHADER_OUTPUT_BASE_DIR}
)

target_link_libraries(UniverseSim_lib PUBLIC SDL3::SDL3 bgfx bimg bx)
target_link_libraries(UniverseSim PRIVATE UniverseSim_lib)

###########################
## TESTING CONFIGURATION ##
###########################

add_subdirectory(tests)
