#pragma once

#include <d3d12.h>
#include <wrl/client.h>

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

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void CreateRootSigAndPipelineState();
	void CreateGeometry();
	void Initalize();

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

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
	std::vector<std::shared_ptr<GameEntity>> entities;

	//Asset Path
};

