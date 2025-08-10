//
// Created by Laeth English on 8/6/25.
//

#include "Render.hpp"

#include <iostream>

#include "CameraHandler.hpp"
#include "../Core/ApplicationConfig.hpp"
#include "../GPU/BufferManager.hpp"
#include "bx/math.h"
#include "glm/glm.hpp"

#if defined(__APPLE__)
#include <metal/vertex_universe.bin.h>
#include <metal/fragment_universe.bin.h>
#endif

int UI::InitializeRenderer() {
    const bgfx::ShaderHandle vs = bgfx::createShader(bgfx::makeRef(vertex_universe, sizeof(vertex_universe)));
    const bgfx::ShaderHandle fs = bgfx::createShader(bgfx::makeRef(fragment_universe, sizeof(fragment_universe)));
    if (!bgfx::isValid(vs) || !bgfx::isValid(fs)) {
        std::cerr << "Error loading shader handles" << std::endl;
        return 1;
    }
    ShaderProgram = bgfx::createProgram(vs, fs, true);
    if (!bgfx::isValid(ShaderProgram)) {
        std::cerr << "Error creating shader program" << std::endl;
        return 1;
    }

    CreateQuadGeometry();

    //ViewProjMatrixUniform = bgfx::createUniform("u_modelViewProj", bgfx::UniformType::Mat4);
    // if (!bgfx::isValid(ViewProjMatrixUniform)) {
    //     std::cerr << "Error creating uniform matrix" << std::endl;
    //     return 1;
    // }
    return 0;
}

// Add this to your initialization
void UI::CreateQuadGeometry() {
    struct CubeVertex {
        float x, y, z;
        float nx, ny, nz;
        float u, v;
    };

    CubeVertex cubeVertices[] = {
        // Front face (Z+)
        {-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f}, // Bottom-left
        { 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f}, // Bottom-right
        { 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f}, // Top-right
        {-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f}, // Top-left

        // Back face (Z-)
        { 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f}, // Bottom-left
        {-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f}, // Bottom-right
        {-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f}, // Top-right
        { 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f}, // Top-left

        // Left face (X-)
        {-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f}, // Bottom-left
        {-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f}, // Bottom-right
        {-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f}, // Top-right
        {-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f}, // Top-left

        // Right face (X+)
        { 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f}, // Bottom-left
        { 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f}, // Bottom-right
        { 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f}, // Top-right
        { 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f}, // Top-left

        // Top face (Y+)
        {-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f}, // Bottom-left
        { 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f}, // Bottom-right
        { 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f}, // Top-right
        {-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f}, // Top-left

        // Bottom face (Y-)
        {-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f}, // Bottom-left
        { 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f}, // Bottom-right
        { 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f}, // Top-right
        {-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f}  // Top-left
    };

    // Indices for cube faces (2 triangles per face)
    uint16_t cubeIndices[] = {
        // Front face
        0,  1,  2,    0,  2,  3,
        // Back face
        4,  5,  6,    4,  6,  7,
        // Left face
        8,  9,  10,   8,  10, 11,
        // Right face
        12, 13, 14,   12, 14, 15,
        // Top face
        16, 17, 18,   16, 18, 19,
        // Bottom face
        20, 21, 22,   20, 22, 23
    };

    bgfx::VertexLayout quadLayout;
    quadLayout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();

    QuadVB = bgfx::createVertexBuffer(bgfx::copy(cubeVertices, sizeof(cubeVertices)), quadLayout);
    QuadIB = bgfx::createIndexBuffer(bgfx::copy(cubeIndices, sizeof(cubeIndices)));
}

void UI::RenderScene() {
    // Set quad geometry
    bgfx::setVertexBuffer(0, QuadVB);
    bgfx::setIndexBuffer(QuadIB);

    // set vertex buffer
    bgfx::setInstanceDataBuffer(GPU::PositionsBuffer_Old, 0, Simulation::Entities.size());

    // set viewproj uniform
    glm::mat4 projmat = GetProjectionMatrix();
    glm::mat4 viewmat = GetViewMatrix();
    /*float projmat[16];
    Mat4ToFloatArray(GetProjectionMatrix(), projmat);
    float viewmat[16];
    Mat4ToFloatArray(GetViewMatrix(), viewmat);*/

    bgfx::setViewTransform(Core::VIEW_ID_MAIN, &viewmat, &projmat);


    // set state
    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_Z | BGFX_STATE_BLEND_ALPHA);

    // draw!
    bgfx::submit(Core::VIEW_ID_MAIN, ShaderProgram);
}

