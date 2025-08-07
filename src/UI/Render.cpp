//
// Created by Laeth English on 8/6/25.
//

#include "Render.hpp"

#include <iostream>

#include "CameraHandler.hpp"
#include "../Core/ApplicationConfig.hpp"
#include "../GPU/BufferManager.hpp"
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
    struct QuadVertex {
        float x, y, z;
        float u, v;
    };

    QuadVertex vertices[] = {
        {-1.0f, -1.0f, 0.0f, 0.0f, 0.0f},
        { 1.0f, -1.0f, 0.0f, 1.0f, 0.0f},
        { 1.0f,  1.0f, 0.0f, 1.0f, 1.0f},
        {-1.0f,  1.0f, 0.0f, 0.0f, 1.0f}
    };

    uint16_t indices[] = {0, 1, 2, 0, 2, 3};

    bgfx::VertexLayout quadLayout;
    quadLayout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();

    QuadVB = bgfx::createVertexBuffer(bgfx::makeRef(vertices, sizeof(vertices)), quadLayout);
    QuadIB = bgfx::createIndexBuffer(bgfx::makeRef(indices, sizeof(indices)));
}

void UI::RenderScene() {
    // Set quad geometry
    bgfx::setVertexBuffer(0, QuadVB);
    bgfx::setIndexBuffer(QuadIB);

    // set vertex buffer
    bgfx::setVertexBuffer(1, GPU::PositionsBuffer_Old);

    // set viewproj uniform
    glm::mat4 projmat = GetProjectionMatrix();
    glm::mat4 viewmat = GetViewMatrix();

    glm::mat4 matrix = projmat * viewmat;
    bgfx::setViewTransform(Core::VIEW_ID_MAIN, &viewmat, &projmat);

    // set state
    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_Z | BGFX_STATE_BLEND_ALPHA);

    // draw!
    bgfx::submit(Core::VIEW_ID_MAIN, ShaderProgram);
}

