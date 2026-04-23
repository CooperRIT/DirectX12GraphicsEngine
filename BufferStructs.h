#pragma once

#include <DirectXMath.h>

// Root constants for bindless resources
struct RayTracingDrawData
{
	unsigned int SceneDataConstantBufferIndex;
	unsigned int EntityDataDescriptorIndex;
	unsigned int SceneTLASDescriptorIndex;
	unsigned int OutputUAVDescriptorIndex;
	unsigned int SkyboxDescriptorIndex;
};

// Overall scene data for ray tracing (constant buffer)
struct RayTracingSceneData
{
	DirectX::XMFLOAT4X4 InverseViewProjection;
	DirectX::XMFLOAT3 CameraPosition;
	unsigned int RaysPerPixel;
};

// Per-entity informatiom like geometry and materials
struct RayTracingEntityData
{
	DirectX::XMFLOAT4 Color;
	unsigned int VertexBufferDescriptorIndex;
	unsigned int IndexBufferDescriptorIndex;
	unsigned int AlbedoIndex;
	unsigned int NormalMapIndex;
	unsigned int RoughnessIndex;
	unsigned int MetalnessIndex;
	float Roughness;
	float Metalness;
};