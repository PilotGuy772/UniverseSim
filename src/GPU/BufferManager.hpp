//
// Created by Laeth English on 7/30/25.
//

#ifndef BUFFERMANAGER_HPP
#define BUFFERMANAGER_HPP
#include <bgfx/bgfx.h>
#include <simd/packed.h>

#include "../Simulation/EntityManager.hpp"

// handles buffers for GPU compute
namespace GPU {
    inline bgfx::DynamicVertexBufferHandle PositionsBuffer_Old;
    inline bgfx::DynamicVertexBufferHandle PositionsBuffer_New;
    inline bgfx::DynamicVertexBufferHandle VelocitiesBuffer_Old;
    inline bgfx::DynamicVertexBufferHandle VelocitiesBuffer_New;
    inline bgfx::DynamicVertexBufferHandle AccelerationsBuffer_Old;
    inline bgfx::DynamicVertexBufferHandle AccelerationsBuffer_New;
    inline bgfx::DynamicVertexBufferHandle BitmaskBuffer;
    inline uint32_t BufferSize = 0; // size of the GPU buffers

    // single-value buffers for delta-t and numParticles
    inline bgfx::UniformHandle u_deltaTime;
    inline bgfx::UniformHandle u_numParticles;

    /**
     * @brief Initializes the GPU buffers for compute.
     * @param size The size of the buffers to allocate.
     */
    void InitBuffers(uint32_t size);

    /**
     * @brief Updates the given index with the contents of the entity.
     * @param index The index position to fill with the new entity data.
     * @param position_mass The position and mass of the entity, in a vec4 format.
     * @param velocity The velocity of the entity, in a vec4 format. The w component is unused.
     * @param flags Optional flags bitmask; defaults to 1 (alive).
     */
    void AddEntity(uint32_t index, simd::packed::float4 position_mass,
                   simd::packed::float4 velocity, uint32_t flags = 1);

    /**
     * @brief Zeros the target entity's bitmask, thereby marking it as dead and skipping it in all future computations.
     * @param index The GPU buffer index of the entity to kill.
     */
    void KillEntity(uint32_t index);

    /**
     * @brief Doubles the size of all buffers.
     * @warning This is an extremely computationally intensive operation and will pause execution for multiple frames, because a special
     * compute shader must be used to copy the data from the old buffers to the new ones. This should only be used in moderation.
     */
    void ResizeBuffers();

}

#endif //BUFFERMANAGER_HPP
