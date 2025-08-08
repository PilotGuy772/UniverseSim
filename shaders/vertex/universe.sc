$input a_position, i_data0//a_texcoord0, a_color0//, a_texcoord1
$output v_texcoord0, v_color0

#include <bgfx_shader.sh>

void main() {
    vec4 worldPos = mul(u_model[0], vec4(i_data0.xyz, 1.0));
    gl_Position = mul(u_viewProj, worldPos);
    //v_texcoord0 = a_texcoord0;
    v_color0 = vec4(1.0, 0.5, 0.2, 1.0);

}