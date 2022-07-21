#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 tex_coord;
layout (location = 3) in vec3 a_color;

uniform vec3 test_1;
in vec3 light_pos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 v_normal;
out vec3 world_pos;
out vec3 camPos;
out vec3 test1;
out vec2 texCoord;
out vec3 v_color;

void main() {
    world_pos = (model * vec4(aPos, 1)).xyz;
    v_normal = (model * vec4(a_normal, 0)).xyz;
    camPos = light_pos;
    test1 = test_1;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    texCoord = tex_coord;
    v_color = a_color;
}
