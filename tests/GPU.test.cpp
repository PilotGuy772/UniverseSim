//
// Created by Laeth English on 7/29/25.
//
#include <catch2/catch_all.hpp>
#include <GPU/ComputeBridge.hpp>
#include "Core/Application.hpp"

void fillRandomData(std::vector<uint32_t>* data) {
    for (auto& value : *data) {
        value = rand() % 100; // Fill with random values between 0 and 99
    }
}

TEST_CASE("Compute Shader Test", "[GPU]") {
    // initialize BGFX first
    REQUIRE(Core::Init() == 0);

    // Example input data for the compute shader
    // vector of size 64 with random values
    std::vector<uint32_t> input(64);
    fillRandomData(&input);
    uint32_t* result = nullptr;

    // Call the compute shader test function
    GPU::TestComputeShader(input, &result);

    // Check if the result is not null
    REQUIRE(result != nullptr);

    // Check if the result matches expected output
    // the shader simply doubles each input value:
    for (size_t i = 0; i < input.size(); ++i) {
        REQUIRE(result[i] == input[i] * 2);
    }

    // Clean up allocated memory
    free(result);
}