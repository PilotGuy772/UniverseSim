//
// Created by Laeth English on 7/28/25.
//

#ifndef COMPUTEBRIDGE_HPP
#define COMPUTEBRIDGE_HPP

#include <bgfx/bgfx.h>
#include <bx/bx.h>
#include <bx/math.h>
#include <iostream>
#include <vector>
#include <cstring>

namespace GPU {
    void TestComputeShader(const std::vector<uint32_t> &input, uint32_t** result);
}

#endif //COMPUTEBRIDGE_HPP
