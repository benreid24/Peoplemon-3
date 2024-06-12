#version 450

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

layout(set = 2, binding = 0) uniform fu {
    vec2 windowSize;
    float progress;
} state;

void main() {
    float barHeight = state.windowSize.y / 12.0;
    float barF = floor(gl_FragCoord.y / barHeight);
    int bar = int(barF);

    float radius = barHeight / 2.0;
    float localY = gl_FragCoord.y - barHeight * barF;
    float triangleY = localY - radius;
    float circleOffset = sqrt(radius * radius - triangleY * triangleY);
    float barWidth = state.windowSize.x * state.progress;

    if (bar % 2 == 0) {
        if (gl_FragCoord.x > barWidth + circleOffset) {
            discard;
        }
    }
    else {
        if (gl_FragCoord.x < state.windowSize.x - barWidth - circleOffset) {
            discard;
        }
    }

    outColor = fragColor;
}
