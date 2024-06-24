#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoords;

layout(location = 0) flat out float centerAlpha;
layout(location = 1) flat out float radius;
layout(location = 2) out vec2 fragCenter;

layout(set = 0, binding = 0) uniform cam {
    mat4 viewProj;
} camera;

struct Particle {
    vec2 pos;
    float radius;
    float progress;
};

layout(std140, set = 1, binding = 0) readonly buffer pcl {
    Particle particles[];
} particles;

layout(std140, set = 1, binding = 1) uniform gpinfo {
    float cameraToWindowScale;
} globalInfo;

layout(set = 1, binding = 2) uniform gpcl {
    float alphaReduce;
} globals;

void main() {
    Particle particle = particles.particles[gl_InstanceIndex];

    mat4 particleTransform = mat4(1.0);
    particleTransform[3][0] = particle.pos[0];
    particleTransform[3][1] = particle.pos[1];
    vec4 worldPos = particleTransform * vec4(inPosition, 1.0);
	gl_Position = camera.viewProj * worldPos;

    gl_PointSize = particle.radius;
    centerAlpha = 1.0 - globals.alphaReduce * particle.progress;
    radius = particle.radius;
    fragCenter = worldPos.xy;
}
