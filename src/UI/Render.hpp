//
// Created by Laeth English on 8/6/25.
//

#ifndef RENDER_HPP
#define RENDER_HPP

#include <bgfx/bgfx.h>

namespace UI {
    inline bgfx::ProgramHandle ShaderProgram;
    inline bgfx::UniformHandle ViewProjMatrixUniform;
    inline bgfx::VertexBufferHandle QuadVB;
    inline bgfx::IndexBufferHandle QuadIB;

    int InitializeRenderer();
    void CreateQuadGeometry();
    void RenderScene();
}

#endif //RENDER_HPP
