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
    AccelerationsBuffer = bgfx::createDynamicVertexBuffer(size, bgfx::VertexLayout(), BGFX_BUFFER_COMPUTE_READ | BGFX_BUFFER_COMPUTE_WRITE);
    BitmaskBuffer = bgfx::createDynamicVertexBuffer(size, bgfx::VertexLayout(), BGFX_BUFFER_COMPUTE_READ | BGFX_BUFFER_COMPUTE_WRITE);
}

void GPU::AddEntity(const uint32_t index, simd::packed::float4 position_mass, simd::packed::float4 velocity, uint32_t flags) {}

void GPU::KillEntity(uint32_t index) {}

void GPU::ResizeBuffers() {}

