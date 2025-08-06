//
// Created by Laeth English on 7/30/25.
//

#ifndef ENTITYMANAGER_HPP
#define ENTITYMANAGER_HPP
#include <queue>
#include <vector>

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

namespace Simulation {
    struct Entity {
        uint32_t Handle;
        uint32_t GpuIndex;
        bool Alive;
    };

    struct NewEntity {
        glm::vec4 PositionMass;
        glm::vec4 Velocity;
        uint32_t Flags;
    };

    inline std::queue<NewEntity> EntityQueue;
    inline std::queue<uint32_t> EntityDeathQueue;
    inline std::vector<Entity> Entities;     // handle -> Entity
    inline std::queue<uint32_t> GpuIndices; // list of available indices in the GPU buffers

    void InitializeEntities(int bufferSize);
    void QueueNewEntity(glm::vec3 newPosition, float newMass, glm::vec3 newVelocity, uint32_t newFlags = 1);
    void KillEntity(uint32_t entityHandle);
    void KillNextEntityFromQueue();

    /**
     * Add the first entity from the queue to the data buffers. Must call bgfx::frame() between each call.
     * @return The entity handle of the newly added entity. Returns -1 if the buffers have no space for the target entity.
     */
    int AddNextEntityFromQueue();
}

#endif //ENTITYMANAGER_HPP
