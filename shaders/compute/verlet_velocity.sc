// verlet_velocity.sc
#include <bgfx_compute.sh>

// READONLY INPUTS //
BUFFER_RO(flags, uint, 0);
BUFFER_RO(accelerations, float4, 1);
BUFFER_RO(velocities_old, float4, 2);

// OUTPUTS //
BUFFER_WO(velocities_new, float4, 3);

// UNIFORMS //
uniform float deltaTime;

void main() {
    // velocity-verlet for velocity:
    // v(t + dt) = v(t) + 0.5 * (a(t) + a(t + dt)) * dt
    uint id = gl_GlobalInvocationID.x;

    // compute the new velocities based on the old velocities and accelerations
    velocities_new[id].x = velocities_old[id].x + 0.5 * (accelerations[id].x + accelerations[id].x) * deltaTime;
    velocities_new[id].y = velocities_old[id].y + 0.5 * (accelerations[id].y + accelerations[id].y) * deltaTime;
    velocities_new[id].z = velocities_old[id].z + 0.5 * (accelerations[id].z + accelerations[id].z) * deltaTime;
    // w is unused
}