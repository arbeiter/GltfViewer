#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 a_normal;

in vec3 light_pos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 v_normal;
out vec3 world_pos;
out vec3 cameraLightPos;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    world_pos = (model * vec4(aPos, 1)).xyz;
    v_normal = (model * vec4(a_normal, 0)).xyz;
    cameraLightPos = light_pos;
}
