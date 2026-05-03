#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include <string>

#include "Vertex.h"


class Mesh
{
public:
	Mesh(const char* name, Vertex* vertArray, size_t numVerts, unsigned int* indexArray, size_t numIndices);
	Mesh(const char* name, const std::wstring& objFile);
	~Mesh();

	// Getters for mesh data
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();
	D3D12_GPU_DESCRIPTOR_HANDLE GetVertexBufferDescriptorHandle();
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();
	const char* GetName();
	unsigned int GetIndexCount();
	unsigned int GetVertexCount();

private:
	//Views
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;


	// D3D buffers
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;


	D3D12_GPU_DESCRIPTOR_HANDLE vbGPUDescriptorHandle;

	// Total indices & vertices in this mesh
	unsigned int numIndices;
	unsigned int numVertices;

	// Name (mostly for UI purposes)
	const char* name;

	// Helper for creating buffers (in the event we add more constructor overloads)
	void CreateBuffers(Vertex* vertArray, size_t numVerts, unsigned int* indexArray, size_t numIndices);

};

