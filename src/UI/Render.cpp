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

inline bgfx::VertexBufferHandle dummyBuffer;
inline bgfx::VertexLayoutHandle dummyLayoutHandle;

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

    // temp
    struct DummyVertex  { float x, y, z; };
    DummyVertex vert = {0.0f, 0.0f, 0.0f};

    bgfx::VertexLayout dummyLayout;
    dummyLayout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .end();
    dummyLayoutHandle = bgfx::createVertexLayout(dummyLayout);

    dummyBuffer = bgfx::createVertexBuffer(
        bgfx::copy(&vert, sizeof(DummyVertex)), dummyLayout);

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
    struct QuadVertex {
        float x, y, z;
        float u, v;
    };

    QuadVertex vertices[] = {
        {-1.0f,  1.0f,  1.0f, 0.0f, 0.0f },
        { 1.0f,  1.0f,  1.0f, 0.0f, 0.0f },
        {-1.0f, -1.0f,  1.0f, 0.0f, 0.0f },
        { 1.0f, -1.0f,  1.0f, 0.0f, 0.0f },
        {-1.0f,  1.0f, -1.0f, 0.0f, 0.0f },
        { 1.0f,  1.0f, -1.0f, 0.0f, 0.0f },
        {-1.0f, -1.0f, -1.0f, 0.0f, 0.0f },
        { 1.0f, -1.0f, -1.0f, 0.0f, 0.0f },
    };

    uint16_t indices[] = {
        0, 1, 2,
        1, 3, 2,
        4, 6, 5,
        5, 6, 7,
        0, 2, 4,
        4, 2, 6,
        1, 5, 3,
        5, 7, 3,
        0, 4, 1,
        4, 5, 1,
        2, 3, 6,
        6, 3, 7,
    };

    bgfx::VertexLayout quadLayout;
    quadLayout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();

    QuadVB = bgfx::createVertexBuffer(bgfx::makeRef(vertices, sizeof(vertices)), quadLayout);
    QuadIB = bgfx::createIndexBuffer(bgfx::makeRef(indices, sizeof(indices)));
}

void Mat4ToFloatArray(const glm::mat4& mat, float out[16]) {
    // GLM stores mat4 in column-major by default, so we can copy directly
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            out[col * 4 + row] = mat[col][row];
        }
    }
}

void UI::RenderScene() {
    // Set quad geometry
    //bgfx::setVertexBuffer(0, QuadVB);
    //bgfx::setIndexBuffer(QuadIB);



    // set vertex buffer
    //bgfx::setVertexBuffer(0, GPU::PositionsBuffer_Old, 0, Simulation::Entities.size(), GPU::VertexLayoutHandle);
    bgfx::setVertexBuffer(0, dummyBuffer);
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
    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_Z | BGFX_STATE_PT_POINTS);

    // draw!
    bgfx::submit(Core::VIEW_ID_MAIN, ShaderProgram);
}

