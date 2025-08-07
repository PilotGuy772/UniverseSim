$input a_position, a_texcoord0, i_data0
$output v_texcoord0, v_color0

#include <bgfx_shader.sh>

void main() {
    // Base quad vertex position (local space: -1 to 1)
    vec3 localPos = a_position;

    // Instance data: position + mass
    vec3 instancePos = i_data0.xyz;
    //gl_Position = vec4(instancePos, 1.0f);
    //vec3 instancePos = vec3(0.0, 0.0, -5.0);
    float instanceMass = 8.0f;

    // Scale based on mass
    float scale = 1.0f;//sqrt(instanceMass) * 10.0;

    // Transform: scale local quad and move to instance position
    vec3 worldPos = localPos * scale + instancePos;

    // Apply view-projection transform
    gl_Position = mul(u_viewProj, vec4(worldPos, 1.0));

    // Pass data to fragment shader
    v_texcoord0 = a_texcoord0;  // UV from quad
    v_color0 = vec4(1.0, 0.8, 0.4, 1.0);
}