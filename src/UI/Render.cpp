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

    //ViewProjMatrixUniform = bgfx::createUniform("u_modelViewProj", bgfx::UniformType::Mat4);
    // if (!bgfx::isValid(ViewProjMatrixUniform)) {
    //     std::cerr << "Error creating uniform matrix" << std::endl;
    //     return 1;
    // }
    return 0;
}

void UI::RenderScene() {
    // set vertex buffer
    bgfx::setVertexBuffer(0, GPU::PositionsBuffer_Old);

    // set viewproj uniform
    glm::mat4 projmat = GetProjectionMatrix();
    glm::mat4 viewmat = GetViewMatrix();

    glm::mat4 matrix = projmat * viewmat;
    //bgfx::setUniform(ViewProjMatrixUniform, &matrix);
    bgfx::setViewTransform(Core::VIEW_ID_MAIN, &viewmat, &projmat);

    // set state
    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_Z | BGFX_STATE_PT_POINTS);

    // draw!
    bgfx::submit(Core::VIEW_ID_MAIN, ShaderProgram);
}

