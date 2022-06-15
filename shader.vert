#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 a_normal;
out vec3 ourColor;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 v_normal;
in vec3 v_pos;
out vec3 v_position;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    v_position = (model * vec4(aPos, 1)).xyz;
    v_normal = (model * vec4(a_normal, 0)).xyz;
    v_position = v_pos;
}
