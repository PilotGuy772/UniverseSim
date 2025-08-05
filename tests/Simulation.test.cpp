//
// Created by Laeth English on 8/4/25.
//
#include <random>
#include <thread>
#include <catch2/catch_all.hpp>
#include <GPU/BufferManager.hpp>
#include <GPU/ComputeBridge.hpp>
#include <Core/Application.hpp>

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

    // print starting data
    for (int i = 0; i < bufferSize; i++) {
        std::cout << "Entity " << i << ": Position: " << positions[i].x << ", " << positions[i].y << ", " << positions[i].z
                  << " | Velocity: " << velocities[i].x << ", " << velocities[i].y << ", " << velocities[i].z << std::endl;
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

TEST_CASE("Verlet position calculation", "[Simulation]") {

}