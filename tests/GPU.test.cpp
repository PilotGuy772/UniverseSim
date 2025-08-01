//
// Created by Laeth English on 7/29/25.
//
#include <catch2/catch_all.hpp>
#include <GPU/ComputeBridge.hpp>

#include "Core/Application.hpp"
#include "GPU/BufferManager.hpp"


// TEST_CASE("Compute shader dispatches without errors", "[GPU]") {
//     // this just tests the init and dispatch functions for blatant errors
//     REQUIRE(Core::Init() == 0);
//     REQUIRE(GPU::Initialize() == 0); // Ensure initialization is successful
//
//     // init buffers
//     REQUIRE_NOTHROW(GPU::InitBuffers(64)); // Initialize buffers with a size of 64
//
//     // create 16 random entities-- position + mass, velocity
//     // use add function to add them to the GPU buffers
//     std::vector<simd::packed::float4> positions(16);
//     std::vector<simd::packed::float4> velocities(16);
//     // fill with random float4
//     for (uint32_t i = 0; i < 16; ++i) {
//         positions[i] = simd::packed::float4{static_cast<float>(rand() % 100), static_cast<float>(rand() % 100), static_cast<float>(rand() % 100), static_cast<float>(rand() % 10)};
//         velocities[i] = simd::packed::float4{static_cast<float>(rand() % 10), static_cast<float>(rand() % 10), static_cast<float>(rand() % 10), 0.0f};
//         REQUIRE_NOTHROW(GPU::AddEntity(i, positions[i], velocities[i])); // Add each entity to the GPU buffers
//         REQUIRE_NOTHROW(bgfx::frame());
//     }
//     std::vector<uint32_t> flags(16, 1); // All entities are alive
//
//     REQUIRE_NOTHROW(GPU::DispatchVerletPosition(0.016f)); // Dispatch position compute shader
//     REQUIRE_NOTHROW(GPU::DispatchVerletVelocity(0.016f)); // Dispatch velocity compute shader
//     REQUIRE_NOTHROW(GPU::DispatchGravity()); // Dispatch gravity compute shader
//
//     REQUIRE_NOTHROW(bgfx::frame());
// }