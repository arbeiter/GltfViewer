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

layout(location = 0) out vec4 FragColor;

struct PointLight {
  vec3 position;
  vec3 color;
};

struct PBRInfo {
  vec3 WP;      // world position.
  vec3 albedo;
  vec3 F0;
  vec3 N;
  vec3 V;
  vec3 baseColor;
  float roughness;
  float metallic;
  float NoV;
  vec3 eye_dir;
  vec3 half_vector;
};

const float PI = 3.14159265359;
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

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

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp((1.0 - abs(cosTheta)), 0.0, 1.0), 5.0);
}


// ------ GLTF2 SPEC IMPLEMENTATION START -----
vec3 LinearSRGB(vec3 c) {
  float power = 2.2;
  return pow(c, vec3(power));
}

vec3 SRGBLinear(vec3 c) {
  float power = 1.0 /2.2;
  return pow(c, vec3(power));
}

vec3 conductor_fresnel(vec3 f0, vec3 bsdf, float HoV) {
  return bsdf * (f0 + (1 - f0) * (1 - abs(int(HoV))^5));
}

vec3 diffuse_brdf(vec3 albedo)
{
  return (1/PI) * albedo;
}

vec3 fresnel_mix(float ior, vec3 base, vec3 layer, float VdotH) {
  float f0 = pow((1.0 - ior)/(1.0 + ior), 2.0);
  float fr = f0 + (1.0 - f0) * pow(1.0 - abs(VdotH), 5.0);
  return mix(base, layer, fr);
}

// cref: http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
// TODO: For now, uses D and V as inputs, should use: float alphaRoughness, float NdotL, float NdotV, float NdotH
float specular_brdf(float D, float visibilty) {
  return D * visibilty;
}

vec3 metal_brdf(float perceptualRoughness, vec3 baseColor, float D, float Visibility, float HoV) {
  float alphaRoughness = perceptualRoughness * perceptualRoughness;
  return conductor_fresnel(baseColor, vec3(specular_brdf(D, Visibility)), HoV);
}

vec3 dielectric_brdf(float ior, vec3 baseColor, float perceptualRoughness, float D, float Visibility, float HoV) {
  vec3 base = diffuse_brdf(baseColor);
  float alphaRoughness = perceptualRoughness * perceptualRoughness;
  vec3 layer = vec3(specular_brdf(D, Visibility));
  return fresnel_mix(ior, base, layer, HoV);
}

vec3 gltfSpecVersion(PointLight light, PBRInfo pbrInfo) {
  float dist = length(light.position.xyz - pbrInfo.WP);
  float attenuation = 1.0 / (dist * dist);

  // if metallicRoughnessTexture, set color to value
  vec3 baseColor = pbrInfo.baseColor;
  vec3 L = normalize(light.position - pbrInfo.WP);
  vec3 H = normalize(pbrInfo.V + L);

  float NoV = clamp(dot(pbrInfo.N, pbrInfo.V), 0.001, 1.0);
  float VoH = clamp(dot(pbrInfo.V, H), 0.0, 1.0);
  float NoL = clamp(dot(pbrInfo.N, L), 0.001, 1.0);
  float black = 1.0;

  float D = DistributionGGX(pbrInfo.N, H, pbrInfo.roughness);
  float G   = GeometrySmith(pbrInfo.N, pbrInfo.V, L, pbrInfo.roughness);
  vec3 F = fresnelSchlick(VoH, pbrInfo.F0);
  float visibility  = (G) / (4 * NoL * NoV);

  vec3 specular_brdf = (vec3(visibility * D) * F);
  vec3 metallic = specular_brdf * (baseColor + (1 - baseColor) * pow(1 - int(VoH), 5));

  vec3 c_diff = mix(baseColor.rgb, vec3(black), metallic);
  vec3 f_diffuse = (1 - F) * (1 / PI) * c_diff;
  vec3 f_specular = (F * D * G) / (4 * abs(NoV) * abs(NoL));
  vec3 material = f_diffuse * attenuation + f_specular * attenuation;
  return SRGBLinear(material);
}

PBRInfo generatePBRInfo(PBRInfo pbrInfo) {
  // baseColorFactor
  // metallicFactor
  // roughnessFactor
  vec3 derived_v = normalize(camPos.xyz - world_pos);

  pbrInfo.WP = world_pos;
  pbrInfo.albedo = vec3(1.0);

  vec3 fresnel = vec3(0.04);
  fresnel = mix(fresnel, pbrInfo.albedo, vec3(pbrInfo.metallic));

  pbrInfo.F0 = fresnel;
  pbrInfo.N = v_normal;
  pbrInfo.V = derived_v;
  pbrInfo.NoV = clamp(dot(pbrInfo.N, pbrInfo.V), 0.0, 1.0);
  return pbrInfo;
}

PointLight generatePointLight(PointLight light) {
  light.position = camPos;
  light.color = vec3(1.0f, 1.0f, 1.0f);
  return light;
}

void main() {
    PBRInfo pbrInfo;
    PointLight light;

    light = generatePointLight(light);
    pbrInfo = generatePBRInfo(pbrInfo);

    vec3 base_color = base_color_provided;
    if(isbaseColorTexturePresent) {
      vec3 temp = texture2D(samp_tex, texCoord).rgb;
      base_color = base_color.rgb * vec3(LinearSRGB(temp));
    }

    float r = 1.0f;
    float m = 1.0f;

    if(isMetallicTexturePresent) {
      vec4 metal_color = texture2D(metallicTex, texCoord);
      r = metal_color.g * roughFactor;
      m = metal_color.b * metallicFactor;
    } else {
      r = roughFactor;
      m = metallicFactor;
    }

    if(isRoughFactorAbsent) {
      r = 0.f;
      FragColor = vec4(vec3(0.0f, 1.0f, 0.0f), 1.0f);
      return;
    }

    pbrInfo.roughness = r;
    pbrInfo.metallic = m;

    pbrInfo.baseColor = vec3(base_color.rgb);
    vec3 color = gltfSpecVersion(light, pbrInfo);
    FragColor = vec4(SRGBLinear(color), 1.0f);
}
