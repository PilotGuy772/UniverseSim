// simple_compute.sc
$input v_texcoord0;

#include <bgfx_compute.sh>
#include <bgfx_shader.sh>

BUFFER_RO(buffer0, vec4, 64);
IMAGE2D_RW(result, rgba8, 1)

void main()
{
    uint idx = gl_GlobalInvocationID.x;
    imageStore(result, ivec2(gl_GlobalInvocationID.x), vec4(buffer0[idx].x * 2.0, 0.0, 0.0, 0.0));
}
