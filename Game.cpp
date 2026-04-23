#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include "BufferStructs.h"
#include "RayTracing.h"

#include <DirectXMath.h>

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

//Useful define from teacher's github
#define RandomRange(min, max) (float)rand() / RAND_MAX * (max - min) + min

// For the DirectX Math library
using namespace DirectX;

Game::Game()
{
	Initalize();
}


// --------------------------------------------------------
// Called once per program, the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
void Game::Initalize()
{
	// Check for DXR support and setup required API objects
	RayTracing::Initialize(
		Window::Width(),
		Window::Height(),
		FixPath(L"RayTracing.cso"));

	// Reserve a descriptor slot for ImGui's font texture
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
	Graphics::ReserveDescriptorHeapSlot(&cpuHandle, &gpuHandle);


	// Create the camera
	camera = std::make_shared<FPSCamera>(
		XMFLOAT3(0.0f, 0.0f, -5.0f),	// Position
		5.0f,					// Move speed
		0.002f,					// Look speed
		XM_PIDIV4,				// Field of view
		Window::AspectRatio(),  // Aspect ratio
		0.01f,					// Near clip
		100.0f,					// Far clip
		CameraProjectionType::Perspective);

	// Create materials
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState{};
	std::shared_ptr<Material> greyMat = std::make_shared<Material>(pipelineState, XMFLOAT3(0.5f, 0.5f, 0.5f));
	std::shared_ptr<Material> lightGreyMat = std::make_shared<Material>(pipelineState, XMFLOAT3(0.9f, 0.9f, 1));


	std::shared_ptr<Material> cobblestone = std::make_shared<Material>(pipelineState, XMFLOAT3(1, 1, 1));
	std::shared_ptr<Material> bronze = std::make_shared<Material>(pipelineState, XMFLOAT3(1, 1, 1));
	std::shared_ptr<Material> bricks = std::make_shared<Material>(pipelineState, XMFLOAT3(1, 1, 1));

	//Assign and load materials
	UINT cobblestoneAlbedo = Graphics::LoadTexture(FixPath(L"../../Assets/PBR/cobblestone_albedo.png").c_str());
	UINT cobblestoneNormals = Graphics::LoadTexture(FixPath(L"../../Assets/PBR/cobblestone_normals.png").c_str());
	UINT cobblestoneRoughness = Graphics::LoadTexture(FixPath(L"../../Assets/PBR/cobblestone_roughness.png").c_str());
	UINT cobblestoneMetal = Graphics::LoadTexture(FixPath(L"../../Assets/PBR/cobblestone_metal.png").c_str());

	UINT bronzeAlbedo = Graphics::LoadTexture(FixPath(L"../../Assets/PBR/bronze_albedo.png").c_str());
	UINT bronzeNormals = Graphics::LoadTexture(FixPath(L"../../Assets/PBR/bronze_normals.png").c_str());
	UINT bronzeRoughness = Graphics::LoadTexture(FixPath(L"../../Assets/PBR/bronze_roughness.png").c_str());
	UINT bronzeMetal = Graphics::LoadTexture(FixPath(L"../../Assets/PBR/bronze_metal.png").c_str());

	UINT bricksAlbedo = Graphics::LoadTexture(FixPath(L"../../Assets/PBR/bricks_albedo.jpg").c_str());
	UINT bricksNormals = Graphics::LoadTexture(FixPath(L"../../Assets/PBR/bricks_normals.jpg").c_str());
	UINT bricksRoughness = Graphics::LoadTexture(FixPath(L"../../Assets/PBR/bricks_roughness.jpg").c_str());
	UINT bricksMetal = Graphics::LoadTexture(FixPath(L"../../Assets/PBR/bricks_metal.jpg").c_str());

	cobblestone->SetAlbedoIndex(cobblestoneAlbedo);
	cobblestone->SetNormalMapIndex(cobblestoneNormals);
	cobblestone->SetRoughnessIndex(cobblestoneRoughness);
	cobblestone->SetMetalnessIndex(cobblestoneMetal);

	bronze->SetAlbedoIndex(bronzeAlbedo);
	bronze->SetNormalMapIndex(bronzeNormals);
	bronze->SetRoughnessIndex(bronzeRoughness);
	bronze->SetMetalnessIndex(bronzeMetal);

	bricks->SetAlbedoIndex(bricksAlbedo);
	bricks->SetNormalMapIndex(bricksNormals);
	bricks->SetRoughnessIndex(bricksRoughness);
	bricks->SetMetalnessIndex(bricksMetal);


	// Load mesh(es)
	std::shared_ptr<Mesh> cubeMesh = std::make_shared<Mesh>("Cube", FixPath(L"../../Assets/Meshes/cube.obj").c_str());
	std::shared_ptr<Mesh> helix = std::make_shared<Mesh>("Helix", FixPath(L"../../Assets/Meshes/helix.obj").c_str());
	std::shared_ptr<Mesh> sphereMesh = std::make_shared<Mesh>("Sphere", FixPath(L"../../Assets/Meshes/sphere.obj").c_str());

	// Floor
	std::shared_ptr<GameEntity> floor = std::make_shared<GameEntity>(cubeMesh, greyMat);
	floor->GetTransform()->SetScale(50);
	floor->GetTransform()->SetPosition(0, -51, 0);
	entities.push_back(floor);

	// Spinning helix
	std::shared_ptr<GameEntity> t = std::make_shared<GameEntity>(helix, lightGreyMat);
	t->GetTransform()->SetScale(2);
	t->GetTransform()->SetPosition(0, 3, 0);
	entities.push_back(t);

	for (int i = 0; i < 15; i++)
	{
		std::shared_ptr<Material> mat;
		float randMat = RandomRange(0, 1);
		if (randMat > 0.95f) mat = bronze;
		else if (randMat > 0.75f) mat = cobblestone;
		else if (randMat > 0.5f) mat = bronze;
		else if (randMat > 0.2f) mat = bricks;
		else mat = std::make_shared<Material>(
			pipelineState,
			XMFLOAT3(
				RandomRange(0.0f, 1.0f),
				RandomRange(0.0f, 1.0f),
				RandomRange(0.0f, 1.0f)),
			XMFLOAT2(1, 1),
			XMFLOAT2(0, 0),
			RandomRange(0.0f, 1.0f));

		float scale = RandomRange(0.25f, 1.0f);

		std::shared_ptr<GameEntity> sphereEnt = std::make_shared<GameEntity>(sphereMesh, mat);
		sphereEnt->GetTransform()->SetScale(scale);
		sphereEnt->GetTransform()->SetPosition(
			RandomRange(-6, 6),
			-1 + scale,
			RandomRange(-6, 6));

		entities.push_back(sphereEnt);
	}

	// Create the ray tracing entity data buffer now that we have a scene
	RayTracing::CreateEntityDataBuffer(entities);

	// Once we have all of the BLASs ready, we can make a TLAS
	RayTracing::CreateTopLevelAccelerationStructureForScene(entities);

	// Finalize any initialization and wait for the GPU
	// before proceeding to the game loop
	Graphics::CloseAndExecuteCommandList();
	Graphics::WaitForGPU();
	Graphics::ResetAllocatorAndCommandList(0);


	
}


