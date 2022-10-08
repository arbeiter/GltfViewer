#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 tex_coord;
layout (location = 3) in vec3 in_tangent;

uniform vec3 light_pos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 MVI;

out vec4 eyeVec;
out vec3 v_normal;
out vec3 world_pos;
out vec3 camPos;
out vec2 texCoord;
out vec4 viewFragPos;
out mat4 vm;
out vec4 view_posn;

void main() {
  gl_Position = projection * view * model * vec4(aPos, 1.0);
  view_posn = view * vec4(aPos, 1.0f);
  world_pos = (model * vec4(aPos, 1)).xyz;
  camPos = light_pos;
  v_normal = mat3(model) * a_normal;
  texCoord = tex_coord;
  viewFragPos = view * model * vec4(aPos, 1.0f);
  eyeVec = normalize(-(view * vec4(aPos, 1.0f)));
  vm = view;
}