#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoords;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoords;
layout(location = 2) flat out uint fragTextureId;
layout(location = 3) out vec2 fragPos;
layout(location = 4) flat out float rotation;

layout(set = 1, binding = 0) uniform cam {
    mat4 viewProj;
} camera;

struct Particle {
    vec2 pos;
    uint mode;
    float height;
};

layout(std140, set = 2, binding = 0) readonly buffer pcl {
    Particle particles[];
} particles;

layout(std140, set = 2, binding = 1) uniform gpinfo {
    float cameraToWindowScale;
} globalInfo;

struct ModeInfo {
    vec2 texCoordCenter;
    uint textureId;
    float radius;
};

layout(set = 2, binding = 2) uniform gpcl {
    ModeInfo modeInfo[3];
    float rotation;
} globals;

void main() {
    Particle particle = particles.particles[gl_InstanceIndex];

    vec2 pos = particle.pos;
    mat4 particleTransform = mat4(1.0);
    vec2 heightAdjust = vec2(0.0);
    if (particle.height > 0.0) {
        heightAdjust = vec2(particle.height * 0.25, -particle.height * 0.5);
    }
    particleTransform[3][0] = pos[0] + heightAdjust.x;
    particleTransform[3][1] = pos[1] + heightAdjust.y;
    vec4 worldPos = particleTransform * vec4(inPosition, 1.0);
	gl_Position = camera.viewProj * worldPos;

    float alpha = 1.0;
    if (particle.height <= -0.3) {
        float p = (-particle.height - 0.3) / 0.25;
        alpha = max(1.0 - p, 0.0);
    }

    ModeInfo info = globals.modeInfo[particle.mode];
    gl_PointSize = info.radius * globalInfo.cameraToWindowScale;
    fragTexCoords = info.texCoordCenter;
    fragTextureId = info.textureId;
    fragPos = worldPos.xy;
    fragColor = vec4(1.0, 1.0, 1.0, alpha);
    rotation = globals.rotation;
}
