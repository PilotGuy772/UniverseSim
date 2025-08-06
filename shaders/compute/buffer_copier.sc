// buffer_copier.sc
#include <bgfx_compute.sh>

// INPUTS //
BUFFER_RO(positions_old, vec4, 0);
BUFFER_RO(velocities_old, vec4, 1);
BUFFER_RO(accelerations_old, vec4, 2);
BUFFER_RO(flags_old, uint, 3);

// OUTPUTS //
BUFFER_WO(positions_new, vec4, 4);
BUFFER_WO(velocities_new, vec4, 5);
BUFFER_WO(accelerations_new, vec4, 6);
BUFFER_WO(flags_new, uint, 7);

NUM_THREADS(1, 1, 1)
void main() {
    uint id = gl_GlobalInvocationID.x;

    positions_new[id] = positions_old[id];
    velocities_new[id] = velocities_old[id];
    accelerations_new[id] = accelerations_old[id];
    flags_new[id] = flags_old[id];
}