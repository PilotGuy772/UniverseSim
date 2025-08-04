#include <bgfx_compute.sh>

// REAONLY INPUTS //
BUFFER_RO(flags, uint, 0);
BUFFER_RO(positions, vec4, 1);

// OUTPUTS //
BUFFER_WO(accelerations, vec4, 2);

// UNIFORMS //
uniform uint u_numParticles;

NUM_THREADS(64, 1, 1)
void main() {
    // use Newton's law of gravitation to compute the accelerations
    // remember:
    // F = G * m1 * m2 / r^2
    // r = sqrt(dx^2 + dy^2 + dz^2)
    // a = F / m

    uint id = gl_GlobalInvocationID.x;
    // check if the bitmask has zero in the first position
    if ((flags[id] & 0x1) == 0) {
        // this particle is not active, skip it
        return;
    }

    for (uint other = 0; other < u_numParticles; other++) {
        if (other == id) {
            continue; // skip self
        }

        vec4 pos1 = positions[id];
        vec4 pos2 = positions[other];

        // compute the distance squared
        float dx = pos2.x - pos1.x;
        float dy = pos2.y - pos1.y;
        float dz = pos2.z - pos1.z;
        float distSq = dx * dx + dy * dy + dz * dz;

        // avoid division by zero
        if (distSq < 0.0001) {
            continue; // skip too close particles
        }

        // compute the gravitational force
        float force = 6.67430e-11 * (pos1.w * pos2.w) / distSq; // G * m1 * m2 / r^2

        // compute the acceleration
        float ax = force * dx / sqrt(distSq);
        float ay = force * dy / sqrt(distSq);
        float az = force * dz / sqrt(distSq);

        // set the acceleration in the accelerations buffer
        accelerations[id].x = ax;
        accelerations[id].y = ay;
        accelerations[id].z = az;
    }
}
