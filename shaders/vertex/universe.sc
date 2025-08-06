// vertex_universe.sc
$input a_position, a_texcoord0
$output v_color0, v_texcoord0

#include <bgfx_shader.sh>
//#include "shaderlib.sh"

//uniform mat4 u_modelViewProj;

void main() {
    // transform vertex
    gl_Position = mul(u_viewProj, vec4(a_position, 1.0));
    //gl_PointSize = sqrt(a_texcoord0) * 10.0;

    // pass data to frag shader
    v_color0 = vec4(0.0, 1.0, 0.0, 1.0);
    v_texcoord0 = a_texcoord0;
}