// vertlet-position.sc
#include <bgfx_compute.sh>

// READONLY INPUTS //
BUFFER_RO(flags, uint, 0);
BUFFER_RO(velocities, float4, 1);
BUFFER_RO(accelerations, float4, 2);
BUFFER_RO(positions_old, float4, 3);


// OUTPUTS //
BUFFER_WO(positions_new, float4, 4);

// UNIFORMS //
uniform float deltaTime;


void main() {
    // velocity-verlet for position:
    // x(t + dt) = x(t) + v(t) * dt + 0.5 * a(t) * dt^2
    uint id = gl_GlobalInvocationID.x;

    // check if the bitmask has zero in the first position
    if ((flags[id] & 0x1) == 0) {
        // this particle is not active, skip it
        return;
    }

    positions_new[id].x = positions_old[id].x + velocities[id].x * deltaTime + 0.5 * accelerations[id].x * deltaTime * deltaTime;
    positions_new[id].y = positions_old[id].y + velocities[id].y * deltaTime + 0.5 * accelerations[id].y * deltaTime * deltaTime;
    positions_new[id].z = positions_old[id].z + velocities[id].z * deltaTime + 0.5 * accelerations[id].z * deltaTime * deltaTime;
    // w doesn't change
    positions_new[id].w = positions_old[id].w; // keep mass unchanged, if needed
}
