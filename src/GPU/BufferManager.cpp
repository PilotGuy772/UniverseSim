//
// Created by Laeth English on 7/30/25.
//

#include "BufferManager.hpp"

#include <iostream>

#if defined(BX_PLATFORM_OSX)
#include <__ostream/basic_ostream.h>
#endif

#include "ComputeBridge.hpp"
#include "../Core/Application.hpp"
#include "../Core/ApplicationConfig.hpp"

void GPU::InitBuffers(uint32_t size) {
    // initialize all buffers with the given size
    BufferSize = size;

    VertexLayout = bgfx::VertexLayout();
    VertexLayout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord1, 1, bgfx::AttribType::Float)
        .end();
    VertexLayoutHandle = bgfx::createVertexLayout(VertexLayout);


    PositionsBuffer_Old = bgfx::createDynamicVertexBuffer(size, VertexLayout, BGFX_BUFFER_COMPUTE_READ | BGFX_BUFFER_COMPUTE_WRITE);
    PositionsBuffer_New = bgfx::createDynamicVertexBuffer(size, VertexLayout, BGFX_BUFFER_COMPUTE_READ | BGFX_BUFFER_COMPUTE_WRITE);
    VelocitiesBuffer_Old = bgfx::createDynamicVertexBuffer(size, VertexLayout, BGFX_BUFFER_COMPUTE_READ | BGFX_BUFFER_COMPUTE_WRITE);
    VelocitiesBuffer_New = bgfx::createDynamicVertexBuffer(size, VertexLayout, BGFX_BUFFER_COMPUTE_READ | BGFX_BUFFER_COMPUTE_WRITE);
    AccelerationsBuffer_Old = bgfx::createDynamicVertexBuffer(size, VertexLayout, BGFX_BUFFER_COMPUTE_READ | BGFX_BUFFER_COMPUTE_WRITE);
    AccelerationsBuffer_New = bgfx::createDynamicVertexBuffer(size, VertexLayout, BGFX_BUFFER_COMPUTE_READ | BGFX_BUFFER_COMPUTE_WRITE);
    BitmaskBuffer = bgfx::createDynamicVertexBuffer(size, VertexLayout, BGFX_BUFFER_COMPUTE_READ);


    // uniforms
    u_position = bgfx::createUniform("u_position", bgfx::UniformType::Vec4);
    u_velocity = bgfx::createUniform("u_velocity", bgfx::UniformType::Vec4);
    u_index = bgfx::createUniform("u_index", bgfx::UniformType::Vec4);
    u_numParticles = bgfx::createUniform("u_numParticles", bgfx::UniformType::Vec4);
    u_deltaTime = bgfx::createUniform("u_deltaTime", bgfx::UniformType::Vec4);

    // textures
    WritableSingleEntityTexture = bgfx::createTexture2D(3, 1, false, 1, bgfx::TextureFormat::RGBA32F,
                                                                 BGFX_TEXTURE_COMPUTE_WRITE);
    ReadbackSingleEntityTexture = bgfx::createTexture2D(3, 1, false, 1, bgfx::TextureFormat::RGBA32F,
                                                                BGFX_TEXTURE_READ_BACK | BGFX_TEXTURE_BLIT_DST);
}

void GPU::AddEntity(const uint32_t index, glm::vec4 position_mass, glm::vec4 velocity, uint32_t flags) {
    std::cout << "Adding entity to index " << index << std::endl;
    if (index >= BufferSize) {
        std::cerr << "Error: Index out of bounds for GPU buffers." << std::endl;
        return;
    }

    const bgfx::Memory* bitmask_mem = bgfx::copy(&flags, sizeof(uint32_t));
    bgfx::update(BitmaskBuffer, index, bitmask_mem);

    // use the add_entity shader to add the entity to the GPU buffers
    // arguments: 0. positions_old 1. velocities_old
    // uniforms: 0. vec4 position 1. vec4 velocity 2. uint32_t index
    bgfx::setBuffer(0, PositionsBuffer_Old, bgfx::Access::Write);
    bgfx::setBuffer(1, VelocitiesBuffer_Old, bgfx::Access::Write);

    bgfx::setUniform(u_position, &position_mass, 1);
    bgfx::setUniform(u_velocity, &velocity, 1);
    auto indexAsvec4 = glm::vec4{static_cast<float>(index), 0.0f, 0.0f, 0.0f};
    bgfx::setUniform(u_index, &indexAsvec4, 1);

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
    const uint32_t bitmask_startpos = index ;
    bgfx::update(BitmaskBuffer, bitmask_startpos, mem);
}

