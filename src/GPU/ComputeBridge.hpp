//
// Created by Laeth English on 7/28/25.
//

#ifndef COMPUTEBRIDGE_HPP
#define COMPUTEBRIDGE_HPP

#include <bgfx/bgfx.h>
#include <bx/bx.h>
#include <bx/math.h>
#include <iostream>
#include <vector>
#include <cstring>

namespace GPU {
    inline bgfx::ShaderHandle computeVerletPositionShader;
    inline bgfx::ShaderHandle computeVerletVelocityShader;
    inline bgfx::ShaderHandle computeGravityShader;
    inline bgfx::ShaderHandle computeEntityAdderHandle;
    inline bgfx::ShaderHandle computeReadbackSingleShader;

    inline bgfx::ProgramHandle computeVerletPositionProgram;
    inline bgfx::ProgramHandle computeVerletVelocityProgram;
    inline bgfx::ProgramHandle computeGravityProgram;
    inline bgfx::ProgramHandle computeEntityAdderProgram;
    inline bgfx::ProgramHandle computeReadbackSingleProgram;

    /**
     * @brief Initialize the compute shaders and assign their handles.
     * @return Zero on success, non-zero on failure.
     */
    int Initialize();

    /**
     * @brief Dispatches the compute shader to calculate new positions.
     */
    void DispatchVerletPosition(float deltaTime);
    /**
     * @brief Dispatches the compute shader to calculate new velocities.
     */
    void DispatchVerletVelocity(float deltaTime);
    /**
     * @brief Dispatches the compute shader to calculate new accelerations.
     */
    void DispatchGravity();

    /**
     * @brief Swaps old and new buffers. Should be run once each frame.
     */
    void SwapBuffers();
}

#endif //COMPUTEBRIDGE_HPP
