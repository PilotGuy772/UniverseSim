$input a_position, i_data0//a_texcoord0, a_color0//, a_texcoord1
$output v_texcoord0, v_color0

#include <bgfx_shader.sh>

void main() {
    // Apply instance position offset to the geometry vertex
    vec3 worldPos = vec3(a_position.x + i_data0.x, a_position.y + i_data0.y, a_position.z + i_data0.z);

    // Transform to clip space
    vec4 finalWorldPos = mul(u_model[0], vec4(worldPos, 1.0));
    gl_Position = mul(u_viewProj, finalWorldPos);
    //v_texcoord0 = a_texcoord0;
    v_color0 = vec4(1.0, 0.5, 0.2, 1.0);

}