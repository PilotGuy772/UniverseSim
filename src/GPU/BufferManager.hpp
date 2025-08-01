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
    inline bgfx::UniformHandle u_position;
    inline bgfx::UniformHandle u_velocity;
    inline bgfx::UniformHandle u_index;

    // textures for readback
    inline bgfx::TextureHandle WritableSingleEntityTexture;
    inline bgfx::TextureHandle ReadbackSingleEntityTexture;


    /**
     * @brief Initializes the GPU buffers for compute.
     * @param size The size of the buffers to allocate.
     */
    void InitBuffers(uint32_t size);

    /**
     * @brief Updates the given index with the contents of the entity. You must call `bgfx::frame()` after this WITHOUT
     * first dispatching any other shaders to ensure data is written safely.
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

    /**
     * @brief Read back a single entity's data from the GPU buffers.
     * @param index The GPU buffer index of the entity to read back.
     * @return A texture handle pointing to a texture containing the entity's data.
     *
     * @note The returned texture will NOT be readable. You must:
     * 1. Call this function.
     * 2. Call `bgfx::frame()` to ensure the data is written. The frame must not dispatch any other compute shaders.
     * 3. Call `GPU::GetReadableTexture()` to get a readable texture handle.
     * 4. Call `bgfx::frame()` again to ensure the texture is ready for reading. Other compute shaders can be dispatched
     * during this frame.
     * 5. Copy the data from the texture to a CPU-accessible buffer using `bgfx::readTexture()`.
     */
    void ReadbackSingleEntity(uint32_t index);

    /**
     * @brief Get a readable texture handle from a writable texture handle. Uses bgfx::blit() internally.
     * @param texture A texture that is writable by the GPU.
     * @return A texture that is readable by the CPU, but not writable by the GPU. You must call `bgfx::frame()` after
     * after this function and before attempting to read the texture data.
     */
    void GetReadableTexture();

}

#endif //BUFFERMANAGER_HPP
