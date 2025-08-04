// readback_single.sc
// This shader reads back data for a single entity from the relevant buffers and encodes it in an image
#include <bgfx_compute.sh>
//#include <bgfx_shader.sh>

// READONLY INPUTS //
BUFFER_RO(flags, uint, 0);
BUFFER_RO(positions, vec4, 1);
BUFFER_RO(velocities, vec4, 2);
BUFFER_RO(accelerations, vec4, 3);

// OUTPUTS //
IMAGE2D_WO(outputImage, rgba32f, 4); // output image to store the readback data

// UNIFORMS //
uniform vec4 u_index; // index of the entity to read back

NUM_THREADS(1, 1, 1)
void main() {
    uint id = uint(u_index.x);

    // check if the bitmask has zero in the first position
    //if ((flags[id] & 0x1) == 0) {
        // this particle is not active, skip it
    //    return;
    //}

    // read the position and velocity of the entity
    vec4 position = positions[id];
    vec4 velocity = velocities[id];
    vec4 acceleration = accelerations[id];

    // encode the data into an image
    // Assuming the image format is RGBA8, we can store position in RGB and velocity in A
    // image will be 1D with three pixels; one pixel can encode a single vec4.
    // 0, 0 stores position + mass
    // 1, 0 stores velocity (+ unused w component)
    // 2, 0 stores acceleration (+ unused w component)
    //imageStore(outputImage, ivec2(0, 0), vec4(position.x, position.y, position.z, position.w)); // position + mass
    imageStore(outputImage, ivec2(0, 0), vec4(999.9, 888.8, 777.7, 10.1)); // acceleration + unused
    //imageStore(outputImage, ivec2(1, 0), vec4(velocity.x, velocity.y, velocity.z, 10.1)); // velocity + unused
    imageStore(outputImage, ivec2(1, 0), vec4(666.6, 555.5, 444.4, 10.1)); // acceleration + unused
    //imageStore(outputImage, ivec2(2, 0), vec4(acceleration.x, acceleration.y, acceleration.z, 10.1)); // acceleration + unused
    imageStore(outputImage, ivec2(2, 0), vec4(333.3, 222.2, 111.1, 10.1)); // acceleration + unused

}