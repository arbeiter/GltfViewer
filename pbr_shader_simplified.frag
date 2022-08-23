#version 330 core
in vec2 texCoord;
in vec3 camPos;
in vec3 world_pos;
in vec3 v_normal;
in vec3 v_color;
uniform vec3 base_color_provided;

uniform float metallicFactor;
uniform float roughFactor;

uniform sampler2D samp_tex;
uniform sampler2D metallicTex;
uniform sampler2D normalTex;

uniform bool isRoughFactorAbsent;
uniform bool isbaseColorTexturePresent;
uniform bool isMetallicTexturePresent;
uniform bool isNormalTexturePresent;

const float PI = 3.14159265359;
layout(location = 0) out vec4 FragColor;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float a = roughness*roughness;
    float a2 = a*a;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnel_schlick(vec3 f0, vec3 H, vec3 V) {
    return f0 + (1.0 - f0) * pow(1.0 - max(dot(H, V), 0.0), 5.0);
}

vec3 LinearSRGB(vec3 c) {
  float power = 2.2;
  return pow(c, vec3(power));
}

vec3 SRGBLinear(vec3 c) {
  float power = 1.0 /2.2;
  return pow(c, vec3(power));
}

void main() {
  float gamma = 2.2;

  vec3 baseColor = texture2D(samp_tex, texCoord).rgb;
  float roughness = texture2D(metallicTex, texCoord).g;
  float metallicFactor = texture2D(metallicTex, texCoord).b;
  vec3 lightColor = vec3(1.0, 1.0, 1.0);

  vec3 N = normalize(v_normal);
  vec3 L = normalize(camPos - world_pos);
  vec3 V = normalize(camPos - world_pos);
  vec3 H = normalize(L + V);

  // albedo
  vec3 f0 = mix(vec3(0.04), baseColor.rgb, metallicFactor);

  // lighting visibility / occlusion
  float d = DistributionGGX(N, H, roughness);
  float g = GeometrySmith(N, V, L, roughness);
  vec3 f = fresnel_schlick(f0, H, V);

  // specular and diffuse
  vec3 specular = d * g * f / (4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001);
  vec3 diffuse = mix(baseColor.rgb - f, vec3(0.0), metallicFactor);

  // result color
  vec3 result = (diffuse + specular) * lightColor * max(dot(N, L), 0.0);

  // ambient lighting
  result += vec3(0.33) * baseColor.rgb;

  // gamma correction
  FragColor = vec4(SRGBLinear(result), 1.0f);
}
