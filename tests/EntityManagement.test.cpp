//
// Created by Laeth English on 7/31/25.
//
// Catch2 tests for BufferManager and ComputeBridge

#include <catch2/catch_all.hpp>
#include "GPU/BufferManager.hpp"
#include "GPU/ComputeBridge.hpp"
#include "Core/Application.hpp"
#include "Debug/MetalCapture.h"
#include <bgfx/bgfx.h>
#include <simd/packed.h>
#include <random>
#include <thread>

TEST_CASE("Add an entity within bounds", "[GPU]") {
    REQUIRE(Core::Init() == 0);
    uint32_t bufferSize = 8;
    GPU::InitBuffers(bufferSize);
    GPU::Initialize();
    //startMetalCapture("trace.gputrace");

    REQUIRE(bgfx::isValid(GPU::WritableSingleEntityTexture));
    REQUIRE(bgfx::isValid(GPU::ReadbackSingleEntityTexture));

    REQUIRE(GPU::BufferSize == bufferSize);

    simd::packed::float4 position_mass{1.0f, 2.0f, 3.0f, 4.0f};
    simd::packed::float4 velocity{0.5f, 0.5f, 0.5f, 0.0f};
    uint32_t index = 3;

    GPU::AddEntity(index, position_mass, velocity, 1);
    bgfx::frame();

    GPU::ReadbackSingleEntity(index);
    bgfx::frame();

    GPU::GetReadableTexture();
    bgfx::frame();

    std::this_thread::sleep_for(std::chrono::seconds(5));

    // check the result with the expected values
    auto* gpuData = static_cast<simd::packed::float4*>(malloc(3 * sizeof(simd::packed::float4)));
    uint32_t ready = bgfx::readTexture(GPU::ReadbackSingleEntityTexture, gpuData);

    while (bgfx::frame() < ready) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    //stopMetalCapture();

    // print full contents of returned data
    for (int i = 0; i < 3; i++) {
        std::cout << "Data[" << i << "]: " << gpuData[i].x << ", " << gpuData[i].y << ", " << gpuData[i].z << ", " << gpuData[i].w << std::endl;
    }



    REQUIRE(gpuData[0].x == Catch::Approx(position_mass.x).epsilon(0.01f));
    REQUIRE(gpuData[0].y == Catch::Approx(position_mass.y).epsilon(0.01f));
    REQUIRE(gpuData[0].z == Catch::Approx(position_mass.z).epsilon(0.01f));
    REQUIRE(gpuData[0].w == Catch::Approx(position_mass.w).epsilon(0.01f));
    REQUIRE(gpuData[1].x == Catch::Approx(velocity.x).epsilon(0.01f));
    REQUIRE(gpuData[1].y == Catch::Approx(velocity.y).epsilon(0.01f));
    REQUIRE(gpuData[1].z == Catch::Approx(velocity.z).epsilon(0.01f));
}

/*SCENARIO("GPU buffer initialization and entity management") {
    GIVEN("A buffer size of 8") {
        // NOTICE TO DEBUGGER!! THERE IS SOMETHING NONLINEAR HAPPENING! SOMETHING ASYNCHRONOUS IS CAUSING A PROBLEM!!!
        REQUIRE(Core::Init() == 0);
        uint32_t bufferSize = 8;
        GPU::InitBuffers(bufferSize);

        REQUIRE(GPU::BufferSize == bufferSize);

        WHEN("Adding an entity within bounds") {
            simd::packed::float4 position_mass{1.0f, 2.0f, 3.0f, 4.0f};
            simd::packed::float4 velocity{0.5f, 0.5f, 0.5f, 0.0f};
            uint32_t index = 3;
            REQUIRE_NOTHROW(GPU::AddEntity(index, position_mass, velocity, 1));

            std::cout << "Called AddEntity" << std::endl;

            bgfx::frame(); // Ensure GPU writes are flushed

            std::cout << "Called first frame" << std::endl;

            GPU::ReadbackSingleEntity(index);
            REQUIRE(bgfx::isValid(GPU::WritableSingleEntityTexture));
            std::cout << "Read back single entity" << std::endl;
            //bgfx::frame();
            GPU::GetReadableTexture();
            //bgfx::frame();

            REQUIRE(bgfx::isValid(GPU::ReadbackSingleEntityTexture));
        }

        WHEN("Adding an entity out of bounds") {
            simd::packed::float4 position_mass{1.0f, 2.0f, 3.0f, 4.0f};
            simd::packed::float4 velocity{0.5f, 0.5f, 0.5f, 0.0f};
            uint32_t index = bufferSize; // out of bounds
            GPU::AddEntity(index, position_mass, velocity, 1);

            bgfx::frame();

            GPU::ReadbackSingleEntity(index);
            bgfx::frame();

            REQUIRE_FALSE(bgfx::isValid(GPU::ReadbackSingleEntityTexture));
        }

        // WHEN("Killing an entity within bounds") {
        //     uint32_t index = 2;
        //     GPU::KillEntity(index);
        //
        //     bgfx::frame();
        //
        //     bgfx::TextureHandle tex = GPU::ReadbackSingleEntity(index);
        //     bgfx::frame();
        //     bgfx::TextureHandle readable = GPU::GetReadableTexture(tex);
        //     bgfx::frame();
        //
        //     REQUIRE(bgfx::isValid(readable));
        // }

        // WHEN("Killing an entity out of bounds") {
        //     uint32_t index = bufferSize; // out of bounds
        //     GPU::KillEntity(index);
        //
        //     bgfx::frame();
        //
        //     bgfx::TextureHandle tex = GPU::ReadbackSingleEntity(index);
        //     bgfx::frame();
        //
        //     REQUIRE_FALSE(bgfx::isValid(tex));
        // }
    }
}*/

