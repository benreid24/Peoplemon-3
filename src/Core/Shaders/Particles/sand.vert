#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoords;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoords;
layout(location = 2) flat out uint fragTextureId;
layout(location = 3) out vec2 fragPos;
layout(location = 4) out flat float rotation;

layout(set = 1, binding = 0) uniform cam {
    mat4 viewProj;
} camera;

struct Particle {
    vec2 pos;
};

layout(std140, set = 2, binding = 0) readonly buffer pcl {
    Particle particles[];
} particles;

layout(std140, set = 2, binding = 1) uniform gpinfo {
    float cameraToWindowScale;
} globalInfo;

layout(set = 2, binding = 2) uniform gpcl {
    uint textureId;
    vec2 texCoordCenter;
    float alpha;
    float radius;
} globals;

void main() {
    Particle particle = particles.particles[gl_InstanceIndex];

    vec2 pos = particle.pos;
    mat4 particleTransform = mat4(1.0);
    particleTransform[3][0] = pos[0];
    particleTransform[3][1] = pos[1];
    vec4 worldPos = particleTransform * vec4(inPosition, 1.0);
	gl_Position = camera.viewProj * worldPos;

    gl_PointSize = globals.radius * globalInfo.cameraToWindowScale;
    fragTexCoords = globals.texCoordCenter;
    fragTextureId = globals.textureId;
    fragPos = worldPos.xy;
    fragColor = vec4(1.0, 1.0, 1.0, globals.alpha);
    rotation = 0.0;
}