// --------------------------------------------------------
// Clean up memory or objects created by this class
// 
// Note: Using smart pointers means there probably won't
//       be much to manually clean up here!
// --------------------------------------------------------
Game::~Game()
{
	// Wait for the GPU before we shut down
	Graphics::WaitForGPU();
}

// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	// Update the camera's projection to match the new size
	if (camera)
		camera->UpdateProjectionMatrix(Window::AspectRatio());

	// Resize raytracing output texture
	RayTracing::ResizeOutputUAV(Window::Width(), Window::Height());
}


// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();

	// Update scene elements
	camera->Update(deltaTime);

	// Rotate the helix
	entities[1]->GetTransform()->Rotate(deltaTime * 0.5f, deltaTime * 0.5f, deltaTime * 0.5f);

	// Move the sphere entities (skipping cube and torus)
	for (int i = 2; i < entities.size(); i++)
	{
		XMFLOAT3 pos = entities[i]->GetTransform()->GetPosition();
		switch (i % 2)
		{
		case 0:
			pos.x = sin((totalTime + i) * 0.4f) * 4;
			break;

		case 1:
			pos.z = sin((totalTime + i) * 0.4f) * 4;
			break;
		}
		entities[i]->GetTransform()->SetPosition(pos);
	}

}


// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Grab the current back buffer for this frame
	Microsoft::WRL::ComPtr<ID3D12Resource> currentBackBuffer = Graphics::BackBuffers[Graphics::SwapChainIndex()];

	// Prepare a resoruce barrier for various transitions below
	D3D12_RESOURCE_BARRIER rb = {};
	rb.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	rb.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	rb.Transition.pResource = currentBackBuffer.Get();
	rb.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	// Raytracing: Update the TLAS for the latest entity positions and then trace
	{
		RayTracing::CreateTopLevelAccelerationStructureForScene(entities);
		RayTracing::Raytrace(camera, currentBackBuffer);
	}

	// ImGui Render after all other scene objects
	{
		// The raytracing call above assumes we'll be presenting immediately afterwards,
		// which leaves the back buffer in the PRESENT state.  We'll need to transition
		// back to RENDER_TARGET so that ImGui can also render.  This is definitely
		// an extra step, and could be generalized by not automatically transitioning
		// to PRESENT at the end of raytracing.
		rb.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		rb.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		Graphics::CommandList->ResourceBarrier(1, &rb);

		// ImGui needs the descriptor heap (where its font texture lives) and the render target
		Graphics::CommandList->SetDescriptorHeaps(1, Graphics::CBVSRVDescriptorHeap.GetAddressOf());
		Graphics::CommandList->OMSetRenderTargets(1, &Graphics::RTVHandles[Graphics::SwapChainIndex()], true, 0);
	}

	// Present
	{
		// Transition back to present
		rb.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		rb.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		Graphics::CommandList->ResourceBarrier(1, &rb);

		// Must occur BEFORE present
		Graphics::CloseAndExecuteCommandList();

		// Present the current back buffer and move to the next one
		bool vsync = Graphics::VsyncState();
		Graphics::SwapChain->Present(
			vsync ? 1 : 0,
			vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING);
		Graphics::AdvanceSwapChainIndex();

		// Reset the command list & allocator for the upcoming frame
		Graphics::ResetAllocatorAndCommandList(Graphics::SwapChainIndex());

	}
}

