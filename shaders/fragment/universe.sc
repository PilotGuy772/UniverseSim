// fragment_universe.sc
$input v_color0, v_texcoord0

#include <bgfx_shader.sh>

void main() {
    // Create circle using UV coordinates
    vec2 uv = v_texcoord0 - 0.5;  // Center at (0,0)
    float dist = length(uv);

    // Circular shape
    float alpha = 1.0 - smoothstep(0.3, 0.5, dist);

    gl_FragColor = vec4(v_color0.rgb, alpha);
    //gl_FragColor = v_color0;
    //gl_FragColor = vec4(0.0, v_texcoord0, 0.0, 1.0);
}