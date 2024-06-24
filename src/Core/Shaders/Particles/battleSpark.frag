#version 450

layout(location = 0) flat in float centerAlpha;
layout(location = 1) flat in float radius;
layout(location = 2) in vec2 fragCenter;

layout(location = 0) out vec4 outColor;

void main() {
    float d = distance(fragCenter, gl_FragCoord.xy);
    float ra = max(1.0 - d / radius, 0.0);
    outColor = vec4(0.984, 0.898, 0.117, centerAlpha * ra);
}
