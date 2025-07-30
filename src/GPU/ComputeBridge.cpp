// //
// // Created by Laeth English on 7/28/25.
// //
#include "ComputeBridge.hpp"
#include <metal/compute_simple_compute.bin.h>
#include "../Core/ApplicationConfig.hpp"

void GPU::TestComputeShader(const std::vector<uint32_t> &input, uint32_t** result) {
    std::cout << "[Compute] Creating compute shader..." << std::endl;
    const bgfx::Memory* mem = bgfx::makeRef(compute_simple_compute, sizeof(compute_simple_compute));
    bgfx::ShaderHandle computeShader = bgfx::createShader(mem);
    bgfx::ProgramHandle program = bgfx::createProgram(computeShader, true);

    std::cout << "[Compute] Creating dynamic index buffer..." << std::endl;
    bgfx::DynamicIndexBufferHandle buffer = bgfx::createDynamicIndexBuffer(64,
        BGFX_BUFFER_INDEX32 | BGFX_BUFFER_COMPUTE_READ);
    bgfx::update(
        buffer,
        0,
        bgfx::copy(input.data(), input.size() * sizeof(uint32_t)));

    std::cout << "[Compute] Creating texture for read back..." << std::endl;
    bgfx::TextureHandle texture = bgfx::createTexture2D(64, 1, false, 1, bgfx::TextureFormat::RGBA8,
        BGFX_TEXTURE_COMPUTE_WRITE);
    bgfx::TextureHandle readbacktex = bgfx::createTexture2D(64, 1, false, 1, bgfx::TextureFormat::RGBA8,
        BGFX_TEXTURE_READ_BACK | BGFX_TEXTURE_BLIT_DST);

    std::cout << "[Compute] Creating uniform sampler..." << std::endl;
    bgfx::UniformHandle sampler = bgfx::createUniform("u_texture", bgfx::UniformType::Sampler);
    bgfx::setImage(1, texture, 0, bgfx::Access::Write, bgfx::TextureFormat::RGBA8);

    std::cout << "[Compute] Setting buffer..." << std::endl;
    bgfx::setBuffer(0, buffer, bgfx::Access::ReadWrite);

    std::cout << "[Compute] Setting view and clearing..." << std::endl;
    bgfx::setViewName(Core::VIEW_ID_COMPUTE_GRAVITY, "computePass");
    bgfx::setViewClear(Core::VIEW_ID_COMPUTE_GRAVITY, 0, BGFX_CLEAR_NONE);

    std::cout << "[Compute] Dispatching compute shader..." << std::endl;
    uint32_t threads = 64;
    bgfx::dispatch(Core::VIEW_ID_COMPUTE_GRAVITY, program, threads, 1, 1);

    std::cout << "[Compute] Advancing frame..." << std::endl;
    bgfx::frame();

    bgfx::blit(Core::VIEW_ID_COMPUTE_GRAVITY, readbacktex, 0, 0, texture, 0, 0, 64, 1);

    std::cout << "[Compute] Reading back texture..." << std::endl;
    auto* data = static_cast<uint32_t*>(malloc(64 * sizeof(uint32_t)));
    bgfx::readTexture(readbacktex, data);
    result = &data;
}

