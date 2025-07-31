//
// Created by Laeth English on 7/30/25.
//

#include "BufferManager.hpp"

#include <iostream>
#include <__ostream/basic_ostream.h>

#include "ComputeBridge.hpp"
#include "../Core/Application.hpp"
#include "../Core/ApplicationConfig.hpp"

void GPU::InitBuffers(uint32_t size) {
    // initialize all buffers with the given size
    BufferSize = size;

    bgfx::VertexLayout layout;
    layout.begin()
        .add(bgfx::Attrib::Position, 4, bgfx::AttribType::Float)
        .end();
    bgfx::VertexLayout bitmaskLayout;
    bitmaskLayout.begin()
        .add(bgfx::Attrib::TexCoord0, 1, bgfx::AttribType::Uint8)
        .end();

    PositionsBuffer_Old = bgfx::createDynamicVertexBuffer(size, layout, BGFX_BUFFER_COMPUTE_READ | BGFX_BUFFER_COMPUTE_WRITE);
    PositionsBuffer_New = bgfx::createDynamicVertexBuffer(size, layout, BGFX_BUFFER_COMPUTE_READ | BGFX_BUFFER_COMPUTE_WRITE);
    VelocitiesBuffer_Old = bgfx::createDynamicVertexBuffer(size, layout, BGFX_BUFFER_COMPUTE_READ | BGFX_BUFFER_COMPUTE_WRITE);
    VelocitiesBuffer_New = bgfx::createDynamicVertexBuffer(size, layout, BGFX_BUFFER_COMPUTE_READ | BGFX_BUFFER_COMPUTE_WRITE);
    AccelerationsBuffer_Old = bgfx::createDynamicVertexBuffer(size, layout, BGFX_BUFFER_COMPUTE_READ | BGFX_BUFFER_COMPUTE_WRITE);
    AccelerationsBuffer_New = bgfx::createDynamicVertexBuffer(size, layout, BGFX_BUFFER_COMPUTE_READ | BGFX_BUFFER_COMPUTE_WRITE);
    BitmaskBuffer = bgfx::createDynamicVertexBuffer(size, bitmaskLayout, BGFX_BUFFER_COMPUTE_READ);

    // uniforms
    u_position = bgfx::createUniform("u_position", bgfx::UniformType::Vec4);
    u_velocity = bgfx::createUniform("u_velocity", bgfx::UniformType::Vec4);
    u_index = bgfx::createUniform("u_index", bgfx::UniformType::Vec4);
    u_numParticles = bgfx::createUniform("u_numParticles", bgfx::UniformType::Vec4);
    u_deltaTime = bgfx::createUniform("u_deltaTime", bgfx::UniformType::Vec4);
}

void GPU::AddEntity(const uint32_t index, simd::packed::float4 position_mass, simd::packed::float4 velocity, uint32_t flags) {
    if (index >= BufferSize) {
        std::cerr << "Error: Index out of bounds for GPU buffers." << std::endl;
        return;
    }

    // const bgfx::Memory* position_mem = bgfx::copy(&position_mass, sizeof(simd::packed::float4));
    // const uint32_t float4_pos = index * sizeof(simd::packed::float4);
    // bgfx::update(PositionsBuffer_Old, float4_pos, position_mem);
    // const bgfx::Memory* velocity_mem = bgfx::copy(&velocity, sizeof(simd::packed::float4));
    // bgfx::update(VelocitiesBuffer_Old, float4_pos, velocity_mem);
    const bgfx::Memory* bitmask_mem = bgfx::copy(&flags, sizeof(uint32_t));
    const uint32_t bitmask_startpos = index * sizeof(uint32_t);
    bgfx::update(BitmaskBuffer, bitmask_startpos, bitmask_mem);

    // use the add_entity shader to add the entity to the GPU buffers
    // arguments: 0. positions_old 1. velocities_old
    // uniforms: 0. float4 position 1. float4 velocity 2. uint32_t index
    bgfx::setBuffer(0, PositionsBuffer_Old, bgfx::Access::Write);
    bgfx::setBuffer(1, VelocitiesBuffer_Old, bgfx::Access::Write);

    bgfx::setUniform(u_position, &position_mass, 1);
    bgfx::setUniform(u_velocity, &velocity, 1);
    auto indexAsFloat4 = simd::packed::float4{static_cast<float>(index), 0.0f, 0.0f, 0.0f};
    bgfx::setUniform(u_index, &indexAsFloat4, 1);

    // dispatch
    bgfx::dispatch(Core::VIEW_ID_ENTITY_ADDER, computeEntityAdderProgram,
                   1, 1, 1); // only one thread group needed
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

