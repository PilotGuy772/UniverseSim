// vertex_universe.sc
$input a_position, a_texcoord0, a_texcoord1
$output v_color0, v_texcoord0

#include <bgfx_shader.sh>
//#include "shaderlib.sh"

//uniform mat4 u_modelViewProj;

void main() {
    vec3 entityPos = a_texcoord1.xyz;
    float entityMass = a_texcoord1.w;

    // Scale quad based on mass
    float scale = sqrt(entityMass) * 20.0;  // Adjust scaling

    // Position quad at entity location
    vec3 worldPos = a_position * scale + entityPos;

    gl_Position = mul(u_viewProj, vec4(worldPos, 1.0));

    v_texcoord0 = a_texcoord0;  // UV from quad (0,0 to 1,1)
    v_color0 = vec4(1.0, 1.0, 1.0, 1.0);
}