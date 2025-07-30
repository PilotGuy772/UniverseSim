#include <bgfx_compute.sh>

BUFFER_R(positionsIn, vec4, 0);
BUFFER_R(velocitiesIn, vec4, 1);
BUFFER_R(accelerationsIn, vec4, 2);

BUFFER_W(positionsOut, vec4, 3);
BUFFER_W(velocitiesOut, vec4, 4);

uniform float u_dt;

void main() {
    uint id = gl_GlobalInvocationID.x;

    vec3 pos = positionsIn[id].xyz;
    vec3 vel = velocitiesIn[id].xyz;
    vec3 acc = accelerationsIn[id].xyz;

    // Velocity-Verlet integration step
    vec3 newVel = vel + acc * u_dt;
    vec3 newPos = pos + newVel * u_dt;

    positionsOut[id] = vec4(newPos, positionsIn[id].w); // preserve mass
    velocitiesOut[id] = vec4(newVel, 0.0);
}
