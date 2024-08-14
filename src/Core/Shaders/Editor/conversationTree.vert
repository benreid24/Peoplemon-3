#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoords;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoords;
layout(location = 2) flat out uint fragTextureId;
layout(location = 3) out vec2 fragPos;

layout(set = 1, binding = 0) uniform cam {
    mat4 viewProj;
} camera;

layout(std140, set = 2, binding = 0) readonly buffer obj {
    mat4 model[];
} object;
layout(std140, set = 2, binding = 1) readonly buffer tex {
    uint index[];
} skin;

layout( set = 3, binding = 0) uniform TreeCamera {
    vec2 camCenter;
    vec2 componentSize;
    float zoom;
} tcam;

void main() {
    vec2 gcsHalf = tcam.componentSize * 0.5;

    mat4 vcam = mat4(1);
    vcam[3][0] = gcsHalf.x;
    vcam[3][1] = gcsHalf.y;

    mat4 zoom = mat4(1);
    zoom[0][0] = tcam.zoom;
    zoom[1][1] = tcam.zoom;

    mat4 move = mat4(1);
    move[3][0] = -tcam.camCenter.x;
    move[3][1] = -tcam.camCenter.y;

    vec4 localPos = vcam * zoom * move * vec4(inPosition, 1.0);
    vec4 worldPos = object.model[gl_InstanceIndex] * localPos;

    gl_Position = camera.viewProj * worldPos;
    fragColor = inColor;
	fragTexCoords = inTexCoords;
    fragTextureId = skin.index[gl_InstanceIndex];
    fragPos = vec2(worldPos.x, worldPos.y);
}
