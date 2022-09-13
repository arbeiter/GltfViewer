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

out mat3 tbn;
out vec3 v_normal;
out vec3 world_pos;
out vec3 camPos;
out vec2 texCoord;

void main() {
  world_pos = (model * vec4(aPos, 1)).xyz;
  camPos = light_pos;
  gl_Position = projection * view * model * vec4(aPos, 1.0);

  vec3 N = normalize(MVI * a_normal);
  vec3 T = normalize(MVI * in_tangent);
  T = normalize(T - dot(T, N) * N);
  vec3 B = cross(N, T);
  tbn = transpose(mat3(T, B, N));
  texCoord = tex_coord;
  //  vs_out.TangentLightPos = TBN * lightPos;
  //  vs_out.TangentViewPos  = TBN * viewPos;
  //  vs_out.TangentFragPos  = TBN * vs_out.FragPos;
  // vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
  // vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
}
