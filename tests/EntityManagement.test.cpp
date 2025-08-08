//
// Created by Laeth English on 7/31/25.
//
// Catch2 tests for BufferManager and ComputeBridge

#include <catch2/catch_all.hpp>
#include "GPU/BufferManager.hpp"
#include "GPU/ComputeBridge.hpp"
#include "Core/Application.hpp"
#include <bgfx/bgfx.h>
#include <glm/vec4.hpp>
#include <random>
#include <thread>

TEST_CASE("Create, Read, and Kill entities within bounds.", "[GPU]") {
    uint32_t index = 3;
    glm::vec4 position_mass{1.0f, 2.0f, 3.0f, 4.0f};
    glm::vec4 velocity{0.5f, 0.5f, 0.5f, 0.0f};

    SECTION("Initialization") {
        REQUIRE(Core::Init() == 0);
        uint32_t bufferSize = 8;
        GPU::InitBuffers(bufferSize);
        GPU::Initialize();
        //startMetalCapture("trace.gputrace");

        REQUIRE(bgfx::isValid(GPU::WritableSingleEntityTexture));
        REQUIRE(bgfx::isValid(GPU::ReadbackSingleEntityTexture));

        REQUIRE(GPU::BufferSize == bufferSize);
    }

    SECTION("Creating entity") {
        uint32_t flags = 1 << 0;
        std::cout << "Alive flag present? " << (flags & 0x01) << std::endl;
        GPU::AddEntity(index, position_mass, velocity, flags);
        bgfx::frame();
    }

    SECTION("Reading entity") {
        GPU::ReadbackSingleEntity(index);
        bgfx::frame();

        GPU::GetReadableTexture();
        bgfx::frame();

        // check the result with the expected values
        auto* gpuData = static_cast<glm::vec4*>(malloc(3 * sizeof(glm::vec4)));
        uint32_t ready = bgfx::readTexture(GPU::ReadbackSingleEntityTexture, gpuData);

        while (bgfx::frame() < ready) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

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

        // verify 2nd and 3rd W components are 1.0, alive
        REQUIRE(gpuData[2].w == Catch::Approx(1.0).epsilon((0.01f)));
    }

    SECTION("Kill entity") {
        GPU::KillEntity(index);
        bgfx::frame();
    }

    SECTION("Readback entity again") {
        GPU::ReadbackSingleEntity(index);
        bgfx::frame();

        GPU::GetReadableTexture();
        bgfx::frame();

        auto* gpuData = static_cast<glm::vec4*>(malloc(3 * sizeof(glm::vec4)));
        uint32_t ready = bgfx::readTexture(GPU::ReadbackSingleEntityTexture, gpuData);
        while (bgfx::frame() < ready) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        for (int i = 0; i < 3; i++) {
            std::cout << "Data[" << i << "]: " << gpuData[i].x << ", " << gpuData[i].y << ", " << gpuData[i].z << ", " << gpuData[i].w << std::endl;
        }

        // check for 0.0 in w component of 2nd and 3rd vec4s
        REQUIRE(gpuData[1].w == Catch::Approx(0.0).epsilon(0.01f));
        REQUIRE(gpuData[2].w == Catch::Approx(0.0).epsilon(0.01f));
    }
}
