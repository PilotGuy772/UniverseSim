//
// Created by Laeth English on 8/4/25.
//
#include <random>
#include <thread>
#include <string>
#include <catch2/catch_all.hpp>
#include <GPU/BufferManager.hpp>
#include <GPU/ComputeBridge.hpp>
#include <Core/Application.hpp>
#include <glm/common.hpp>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>

TEST_CASE("Gravity simulation", "[Simulation]") {
    // simulate only the gravity, not integration

    REQUIRE(Core::Init() == 0);
    uint32_t bufferSize = 64;
    GPU::InitBuffers(bufferSize);
    REQUIRE(GPU::Initialize() == 0);

    // create random entities
    std::vector<glm::vec4> positions(bufferSize);
    std::vector<glm::vec4> velocities(bufferSize);

    for (int i = 0; i < bufferSize; i++) {
        positions[i] = glm::vec4{static_cast<float>(rand() % 1000), static_cast<float>(rand() % 1000), static_cast<float>(rand() % 1000), static_cast<float>(rand() % 1000000)};
        velocities[i] = glm::vec4{static_cast<float>(rand() % 10), static_cast<float>(rand() % 10), static_cast<float>(rand() % 10), 0.0f};
        GPU::AddEntity(i, positions[i], velocities[i], 1 << 0);
        bgfx::frame();
    }

    // run simulation-- only one frame; we're just calculating accelerations for the current positions
    GPU::DispatchGravity();
    bgfx::frame();
    GPU::SwapBuffers();

    // simulate on CPU side
    std::vector accelerations(bufferSize, glm::vec4(0.0f));
    const float G = 6.67430e-11f; // Gravitational constant
    for (int i = 0; i < bufferSize; i++) {
        for (int j = 0; j < bufferSize; j++) {
            if (i != j) {
                // F = G * m1 * m2 / r^2
                // a = F / m1
                float dx = positions[j].x - positions[i].x;
                float dy = positions[j].y - positions[i].y;
                float dz = positions[j].z - positions[i].z;
                float rSquared = dx * dx + dy * dy + dz * dz;

                if (rSquared > 0.0001f) {
                    float force = G * (positions[i].w * positions[j].w) / rSquared; // m1 * m2 / r^2
                    float distance = sqrt(rSquared);
                    accelerations[i].x += force * dx / distance / positions[i].w; // a = F / m1
                    accelerations[i].y += force * dy / distance / positions[i].w;
                    accelerations[i].z += force * dz / distance / positions[i].w;
                }

            }
        }
    }

    // readback data
    uint32_t readFrom = bufferSize / 2;
    GPU::ReadbackSingleEntity(readFrom);
    bgfx::frame();
    GPU::GetReadableTexture();
    bgfx::frame();

    // read texture
    auto* gpuData = static_cast<glm::vec4*>(malloc(3 * sizeof(glm::vec4)));
    uint32_t ready = bgfx::readTexture(GPU::ReadbackSingleEntityTexture, gpuData);
    while (bgfx::frame() < ready) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // print expected data
    std::cout << "Expected Acceleration for Entity " << readFrom << " (CPU side math): "
              << "X: " << accelerations[readFrom].x << ", "
              << "Y: " << accelerations[readFrom].y << ", "
              << "Z: " << accelerations[readFrom].z
              << std::endl;

    std::cout << "Actual Acceleration for Entity " << readFrom << " (GPU side math): "
              << "X: " << gpuData[2].x << ", "
              << "Y: " << gpuData[2].y << ", "
              << "Z: " << gpuData[2].z
              << std::endl;

    // verify results are similar
    REQUIRE(gpuData[2].x == Catch::Approx(accelerations[readFrom].x).epsilon(0.01f));
    REQUIRE(gpuData[2].y == Catch::Approx(accelerations[readFrom].y).epsilon(0.01f));
    REQUIRE(gpuData[2].z == Catch::Approx(accelerations[readFrom].z).epsilon(0.01f));

    free(gpuData);
}

