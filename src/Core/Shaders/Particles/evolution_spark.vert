#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoords;

layout(location = 0) out flat vec2 fragCenter;
layout(location = 1) out flat float fragAlpha;
layout(location = 2) out flat float fragRadius;

layout(set = 0, binding = 0) uniform cam {
    mat4 viewProj;
} camera;

struct Particle {
    vec2 pos;
    float radius;
    float alpha;
};

layout(std140, set = 1, binding = 0) readonly buffer pcl {
    Particle particles[];
} particles;

layout(std140, set = 2, binding = 1) uniform gpinfo {
    float cameraToWindowScale;
} globalInfo;

void main() {
    vec2 pos = particles.particles[gl_InstanceIndex].pos;
    mat4 particleTransform = mat4(1.0);
    particleTransform[3][0] = pos[0];
    particleTransform[3][1] = pos[1];
    vec4 worldPos = particleTransform * vec4(inPosition, 1.0);
	gl_Position = camera.viewProj * worldPos;
    
    fragCenter = vec2(worldPos.x, worldPos.y);
    fragAlpha = particles.particles[gl_InstanceIndex].alpha;
    fragRadius = particles.particles[gl_InstanceIndex].radius;
    gl_PointSize = fragRadius * globalInfo.cameraToWindowScale;

}
