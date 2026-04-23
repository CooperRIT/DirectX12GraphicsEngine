#include "Lighting.hlsli"

cbuffer ExternalData : register(b0)
{
    uint albedoIndex;
    uint normalMapIndex;
    uint roughnessIndex;
    uint metalnessIndex;
    float2 uvScale;
    float2 uvOffset;
    float3 cameraPosition;
    int lightCount;
    Light lights[MAX_LIGHTS];
}

struct VertexToPixel
{
    float4 screenPosition : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 worldPos : POSITION;
};

SamplerState BasicSampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
    Texture2D AlbedoTexture = ResourceDescriptorHeap[albedoIndex];
    Texture2D NormalMap = ResourceDescriptorHeap[normalMapIndex];
    Texture2D RoughnessMap = ResourceDescriptorHeap[roughnessIndex];
    Texture2D MetalMap = ResourceDescriptorHeap[metalnessIndex];

    // Clean up un-normalized normals
    input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent);

    // Scale and offset uv as necessary
    input.uv = input.uv * uvScale + uvOffset;

    // Sample normal map raw to verify it looks correct
    float3 normalSample = NormalMap.Sample(BasicSampler, input.uv).rgb;
    
    // Only apply normal mapping if the sample looks valid (mostly blue = tangent space)
    // A valid normal map should have a strong blue channel
    float3 mappedNormal = input.normal;
    if (normalSample.b > 0.3f)
    {
        mappedNormal = NormalMapping(NormalMap, BasicSampler, input.uv, input.normal, input.tangent);
    }

    // Surface color with gamma correction
    float4 surfaceColor = AlbedoTexture.Sample(BasicSampler, input.uv);
    surfaceColor.rgb = pow(surfaceColor.rgb, 2.2);

    // Sample the other maps
    float roughness = RoughnessMap.Sample(BasicSampler, input.uv).r;
    float metal = MetalMap.Sample(BasicSampler, input.uv).r;

    // Specular color
    float3 specColor = lerp(F0_NON_METAL.rrr, surfaceColor.rgb, metal);

    // Keep a running total of light
    float3 totalLight = float3(0, 0, 0);
    
    // Add before the loop
    float3 ambient = float3(0.1f, 0.1f, 0.1f) * surfaceColor.rgb;

    // Loop and handle all lights
    for (int i = 0; i < lightCount; i++)
    {
        Light light = lights[i];
        light.Direction = normalize(light.Direction);

        switch (lights[i].Type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                totalLight += DirLightPBR(light, mappedNormal, input.worldPos, cameraPosition, roughness, metal, surfaceColor.rgb, specColor, 0);
                break;
            case LIGHT_TYPE_POINT:
                totalLight += PointLightPBR(light, mappedNormal, input.worldPos, cameraPosition, roughness, metal, surfaceColor.rgb, specColor, 0);
                break;
            case LIGHT_TYPE_SPOT:
                totalLight += SpotLightPBR(light, mappedNormal, input.worldPos, cameraPosition, roughness, metal, surfaceColor.rgb, specColor, 0);
                break;
        }
    }
    totalLight += ambient;

    // Gamma correct and return
    return float4(pow(totalLight, 1.0f / 2.2f), 1.0f);
}