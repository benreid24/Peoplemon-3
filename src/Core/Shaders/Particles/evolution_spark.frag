#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in flat vec2 fragCenter;
layout(location = 1) in flat float fragAlpha;
layout(location = 2) in flat float fragRadius;

layout(location = 0) out vec4 outColor;

void main() {
    float d = distance(fragCenter, gl_FragCoord.xy);
    float ra = max(1.0 - d / fragRadius, 0.0);
    outColor = vec4(0.988, 0.90, 0.118, fragAlpha * ra);
}
