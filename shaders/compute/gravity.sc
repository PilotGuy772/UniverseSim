#include <bgfx_compute.sh>

// REAONLY INPUTS //
BUFFER_RO(flags, uint, 0);
BUFFER_RO(positions, vec4, 1);

// OUTPUTS //
BUFFER_RW(accelerations, vec4, 2);

NUM_THREADS(16, 16, 1)
void main() {
    // use Newton's law of gravitation to compute the accelerations.
    // remember:
    // F = G * m1 * m2 / r^2
    // r = sqrt(dx^2 + dy^2 + dz^2)
    // a = F / m

    uint idx = gl_GlobalInvocationID.x;
    uint idy = gl_GlobalInvocationID.y;

    //accelerations[id] = vec4(1.0, 2.0, 3.0, 1.0);
    // check if the bitmask has zero in the first position
    if ((flags[idx] & 0x01) == 0 || (flags[idy] & 0x01) == 0) {
        // this particle is not active, skip it
        return;
    }

    if (idx == idy) {
        return; // skip self
    }

    // testing
    //accelerations[idx].x += 0.1;
    //accelerations[idx].y += 0.2;
    //accelerations[idx].z += 0.4;

    vec4 pos1 = positions[idx];
    vec4 pos2 = positions[idy];
    // check if the particles are too far apart
    // compute the distance squared
    float dx = pos2.x - pos1.x;
    float dy = pos2.y - pos1.y;
    float dz = pos2.z - pos1.z;
    float distSq = dx * dx + dy * dy + dz * dz;

    // avoid division by zero
    if (distSq < 0.0001) {
        accelerations[idx].x = 100.0;
        return; // skip too close particles
    }

    // compute the gravitational force
    float force = 6.67430e-11 * (pos1.w * pos2.w) / distSq; // G * m1 * m2 / r^2

    // compute the acceleration
    float dist = sqrt(distSq);
    float ax = force * dx / dist / pos1.w;
    float ay = force * dy / dist / pos1.w;
    float az = force * dz / dist / pos1.w;

    // set the acceleration in the accelerations buffer
    atomicAdd(accelerations[idx].x, ax);
    atomicAdd(accelerations[idx].y, ay);
    atomicAdd(accelerations[idx].z, az);
}
