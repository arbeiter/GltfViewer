#version 330 core
in vec2 texCoord;
in vec3 camPos;
in vec3 world_pos;
in vec3 v_normal;
in vec3 v_color;
in mat3 tbn;
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

#define NR_POINT_LIGHTS 4
struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform PointLight pointLights[NR_POINT_LIGHTS];

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

void main() {
  float gamma = 2.2;
  float min_roughness = 0.04;

  vec4 col = texture2D(samp_tex, texCoord);
  vec3 colrgb = col.rgb;

  vec3 baseColor = pow(colrgb, vec3(gamma));
  float roughness = texture2D(metallicTex, texCoord).g;
  float mFactor = texture2D(metallicTex, texCoord).b;
  vec3 lightColor = vec3(1.0, 1.0, 1.0);
  vec3 tempNormalVec = normalize(texture(normalTex, texCoord).rgb * 2.0 - 1.0);
  vec3 N = normalize(tbn * tempNormalVec);
  vec3 V = normalize(camPos - world_pos);
  vec3 f0 = mix(vec3(0.04), baseColor.rgb, mFactor);
	vec3 result = vec3(0,0,0);

  for(int i = 0; i < NR_POINT_LIGHTS; i++) {
    vec3 L = normalize(pointLights[i].position - world_pos);
    N = v_normal;
    float NdotL = max(dot(N, L), 0);
    vec3 H = normalize(L + V);
    vec3 f = fresnel_schlick(f0, H, V);

    float perceptual_roughness = clamp(roughness, min_roughness, 1.0);
    perceptual_roughness = perceptual_roughness * perceptual_roughness;
    float d = DistributionGGX(N, H, perceptual_roughness);
    float g = GeometrySmith(N, V, L, perceptual_roughness);

    vec3 specular = (d * g * f) / (4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001);
    vec3 diffuse = mix(baseColor.rgb - f, vec3(0.0), mFactor);

    vec3 temp = vec3(0,0,0);
    temp += NdotL * pointLights[0].diffuse * (diffuse + specular);
    temp += vec3(pointLights[0].ambient) * baseColor.rgb;
    result += temp;
  }

  result = pow(result, vec3(1.0/gamma));
  FragColor = vec4(result, col.w);
}
