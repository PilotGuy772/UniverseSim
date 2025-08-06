//
// Created by Laeth English on 7/30/25.
//

#include "EntityManager.hpp"

#include "../GPU/BufferManager.hpp"

void Simulation::InitializeEntities(const int bufferSize) {
    EntityQueue = std::queue<NewEntity>();
    Entities = std::vector<Entity>();
    GpuIndices = std::queue<uint32_t>();
    for (int i = 0; i < bufferSize; i++) {
        GpuIndices.push(i);
    }
}

void Simulation::QueueNewEntity(const glm::vec3 newPosition, const float newMass, const glm::vec3 newVelocity, const uint32_t newFlags) {
    EntityQueue.push(NewEntity(
        glm::vec4{newPosition, newMass},
        glm::vec4{newVelocity, 0.0f},
        newFlags));
}

void Simulation::KillEntity(const uint32_t entityHandle) {
    const uint32_t gpuIndex = Entities[entityHandle].GpuIndex;
    GPU::KillEntity(gpuIndex);
}

void Simulation::KillNextEntityFromQueue() {
    uint32_t handle = EntityDeathQueue.front();
    EntityDeathQueue.pop();
    KillEntity(handle);
}

int Simulation::AddNextEntityFromQueue() {
    if (GpuIndices.size() < 1) return -1;

    NewEntity newEntity = EntityQueue.front();
    EntityQueue.pop();
    uint32_t gpuIndex = GpuIndices.front();
    GpuIndices.pop();

    const auto target = Entity{static_cast<uint32_t>(Entities.size()), gpuIndex, true};
    Entities.push_back(target);

    GPU::AddEntity(gpuIndex, newEntity.PositionMass, newEntity.Velocity, newEntity.Flags);

    return Entities.size() - 1;
}

