// //
// // Created by Laeth English on 7/28/25.
// //
#include "ComputeBridge.hpp"
#include "BufferManager.hpp"
#include "../Core/ApplicationConfig.hpp"
#include <bx/platform.h>

// shaders
#if defined(__APPLE__)
#include <metal/compute_gravity.bin.h>
#include <metal/compute_verlet_position.bin.h>
#include <metal/compute_verlet_velocity.bin.h>
#include <metal/compute_entity_adder.bin.h>
#include <metal/compute_readback_single.bin.h>
#elif defined(__linux__)
#if defined(SHADER_TYPE_SPIRV)
#include <spirv/compute_gravity.bin.h>
#include <spirv/compute_verlet_position.bin.h>
#include <spirv/compute_verlet_velocity.bin.h>
#include <spirv/compute_entity_adder.bin.h>
#include <spirv/compute_readback_single.bin.h>
#elif defined(SHADER_TYPE_430)
#include <430/compute_gravity.bin.h>
#include <430/compute_verlet_position.bin.h>
#include <430/compute_verlet_velocity.bin.h>
#include <430/compute_entity_adder.bin.h>
#include <430/compute_readback_single.bin.h>
#endif
#endif

int GPU::Initialize() {
    bgfx::setViewName(Core::VIEW_ID_COMPUTE_GRAVITY, "computeGravity");
    bgfx::setViewClear(Core::VIEW_ID_COMPUTE_GRAVITY, BGFX_CLEAR_NONE);

    bgfx::setViewName(Core::VIEW_ID_COMPUTE_POSITION, "computeVerletPosition");
    bgfx::setViewClear(Core::VIEW_ID_COMPUTE_POSITION, BGFX_CLEAR_NONE);

    bgfx::setViewName(Core::VIEW_ID_COMPUTE_VELOCITY, "computeVerletVelocity");
    bgfx::setViewClear(Core::VIEW_ID_COMPUTE_VELOCITY, BGFX_CLEAR_NONE);

    auto createComputeProgram = [](const void* shaderData, size_t shaderSize, const char* errorMsg) {
        auto shader = bgfx::createShader(bgfx::makeRef(shaderData, shaderSize));
        auto program = bgfx::createProgram(shader);
        if (!bgfx::isValid(shader) || !bgfx::isValid(program)) {
            std::cerr << errorMsg << std::endl;
            return std::make_pair(bgfx::ShaderHandle(), bgfx::ProgramHandle()); // invalid handles
        }
        return std::make_pair(shader, program);
    };

    std::tie(computeVerletPositionShader, computeVerletPositionProgram) =
        createComputeProgram(compute_verlet_position, sizeof(compute_verlet_position), "Failed to create compute shader for Verlet Position.");
    if (!bgfx::isValid(computeVerletPositionShader) || !bgfx::isValid(computeVerletPositionProgram)) return -1;

    std::tie(computeVerletVelocityShader, computeVerletVelocityProgram) =
        createComputeProgram(compute_verlet_velocity, sizeof(compute_verlet_velocity), "Failed to create compute shader for Verlet Velocity.");
    if (!bgfx::isValid(computeVerletVelocityShader) || !bgfx::isValid(computeVerletVelocityProgram)) return -1;

    std::tie(computeGravityShader, computeGravityProgram) =
        createComputeProgram(compute_gravity, sizeof(compute_gravity), "Failed to create compute shader for Gravity.");
    if (!bgfx::isValid(computeGravityShader) || !bgfx::isValid(computeGravityProgram)) return -1;

    std::tie(computeEntityAdderHandle, computeEntityAdderProgram) =
        createComputeProgram(compute_entity_adder, sizeof(compute_entity_adder), "Failed to create compute shader for Entity Adder.");
    if (!bgfx::isValid(computeEntityAdderHandle) || !bgfx::isValid(computeEntityAdderProgram)) return -1;

    std::tie(computeReadbackSingleShader, computeReadbackSingleProgram) =
        createComputeProgram(compute_readback_single, sizeof(compute_readback_single), "Failed to create compute shader for Readback Single.");
    if (!bgfx::isValid(computeReadbackSingleShader) || !bgfx::isValid(computeReadbackSingleProgram)) return -1;

    return 0;
}


void GPU::DispatchGravity() {
    // dispatch gravity shader
    // gravity shader params:
    // 0. flags 1. positions_old 2. accelerations_new
    // uniform uint numParticles

    // set buffers
    bgfx::setBuffer(0, BitmaskBuffer, bgfx::Access::Read);
    bgfx::setBuffer(1, PositionsBuffer_Old, bgfx::Access::Read);
    bgfx::setBuffer(2, AccelerationsBuffer_New, bgfx::Access::ReadWrite);

    // dispatch
    bgfx::dispatch(Core::VIEW_ID_COMPUTE_GRAVITY,
        computeGravityProgram,
        ceil(BufferSize / 16), ceil(BufferSize / 16), 1);
}

void GPU::DispatchVerletPosition(float deltaTime) {
    // dispatch verlet position shader
    // verlet position shader params:
    // 0. flags 1. velocities_old 2. accelerations_old 3. positions_old 4. positions_new
    // uniform float deltaTime

    // set buffers
    bgfx::setBuffer(0, GPU::BitmaskBuffer, bgfx::Access::Read);
    bgfx::setBuffer(1, GPU::VelocitiesBuffer_Old, bgfx::Access::Read);
    bgfx::setBuffer(2, GPU::AccelerationsBuffer_Old, bgfx::Access::Read);
    bgfx::setBuffer(3, GPU::PositionsBuffer_Old, bgfx::Access::Read);
    bgfx::setBuffer(4, GPU::PositionsBuffer_New, bgfx::Access::Write);
    bgfx::setUniform(GPU::u_deltaTime, &BufferSize, 1);

    // dispatch
    bgfx::dispatch(Core::VIEW_ID_COMPUTE_POSITION,
                   computeVerletPositionProgram,
                   BufferSize / 64, 1, 1);
}

void GPU::DispatchVerletVelocity(float deltaTime) {
    // dispatch verlet velocity shader
    // verlet velocity shader params:
    // 0. flags 1. accelerations_old 2. velocities_old 3. velocities_new
    // uniform float deltaTime

    // set buffers
    bgfx::setBuffer(0, GPU::BitmaskBuffer, bgfx::Access::Read);
    bgfx::setBuffer(1, GPU::AccelerationsBuffer_Old, bgfx::Access::Read);
    bgfx::setBuffer(2, GPU::VelocitiesBuffer_Old, bgfx::Access::Read);
    bgfx::setBuffer(3, GPU::VelocitiesBuffer_New, bgfx::Access::Write);
    bgfx::setUniform(GPU::u_deltaTime, &deltaTime, 1);

    // dispatch
    bgfx::dispatch(Core::VIEW_ID_COMPUTE_VELOCITY,
                   computeVerletVelocityProgram,
                   BufferSize / 64, 1, 1);
}

void GPU::SwapBuffers() {
    std::swap(PositionsBuffer_Old, PositionsBuffer_New);
    std::swap(VelocitiesBuffer_Old, VelocitiesBuffer_New);
    std::swap(AccelerationsBuffer_Old, AccelerationsBuffer_New);
    // Accelerations_Old will be used for verlet calculations, but Accelerations_New must be zeroed out
    AccelerationsBuffer_New = bgfx::createDynamicVertexBuffer(BufferSize, VertexLayout, BGFX_BUFFER_COMPUTE_READ | BGFX_BUFFER_COMPUTE_WRITE);
}



