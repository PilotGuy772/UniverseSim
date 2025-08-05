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
    // simulate the full physics simulation for five frames
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

    // run simulation for five frames
    // using a constant deltaTime-- in the real sim, this will be controlled and possibly variable.
    float deltaTime = 0.016f; // 60 FPS
    for (int frame = 0; frame < 5; frame++) {
        GPU::DispatchVerletPosition(deltaTime);
        GPU::DispatchGravity();
        GPU::DispatchVerletVelocity(deltaTime);
        bgfx::frame();
        GPU::SwapBuffers();
    }

    // now, run the sim on CPU side for comparison
    const float G = 6.67430e-11f; // Gravitational constant
    std::vector<glm::vec4> accelerations(bufferSize, glm::vec4(0.0f));

    for (int f = 0; f < 5; f++) {
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

    // read back an entity in the middle of the pack to compare
    uint32_t readFrom = 60;
    GPU::ReadbackSingleEntity(readFrom);
    bgfx::frame();
    GPU::GetReadableTexture();
    bgfx::frame();

    auto* gpuData = static_cast<glm::vec4*>(malloc(3 * sizeof(glm::vec4)));
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

    // compare
    REQUIRE(gpuData[0].x == Catch::Approx(positions[readFrom].x).epsilon(0.01f));
    REQUIRE(gpuData[0].y == Catch::Approx(positions[readFrom].y).epsilon(0.01f));
    REQUIRE(gpuData[0].z == Catch::Approx(positions[readFrom].z).epsilon(0.01f));
    REQUIRE(gpuData[1].x == Catch::Approx(velocities[readFrom].x).epsilon(0.01f));
    REQUIRE(gpuData[1].y == Catch::Approx(velocities[readFrom].y).epsilon(0.01f));
    REQUIRE(gpuData[1].z == Catch::Approx(velocities[readFrom].z).epsilon(0.01f));

}
