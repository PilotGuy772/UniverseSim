// //
// // Created by Laeth English on 7/28/25.
// //
#include "ComputeBridge.hpp"
#include "BufferManager.hpp"
#include "../Core/ApplicationConfig.hpp"

// shaders
#if defined(BX_PLATFORM_OSX)
#include <metal/compute_gravity.bin.h>
#include <metal/compute_verlet_position.bin.h>
#include <metal/compute_verlet_velocity.bin.h>
#elif defined(BX_PLATFORM_LINUX)
#include <glsl/compute_gravity.bin.h>
#include <glsl/compute_verlet_position.bin.h>
#include <glsl/compute_verlet_velocity.bin.h>
#endif

int GPU::Initialize() {
    bgfx::setViewName(Core::VIEW_ID_COMPUTE_GRAVITY, "computeGravity");
    bgfx::setViewClear(Core::VIEW_ID_COMPUTE_GRAVITY, BGFX_CLEAR_NONE);

    bgfx::setViewName(Core::VIEW_ID_COMPUTE_POSITION, "computeVerletPosition");
    bgfx::setViewClear(Core::VIEW_ID_COMPUTE_POSITION, BGFX_CLEAR_NONE);

    bgfx::setViewName(Core::VIEW_ID_COMPUTE_VELOCITY, "computeVerletVelocity");
    bgfx::setViewClear(Core::VIEW_ID_COMPUTE_VELOCITY, BGFX_CLEAR_NONE);

    computeVerletPositionShader = bgfx::createShader(bgfx::makeRef(compute_verlet_position, sizeof(compute_verlet_position)));
    computeVerletPositionProgram = bgfx::createProgram(computeVerletPositionShader);
    if (!bgfx::isValid(computeVerletPositionShader) || !bgfx::isValid(computeVerletPositionProgram)) {
        std::cerr << "Failed to create compute shader for Verlet Position." << std::endl;
        return -1;
    }
    computeVerletVelocityShader = bgfx::createShader(bgfx::makeRef(compute_verlet_velocity, sizeof(compute_verlet_velocity)));
    computeVerletVelocityProgram = bgfx::createProgram(computeVerletVelocityShader);
    if (!bgfx::isValid(computeVerletVelocityShader) || !bgfx::isValid(computeVerletVelocityProgram)) {
        std::cerr << "Failed to create compute shader for Verlet Velocity." << std::endl;
        return -1;
    }
    computeGravityShader = bgfx::createShader(bgfx::makeRef(compute_gravity, sizeof(compute_gravity)));
    computeGravityProgram = bgfx::createProgram(computeGravityShader);
    if (!bgfx::isValid(computeGravityShader) || !bgfx::isValid(computeGravityProgram)) {
        std::cerr << "Failed to create compute shader for Gravity." << std::endl;
        return -1;
    }

    return 0;
}


void GPU::DispatchGravity() {
    // dispatch gravity shader
    // gravity shader params:
    // 0. flags 1. positions_old 2. accelerations_new
    // uniform uint numParticles

    // set buffers
    bgfx::setBuffer(0, GPU::BitmaskBuffer, bgfx::Access::Read);
    bgfx::setBuffer(1, GPU::PositionsBuffer_Old, bgfx::Access::Read);
    bgfx::setBuffer(2, GPU::AccelerationsBuffer_New, bgfx::Access::Write);
    bgfx::setUniform(GPU::u_numParticles, &BufferSize, 1);

    // dispatch
    bgfx::dispatch(Core::VIEW_ID_COMPUTE_GRAVITY,
        computeGravityProgram,
        BufferSize / 64, 1, 1);
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
}