/*SCENARIO("Compute shader dispatch and buffer swapping") {
    GIVEN("Initialized buffers and compute shaders with random entity data") {
        REQUIRE(Core::Init() == 0);
        uint32_t bufferSize = 16;
        GPU::InitBuffers(bufferSize);
        REQUIRE(GPU::Initialize() == 0);

        // Generate random entity data
        std::vector<simd::packed::float4> positions(bufferSize);
        std::vector<simd::packed::float4> velocities(bufferSize);
        std::vector<uint32_t> flags(bufferSize, 1);

        std::mt19937 rng(42);
        std::uniform_real_distribution<float> posDist(-100.0f, 100.0f);
        std::uniform_real_distribution<float> velDist(-10.0f, 10.0f);
        std::uniform_real_distribution<float> massDist(1.0f, 10.0f);

        for (uint32_t i = 0; i < bufferSize; ++i) {
            positions[i] = simd::packed::float4{posDist(rng), posDist(rng), posDist(rng), massDist(rng)};
            velocities[i] = simd::packed::float4{velDist(rng), velDist(rng), velDist(rng), 0.0f};
            GPU::AddEntity(i, positions[i], velocities[i], flags[i]);
        }

        // Dispatch shaders in correct order and swap buffers before frame
        float deltaTime = 0.016f;
        GPU::DispatchVerletPosition(deltaTime);
        GPU::DispatchGravity();
        GPU::DispatchVerletVelocity(deltaTime);
        GPU::SwapBuffers();

        bgfx::frame();

        WHEN("Reading back a single entity and comparing to CPU simulation") {
            uint32_t testIndex = bufferSize / 2;

            // Step 1: Compute initial acceleration using Newton's law of gravitation
            auto compute_acceleration = [&](uint32_t idx, const std::vector<simd::packed::float4>& pos) {
                simd::packed::float4 acc{0.0f, 0.0f, 0.0f, 0.0f};
                float G = 6.67430e-11f; // gravitational constant
                float mass_i = pos[idx].w;
                for (uint32_t j = 0; j < pos.size(); j++) {
                    if (j == idx) continue;
                    float mass_j = pos[j].w;
                    float dx = pos[j].x - pos[idx].x;
                    float dy = pos[j].y - pos[idx].y;
                    float dz = pos[j].z - pos[idx].z;
                    float distSqr = dx*dx + dy*dy + dz*dz + 1e-6f; // avoid div by zero
                    float dist = std::sqrt(distSqr);
                    float force = G * mass_j / distSqr;
                    acc.x += force * dx / dist;
                    acc.y += force * dy / dist;
                    acc.z += force * dz / dist;
                }
                return acc;
            };

            simd::packed::float4 pos = positions[testIndex];
            simd::packed::float4 vel = velocities[testIndex];
            float mass = pos.w;

            // Step 1: Compute old acceleration
            simd::packed::float4 accel_old = compute_acceleration(testIndex, positions);

            // Step 2: Update position
            simd::packed::float4 pos_new = {
                pos.x + vel.x * deltaTime + 0.5f * accel_old.x * deltaTime * deltaTime,
                pos.y + vel.y * deltaTime + 0.5f * accel_old.y * deltaTime * deltaTime,
                pos.z + vel.z * deltaTime + 0.5f * accel_old.z * deltaTime * deltaTime,
                mass
            };

            // Step 3: Compute new acceleration from updated positions
            std::vector<simd::packed::float4> positions_new = positions;
            positions_new[testIndex] = pos_new;
            simd::packed::float4 accel_new = compute_acceleration(testIndex, positions_new);

            // Step 4: Update velocity using average acceleration
            simd::packed::float4 vel_new = {
                vel.x + 0.5f * (accel_old.x + accel_new.x) * deltaTime,
                vel.y + 0.5f * (accel_old.y + accel_new.y) * deltaTime,
                vel.z + 0.5f * (accel_old.z + accel_new.z) * deltaTime,
                0.0f
            };

            // Read back from GPU
            GPU::ReadbackSingleEntity(testIndex);
            bgfx::frame();
            GPU::GetReadableTexture();
            bgfx::frame();

            REQUIRE(bgfx::isValid(GPU::ReadbackSingleEntityTexture));

            // Read texture data (assuming RGBA32F format for simplicity)
            simd::packed::float4 gpuData[3];
            bgfx::readTexture(GPU::ReadbackSingleEntityTexture, gpuData, sizeof(gpuData));

            // Compare position and velocity
            REQUIRE(gpuData[0].x == Catch::Approx(pos_new.x).epsilon(0.01f));
            REQUIRE(gpuData[0].y == Catch::Approx(pos_new.y).epsilon(0.01f));
            REQUIRE(gpuData[0].z == Catch::Approx(pos_new.z).epsilon(0.01f));
            REQUIRE(gpuData[1].x == Catch::Approx(vel_new.x).epsilon(0.01f));
            REQUIRE(gpuData[1].y == Catch::Approx(vel_new.y).epsilon(0.01f));
            REQUIRE(gpuData[1].z == Catch::Approx(vel_new.z).epsilon(0.01f));

        }
    }
}*/