TEST_CASE("Full simulation for five frames.", "[Simulation]") {
    // Error thresholds (configurable constants)
    const float MAX_POSITION_ERROR_PERCENT = 1.0f; // 1% maximum error for positions
    const float MAX_VELOCITY_ERROR_PERCENT = 1.0f; // 1% maximum error for velocities

    // simulate the full physics simulation for five frames
    REQUIRE(Core::Init() == 0);
    uint32_t bufferSize = 64;
    GPU::InitBuffers(bufferSize);
    REQUIRE(GPU::Initialize() == 0);

    uint32_t frames = 60;

    // create random entities
    std::vector<glm::vec4> positions(bufferSize);
    std::vector<glm::vec4> velocities(bufferSize);

    for (int i = 0; i < bufferSize; i++) {
        positions[i] = glm::vec4{static_cast<float>(rand() % 1000), static_cast<float>(rand() % 1000), static_cast<float>(rand() % 1000), static_cast<float>(rand() % 1000000)};
        velocities[i] = glm::vec4{static_cast<float>(rand() % 10), static_cast<float>(rand() % 10), static_cast<float>(rand() % 10), 0.0f};
        GPU::AddEntity(i, positions[i], velocities[i], 1 << 0);
        bgfx::frame();
    }

    // run simulation for five frames
    // using a constant deltaTime-- in the real sim, this will be controlled and possibly variable.
    float deltaTime = 0.016f; // 60 FPS
    for (int frame = 0; frame < frames; frame++) {
        GPU::DispatchVerletPosition(deltaTime);
        GPU::DispatchGravity();
        GPU::DispatchVerletVelocity(deltaTime);
        bgfx::frame();
        GPU::SwapBuffers();
    }

    // now, run the sim on CPU side for comparison
    const float G = 6.67430e-11f; // Gravitational constant
    std::vector accelerations(bufferSize, glm::vec4(0.0f));

    for (int f = 0; f < frames; f++) {
        // verlet positions
        for (int i = 0; i < bufferSize; i++) {
            positions[i] += velocities[i] * deltaTime + 0.5f * accelerations[i] * (deltaTime * deltaTime);
        }

        // gravity
        for (int i = 0; i < bufferSize; i++) {
            accelerations[i] = glm::vec4(0.0f);
            for (int j = 0; j < bufferSize; j++) {
                if (i != j) {
                    float dx = positions[j].x - positions[i].x;
                    float dy = positions[j].y - positions[i].y;
                    float dz = positions[j].z - positions[i].z;
                    float rSquared = dx * dx + dy * dy + dz * dz;

                    if (rSquared > 0.0001f) {
                        float force = G * (positions[i].w * positions[j].w) / rSquared; // m1 * m2 / r^2
                        float distance = sqrt(rSquared);
                        accelerations[i].x += force * dx / distance / positions[i].w; // a = F / m1
                        accelerations[i].y += force * dy / distance / positions[i].w;
                        accelerations[i].z += force * dz / distance / positions[i].w;
                    }
                }
            }
        }

        // verlet velocities
        for (int i = 0; i < bufferSize; i++) {
            velocities[i] += accelerations[i] * deltaTime;
        }
    }

    // read back all entities and calculate errors
    std::vector<glm::vec4> errorsPos(bufferSize);
    std::vector<glm::vec4> errorsVel(bufferSize);
    auto *gpuData = static_cast<glm::vec4 *>(malloc(3 * sizeof(glm::vec4)));

    for (int i = 0; i < bufferSize; i++)
    {
        uint32_t readFrom = i;
        GPU::ReadbackSingleEntity(readFrom);
        bgfx::frame();
        GPU::GetReadableTexture();
        bgfx::frame();

        uint32_t ready = bgfx::readTexture(GPU::ReadbackSingleEntityTexture, gpuData);
        while (bgfx::frame() < ready) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        // print expected data
        std::cout << "Expected Position for Entity " << readFrom << " (CPU side math): "
                << "X: " << positions[readFrom].x << ", "
                << "Y: " << positions[readFrom].y << ", "
                << "Z: " << positions[readFrom].z
                << std::endl;
        std::cout << "Expected Velocity for Entity " << readFrom << " (CPU side math): "
                << "X: " << velocities[readFrom].x << ", "
                << "Y: " << velocities[readFrom].y << ", "
                << "Z: " << velocities[readFrom].z
                << std::endl;
        std::cout << "Actual Position for Entity " << readFrom << " (GPU side math): "
                << "X: " << gpuData[0].x << ", "
                << "Y: " << gpuData[0].y << ", "
                << "Z: " << gpuData[0].z
                << std::endl;
        std::cout << "Actual Velocity for Entity " << readFrom << " (GPU side math): "
                << "X: " << gpuData[1].x << ", "
                << "Y: " << gpuData[1].y << ", "
                << "Z: " << gpuData[1].z
                << std::endl;

        // absolute errors
        errorsPos[i] = glm::vec4(
            gpuData[0].x - positions[readFrom].x,
            gpuData[0].y - positions[readFrom].y,
            gpuData[0].z - positions[readFrom].z,
            0.0f
        );
        errorsVel[i] = glm::vec4(
            gpuData[1].x - velocities[readFrom].x,
            gpuData[1].y - velocities[readFrom].y,
            gpuData[1].z - velocities[readFrom].z,
            0.0f
        );

        bgfx::frame();
    }

    free(gpuData);

    // Calculate average percentage errors
    float totalPosErrorPercent = 0.0f;
    float totalVelErrorPercent = 0.0f;
    int validPosComparisons = 0;
    int validVelComparisons = 0;

    for (int i = 0; i < bufferSize; i++) {
        // Calculate position error percentages
        glm::vec3 expectedPos(positions[i].x, positions[i].y, positions[i].z);
        glm::vec3 actualPosError(errorsPos[i].x, errorsPos[i].y, errorsPos[i].z);

        float expectedPosMagnitude = glm::length(expectedPos);
        if (expectedPosMagnitude > 1e-6f) { // Avoid division by near-zero
            float errorPosMagnitude = glm::length(actualPosError);
            float posErrorPercent = (errorPosMagnitude / expectedPosMagnitude) * 100.0f;
            totalPosErrorPercent += posErrorPercent;
            validPosComparisons++;
        }

        // Calculate velocity error percentages
        glm::vec3 expectedVel(velocities[i].x, velocities[i].y, velocities[i].z);
        glm::vec3 actualVelError(errorsVel[i].x, errorsVel[i].y, errorsVel[i].z);

        float expectedVelMagnitude = glm::length(expectedVel);
        if (expectedVelMagnitude > 1e-6f) { // Avoid division by near-zero
            float errorVelMagnitude = glm::length(actualVelError);
            float velErrorPercent = (errorVelMagnitude / expectedVelMagnitude) * 100.0f;
            totalVelErrorPercent += velErrorPercent;
            validVelComparisons++;
        }

        // visualize individual errors
        std::cout << "Error for Entity " << i << " Position: "
                  << "X: " << std::format("{:>14}", errorsPos[i].x) << ", "
                  << "Y: " << std::format("{:>14}", errorsPos[i].y) << ", "
                  << "Z: " << std::format("{:>14}", errorsPos[i].z) << " | Velocity: "
                  << "X: " << std::format("{:>14}", errorsVel[i].x) << ", "
                  << "Y: " << std::format("{:>14}", errorsVel[i].y) << ", "
                  << "Z: " << std::format("{:>14}", errorsVel[i].z)
                  << std::endl;
    }

    // Calculate averages
    float avgPosErrorPercent = validPosComparisons > 0 ? totalPosErrorPercent / validPosComparisons : 0.0f;
    float avgVelErrorPercent = validVelComparisons > 0 ? totalVelErrorPercent / validVelComparisons : 0.0f;

    // Print summary
    std::cout << "\n=== ERROR SUMMARY ===" << std::endl;
    std::cout << "Average Position Error: " << avgPosErrorPercent << "% (Max allowed: " << MAX_POSITION_ERROR_PERCENT << "%)" << std::endl;
    std::cout << "Average Velocity Error: " << avgVelErrorPercent << "% (Max allowed: " << MAX_VELOCITY_ERROR_PERCENT << "%)" << std::endl;
    std::cout << "Valid Position Comparisons: " << validPosComparisons << "/" << bufferSize << std::endl;
    std::cout << "Valid Velocity Comparisons: " << validVelComparisons << "/" << bufferSize << std::endl;

    // Verify that errors are within acceptable limits
    REQUIRE(avgPosErrorPercent <= MAX_POSITION_ERROR_PERCENT);
    REQUIRE(avgVelErrorPercent <= MAX_VELOCITY_ERROR_PERCENT);

    // Also ensure we had enough valid comparisons to make the test meaningful
    REQUIRE(validPosComparisons > bufferSize / 2); // At least half should be valid
    REQUIRE(validVelComparisons > bufferSize / 2); // At least half should be valid
}
