//
// Created by Laeth English on 8/6/25.
//

#include "Render.hpp"

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
    ShaderProgram = bgfx::createProgram(vs, fs);
    if (!bgfx::isValid(ShaderProgram)) { return 1; }
    return 0;
}

void UI::RenderScene() {
    // set vertex buffer
    bgfx::setVertexBuffer(0, GPU::PositionsBuffer_Old);

    // set viewproj uniform
    glm::mat4 matrix = GetProjectionMatrix() * GetViewMatrix();
    bgfx::setUniform(ViewProjMatrixUniform, &matrix);

    // set state
    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_Z | BGFX_STATE_PT_POINTS);

    // draw!
    bgfx::submit(Core::VIEW_ID_MAIN, ShaderProgram);
}

