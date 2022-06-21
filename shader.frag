#version 330 core

in vec3 cameraLightPos;
in vec3 world_pos;
in vec3 v_normal;
uniform vec3 camPos;
out vec4 FragColor;

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
  float roughness;
  float metallic;
  float NoV;
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
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


vec3 CookTorranceBRDF(float D, vec3 F, float G, float NoL, float NoV) {
  vec3 brdf = D * F * G / (4 * NoL * NoV);
  return brdf;
}

vec3 LambertDiffuse(vec3 kD, vec3 albedoFrag)
{
  return kD * albedoFrag / PI;
}

// ------ GLTF2 SPEC IMPLEMENTATION START -----

vec3 conductor_fresnel(vec3 f0, vec3 bsdf, float HoV) {
  return bsdf * (f0 + (1 - f0) * (1 - abs(HoV)^5)
}

vec3 diffuse_brdf(vec3 albedo)
{
  return albedo / M_PI;
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

vec3 metal_brdf(float perceptualRoughness, vec3 baseColor, float D, float Visibility) {
  float alphaRoughness = perceptualRoughness * perceptualRoughness;
  return conductor_fresnel(baseColor, specular_brdf(D, Visibility));
}

vec3 dielectric_brdf(float ior, vec3 baseColor, float perceptualRoughness, float D, float Visibility, float HoV) {
  vec3 base = diffuse_brdf(baseColor);
  float alphaRoughness = perceptualRoughness * perceptualRoughness;
  vec3 layer = vec3(specular_brdf(D, Visibility));
  return fresnel_mix(ior, base, layer, HoV);
}
vec3 GltfSpecBRDFPoint(PointLight light, PBRInfo pbrInfo)
{
  vec3 L = normalize(light.position - pbrInfo.WP);
  vec3 H = normalize(pbrInfo.V + L);
  float HoV = clamp(dot(H, pbrInfo.V), 0.0, 1.0);
  float NoH = clamp(dot(pbrInfo.N, H), 0.0, 1.0);
  float VoH = clamp(dot(pbrInfo.V, H), 0.0, 1.0);
  float NoL = clamp(dot(pbrInfo.N, L), 0.001, 1.0);

  // Calculate radiance
  float distance = length(light.position.xyz - pbrInfo.WP);
  float attenuation = 1.0 / (distance * distance);
  vec3 radiance = light.color.xyz * attenuation; 

  // Calculate D, G, F
  float D = DistributionGGX(pbrInfo.N, H, pbrInfo.roughness);   
  float Visibility = ComputeVisibility(
}

// ------- GLTF2 SPEC IMPLEMENTATION END ---- //


vec3 CookTorrBRDFPoint(PointLight light, PBRInfo pbrInfo)
{
  vec3 L = normalize(light.position - pbrInfo.WP);
  vec3 H = normalize(pbrInfo.V + L);
  float HoV = clamp(dot(H, pbrInfo.V), 0.0, 1.0);
  float NoH = clamp(dot(pbrInfo.N, H), 0.0, 1.0);
  float VoH = clamp(dot(pbrInfo.V, H), 0.0, 1.0);
  float NoL = clamp(dot(pbrInfo.N, L), 0.001, 1.0);

  // Calculate radiance
  float distance = length(light.position.xyz - pbrInfo.WP);
  float attenuation = 1.0 / (distance * distance);
  vec3 radiance = light.color.xyz * attenuation; 

  // Calculate D, G, F
  float D = DistributionGGX(pbrInfo.N, H, pbrInfo.roughness);   
  float G   = GeometrySmith(pbrInfo.N, pbrInfo.V, L, pbrInfo.roughness);      
  vec3 F = fresnelSchlick(VoH, pbrInfo.F0);

  // Calculate ks and kD
  vec3 kS = F;
  vec3 kD = vec3(1.0) - kS;
  kD *= 1.0 - pbrInfo.metallic;	  

  // Calculate output color
  vec3 lambertianDiffuse = LambertDiffuse(kD, pbrInfo.albedo); 
  vec3 brdfTerm = CookTorranceBRDF(D, F, G, NoL, pbrInfo.NoV);
  vec3 outputColor = lambertianDiffuse + brdfTerm * radiance * NoL;
  return outputColor;
}

PBRInfo generatePBRInfo(PBRInfo pbrInfo) {
  float hardCodedMetallicFactor = 0.5f;
  vec3 albedoFactor = vec3(1.0, 0.766, 0.336);
  float roughnessFactor = 0.1f;
  vec3 derived_v = normalize(camPos - world_pos);

  pbrInfo.WP = world_pos;
  pbrInfo.albedo = albedoFactor;
  pbrInfo.metallic = hardCodedMetallicFactor;

  vec3 fresnel = vec3(0.04); 
  fresnel = mix(fresnel, pbrInfo.albedo, vec3(pbrInfo.metallic)); // ???????

  pbrInfo.F0 = fresnel;
  pbrInfo.N = v_normal;
  pbrInfo.V = derived_v;
  pbrInfo.NoV = clamp(dot(pbrInfo.N, pbrInfo.V), 0.0, 1.0);
  pbrInfo.roughness = roughnessFactor;
  return pbrInfo;
}

PointLight generatePointLight(PointLight light) {
  light.position = cameraLightPos;
  light.color = vec3(1.0f, 1.0f, 1.0f);
  return light;
}

void main()
{
    PBRInfo pbrInfo;
    PointLight light;

    light = generatePointLight(light);
    pbrInfo = generatePBRInfo(pbrInfo);
    vec3 color = CookTorrBRDFPoint(light, pbrInfo);
    FragColor = vec4(color, 1.0f);
}
