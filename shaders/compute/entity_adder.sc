// entity_adder.sc
#include <bgfx_compute.sh>

// simple compute shader to add entities to the scene

// OUTPUTS //
BUFFER_WO(positions_old, float4, 0);
BUFFER_WO(velocities_old, float4, 1);

// UNIFORMS //
uniform float4 u_position;
uniform float4 u_velocity;
uniform float4 u_index;

void main() {
    // set the position and velocity
    uint index_as_int = uint(u_index.x);
    positions_old[index_as_int] = u_position;
    velocities_old[index_as_int] = u_velocity;
}