//
// Created by Laeth English on 8/5/25.
//

#ifndef SIMULATION_HPP
#define SIMULATION_HPP
#include <stopwatch/Stopwatch.hpp>
#include <glm/glm.hpp>

namespace Simulation {

    inline stopwatch::Stopwatch Stopwatch;
    inline glm::vec3 CameraPosition{0.0f, 0.0f, 0.0f};
    inline

    void RunMainThread();

}

#endif //SIMULATION_HPP
