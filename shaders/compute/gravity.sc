#include <bgfx_compute.sh>

BUFFER_R(positions, vec4, 0);   // xyz = pos, w = mass
BUFFER_W(accelerations, vec4, 1); // xyz = acceleration, w = unused

void main() {
    uint id = gl_GlobalInvocationID.x;
    vec3 pos_i = positions[id].xyz;
    float m_i = positions[id].w;

    vec3 acc = vec3(0.0);
    for (uint j = 0; j < u_particleCount; ++j) {
        if (id == j) continue;
        vec3 pos_j = positions[j].xyz;
        float m_j = positions[j].w;

        vec3 diff = pos_j - pos_i;
        float distSqr = max(dot(diff, diff), 0.01);
        float invDist = inversesqrt(distSqr);
        float invDist3 = invDist * invDist * invDist;
        acc += m_j * diff * invDist3;
    }

    accelerations[id] = vec4(acc, 0.0);
}
