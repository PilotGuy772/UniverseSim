#include <thread>

#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"
#include "Core/Application.hpp"
#include "glm/vec3.hpp"
#include "GPU/BufferManager.hpp"
#include "GPU/ComputeBridge.hpp"
//
// Created by Laeth English on 8/6/25.
//
TEST_CASE("Verify buffer resizing and entity C&D", "[SimLoop]") {
    constexpr uint32_t initialBufferSize = 16;
    constexpr uint32_t targetPreResize = 8;
    constexpr uint32_t targetAfterResize = 18;
    glm::vec3 entity1pos = glm::vec3{1.0f, 2.0f, 3.0f};
    glm::vec3 entity2pos = glm::vec3{4.0f, 5.0f, 6.0f};

    SECTION("Initialization") {
        REQUIRE(Core::Init() == 0);
        GPU::Initialize();
        GPU::InitBuffers(initialBufferSize);
        Simulation::InitializeEntities(initialBufferSize);
        REQUIRE(GPU::BufferSize == initialBufferSize);
        REQUIRE(Simulation::GpuIndices.size() == initialBufferSize);
    }

    SECTION("Adding entity within bounds") {

        int addResult;

        SECTION("Queue entity and add it") {
            Simulation::QueueNewEntity(entity1pos, 1.0f, glm::vec3(0.0f), 1 << 0);
            addResult = Simulation::AddNextEntityFromQueue();
            REQUIRE(addResult >= 0);
            bgfx::frame();
        }

        SECTION("Check entity in entity vector") {
            REQUIRE(Simulation::Entities[0].Handle == addResult);
        }

        SECTION("Get entity from buffers using handle") {
            GPU::ReadbackSingleEntity(Simulation::Entities[addResult].GpuIndex);
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

            REQUIRE(gpuData[0].x == Catch::Approx(entity1pos.x).epsilon(0.01f));
            REQUIRE(gpuData[0].y == Catch::Approx(entity1pos.y).epsilon(0.01f));
            REQUIRE(gpuData[0].z == Catch::Approx(entity1pos.z).epsilon(0.01f));
            REQUIRE(gpuData[0].w == Catch::Approx(1.0f).epsilon(0.01f));
        }

        SECTION("Fill buffers with entities") {
            // we have space for `initialBufferSize` entities, we currently have one
            // fill initialBufferSize - 1 entities, then try one more; last one should fail.
            for (int i = 0; i < initialBufferSize - 1; i++) {
                Simulation::QueueNewEntity(entity1pos, i * 1.0f, glm::vec3(0.0f), 1 << 0);
            }
            Simulation::QueueNewEntity(entity2pos, 1.0f, glm::vec3(0.0f), 1 << 0);
            for (int i = 0; i < initialBufferSize - 1; i++) {
                REQUIRE(Simulation::AddNextEntityFromQueue() >= 0);
                bgfx::frame();
            }

            // space is full, this should fail.
            REQUIRE(Simulation::AddNextEntityFromQueue() < 0);
        }

    }

    SECTION("Grow buffers and test adding entities") {
        SECTION("Resize buffers") {
            GPU::ResizeBuffers();
            REQUIRE(GPU::BufferSize > initialBufferSize);
        }

        int result;
        SECTION("Add new entity from queue") {
            result = Simulation::AddNextEntityFromQueue();
            bgfx::frame();
        }

        SECTION("Readback entity from data buffers") {
            GPU::ReadbackSingleEntity(Simulation::Entities[result].GpuIndex);
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

            REQUIRE(gpuData[0].x == Catch::Approx(entity2pos.x).epsilon(0.01f));
            REQUIRE(gpuData[0].y == Catch::Approx(entity2pos.y).epsilon(0.01f));
            REQUIRE(gpuData[0].z == Catch::Approx(entity2pos.z).epsilon(0.01f));
            REQUIRE(gpuData[0].w == Catch::Approx(1.0f).epsilon(0.01f));
        }
    }
}