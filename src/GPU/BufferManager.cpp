//
// Created by Laeth English on 7/30/25.
//

#include "BufferManager.hpp"

#include <iostream>
#include <__ostream/basic_ostream.h>

void GPU::InitBuffers(uint32_t size) {
    // initialize all buffers with the given size
    BufferSize = size;

    PositionsBuffer_Old = bgfx::createDynamicVertexBuffer(size, bgfx::VertexLayout(), BGFX_BUFFER_COMPUTE_READ | BGFX_BUFFER_COMPUTE_WRITE);
    PositionsBuffer_New = bgfx::createDynamicVertexBuffer(size, bgfx::VertexLayout(), BGFX_BUFFER_COMPUTE_READ | BGFX_BUFFER_COMPUTE_WRITE);
    VelocitiesBuffer_Old = bgfx::createDynamicVertexBuffer(size, bgfx::VertexLayout(), BGFX_BUFFER_COMPUTE_READ | BGFX_BUFFER_COMPUTE_WRITE);
    VelocitiesBuffer_New = bgfx::createDynamicVertexBuffer(size, bgfx::VertexLayout(), BGFX_BUFFER_COMPUTE_READ | BGFX_BUFFER_COMPUTE_WRITE);
    AccelerationsBuffer_Old = bgfx::createDynamicVertexBuffer(size, bgfx::VertexLayout(), BGFX_BUFFER_COMPUTE_READ | BGFX_BUFFER_COMPUTE_WRITE);
    AccelerationsBuffer_New = bgfx::createDynamicVertexBuffer(size, bgfx::VertexLayout(), BGFX_BUFFER_COMPUTE_READ | BGFX_BUFFER_COMPUTE_WRITE);
    BitmaskBuffer = bgfx::createDynamicVertexBuffer(size, bgfx::VertexLayout(), BGFX_BUFFER_COMPUTE_READ | BGFX_BUFFER_COMPUTE_WRITE);
}

void GPU::AddEntity(const uint32_t index, simd::packed::float4 position_mass, simd::packed::float4 velocity, uint32_t flags) {
    if (index >= BufferSize) {
        std::cerr << "Error: Index out of bounds for GPU buffers." << std::endl;
        return;
    }

    const bgfx::Memory* position_mem = bgfx::copy(&position_mass, sizeof(simd::packed::float4));
    const uint32_t float4_pos = index * sizeof(simd::packed::float4);
    bgfx::update(PositionsBuffer_Old, float4_pos, position_mem);
    bgfx::update(PositionsBuffer_New, float4_pos, position_mem);
    const bgfx::Memory* velocity_mem = bgfx::copy(&velocity, sizeof(simd::packed::float4));
    bgfx::update(VelocitiesBuffer_New, float4_pos, velocity_mem);
    bgfx::update(VelocitiesBuffer_Old, float4_pos, velocity_mem);
    const bgfx::Memory* bitmask_mem = bgfx::copy(&flags, sizeof(uint32_t));
    const uint32_t bitmask_startpos = index * sizeof(uint32_t);
    bgfx::update(BitmaskBuffer, bitmask_startpos, bitmask_mem);
}

void GPU::KillEntity(uint32_t index) {
    // just zero the bitmask
    if (index >= BufferSize) {
        std::cerr << "Error: Index out of bounds for GPU buffers." << std::endl;
        return;
    }

    const uint32_t zero = 0;
    const bgfx::Memory* mem = bgfx::copy(&zero, sizeof(uint32_t));
    const uint32_t bitmask_startpos = index * sizeof(uint32_t);
    bgfx::update(BitmaskBuffer, bitmask_startpos, mem);
}

void GPU::ResizeBuffers() {
    // not implemented (yet) //
}