void GPU::ResizeBuffers() {
    uint32_t newSize = (BufferSize + Simulation::EntityQueue.size()) * 2;
    bgfx::DynamicVertexBufferHandle newFlagsBuffer = bgfx::createDynamicVertexBuffer(newSize, VertexLayout, BGFX_BUFFER_COMPUTE_READ);
    PositionsBuffer_New = bgfx::createDynamicVertexBuffer(newSize, VertexLayout, BGFX_BUFFER_COMPUTE_READ | BGFX_BUFFER_COMPUTE_WRITE);
    AccelerationsBuffer_New = bgfx::createDynamicVertexBuffer(newSize, VertexLayout, BGFX_BUFFER_COMPUTE_READ | BGFX_BUFFER_COMPUTE_WRITE);
    VelocitiesBuffer_New = bgfx::createDynamicVertexBuffer(newSize, VertexLayout, BGFX_BUFFER_COMPUTE_READ | BGFX_BUFFER_COMPUTE_WRITE);

    bgfx::setBuffer(0, PositionsBuffer_Old, bgfx::Access::Read);
    bgfx::setBuffer(1, VelocitiesBuffer_Old, bgfx::Access::Read);
    bgfx::setBuffer(2, AccelerationsBuffer_Old, bgfx::Access::Read);
    bgfx::setBuffer(3, BitmaskBuffer, bgfx::Access::Read);

    bgfx::setBuffer(4, PositionsBuffer_New, bgfx::Access::Write);
    bgfx::setBuffer(5, VelocitiesBuffer_New, bgfx::Access::Write);
    bgfx::setBuffer(6, AccelerationsBuffer_New, bgfx::Access::Write);
    bgfx::setBuffer(7, newFlagsBuffer, bgfx::Access::Write);

    bgfx::dispatch(Core::VIEW_ID_ENTITY_ADDER, computeBufferCopierProgram, BufferSize, 1, 1);
    bgfx::frame();

    // now swap buffers and recreate *_New buffers again
    std::swap(BitmaskBuffer, newFlagsBuffer);
    SwapBuffers();
    PositionsBuffer_New = bgfx::createDynamicVertexBuffer(newSize, VertexLayout, BGFX_BUFFER_COMPUTE_READ | BGFX_BUFFER_COMPUTE_WRITE);
    AccelerationsBuffer_New = bgfx::createDynamicVertexBuffer(newSize, VertexLayout, BGFX_BUFFER_COMPUTE_READ | BGFX_BUFFER_COMPUTE_WRITE);
    VelocitiesBuffer_New = bgfx::createDynamicVertexBuffer(newSize, VertexLayout, BGFX_BUFFER_COMPUTE_READ | BGFX_BUFFER_COMPUTE_WRITE);

    // fill available GPU indices with newly created space
    for (int i = BufferSize; i < newSize; i++) {
        Simulation::GpuIndices.push(i);
    }
    BufferSize = newSize;
}

void GPU::GetReadableTexture() {
    // blit writable texture to readback texture
    bgfx::blit(Core::VIEW_ID_COMPUTE_READBACK_SINGLE, ReadbackSingleEntityTexture, 0, 0, WritableSingleEntityTexture, 0, 0, 3, 1);
}

void GPU::ReadbackSingleEntity(uint32_t index) {
    // readback single arguments:
    // 0. flags 1. positions_old 2. velocities_old 3. accelerations_old 4. texture
    // uniforms:
    // 0. vec4 index
    if (index >= BufferSize) {
        std::cerr << "Error: Index out of bounds for GPU buffers." << std::endl;
        return;
    }

    // set buffers
    bgfx::setBuffer(0, BitmaskBuffer, bgfx::Access::Read);
    bgfx::setBuffer(1, PositionsBuffer_Old, bgfx::Access::Read);
    bgfx::setBuffer(2, VelocitiesBuffer_Old, bgfx::Access::Read);
    bgfx::setBuffer(3, AccelerationsBuffer_Old, bgfx::Access::Read);
    bgfx::setImage(4, WritableSingleEntityTexture, 0, bgfx::Access::Write, bgfx::TextureFormat::RGBA32F);
    auto indexAsFloat4 = glm::vec4{static_cast<float>(index), 0.0f, 0.0f, 0.0f};
    bgfx::setUniform(u_index, &indexAsFloat4, 1);

    // dispatch
    bgfx::dispatch(Core::VIEW_ID_COMPUTE_READBACK_SINGLE, computeReadbackSingleProgram,
                   1, 1, 1); // only one thread group needed
}

