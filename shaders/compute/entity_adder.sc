// entity_adder.sc
#include <bgfx_compute.sh>

// simple compute shader to add entities to the scene

// OUTPUTS //
BUFFER_WO(positions_old, vec4, 0);
BUFFER_WO(velocities_old, vec4, 1);

// UNIFORMS //
uniform vec4 u_position;
uniform vec4 u_velocity;
uniform vec4 u_index;

NUM_THREADS(1, 1, 1)
void main() {
    // set the position and velocity
    uint index_as_int = uint(u_index.x);

    positions_old[index_as_int] = vec4(999.99, 888.88, 777.77, 1.0); // debug position
    velocities_old[index_as_int] = vec4(666.66, 555.55, 444.44, 1.0); // debug velocity

    //positions_old[index_as_int] = u_position;
    //velocities_old[index_as_int] = u_velocity;
}