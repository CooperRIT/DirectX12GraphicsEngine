#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include "Lights.h"

#include <memory>
#include <vector>
#include "Camera.h"
#include "GameEntity.h"
#include "BufferStructs.h"

class Game
{
public:
	// Basic OOP setup
	Game();
	~Game();
	Game(const Game&) = delete; // Remove copy constructor
	Game& operator=(const Game&) = delete; // Remove copy-assignment operator

	// Primary functions
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	void OnResize();

private:
	void Initalize();

	// Buffers to hold actual geometry data
	// Pipeline
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	// Geometry
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
	D3D12_INDEX_BUFFER_VIEW ibView{};
	// Other graphics data
	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissorRect{};

	//Player Variabes
	std::shared_ptr<FPSCamera> camera;
	//std::shared_ptr<Mesh> sphereMesh;
	//std::shared_ptr<GameEntity> sphereEntity;
	std::vector<std::shared_ptr<GameEntity>> entities;
	//int lightCount;
	//std::vector<Light> lights;
};

