#version 400

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec3 i_color;
layout(location = 2) in vec3 i_normal;

uniform mat4 u_objectTransform;
uniform mat4 u_viewingTransform;

out vec3 fs_position;
out vec3 fs_color;
out vec3 fs_normal;

void main(void) {
    vec3 objectPosition = vec3(vec4(i_position, 1.0f) * u_objectTransform);
    gl_Position = vec4(objectPosition, 1.0f) * u_viewingTransform;
    fs_position = objectPosition;
    fs_color = i_color;
    fs_normal = vec3(vec4(i_position + i_normal, 1.0f) * u_objectTransform) - objectPosition;
}

