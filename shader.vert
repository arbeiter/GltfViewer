#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 tex_coord;
layout (location = 3) in vec3 in_tangent;

in vec3 light_pos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 MVI;

out mat3 tbn;
out vec3 v_normal;
out vec3 world_pos;
out vec3 camPos;
out vec2 texCoord;

void main() {
  world_pos = (model * vec4(aPos, 1)).xyz;
  v_normal = (MVI * a_normal);
  camPos = light_pos;
  gl_Position = projection * view * model * vec4(aPos, 1.0);

  vec3 tangent = normalize(MVI * in_tangent);
  vec3 bitangent = cross(v_normal, tangent);
  tbn = mat3(tangent, bitangent, v_normal);
  texCoord = tex_coord;
}
