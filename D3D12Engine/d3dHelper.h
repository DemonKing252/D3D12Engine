#pragma once
#include <d3d12.h>
#include <string>
#include "UploadHeap.h"
#include <DirectXMath.h>
class d3dHelper
{
public:
	template <class T>
	static D3D12_VERTEX_BUFFER_VIEW VertexBuffer(UploadBuffer<T>* vertexBuffer, const UINT& size);

	template <class T>
	static D3D12_INDEX_BUFFER_VIEW IndexBuffer(UploadBuffer<T>* indexBuffer, const UINT& size);

};

struct Material
{
	std::string Name;
	DirectX::XMFLOAT4 DiffuseAlbedo;
};

template<class T>
D3D12_VERTEX_BUFFER_VIEW d3dHelper::VertexBuffer(UploadBuffer<T>* vertexBuffer, const UINT& size)
{
	D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view;
	ZeroMemory(&vertex_buffer_view, sizeof(D3D12_VERTEX_BUFFER_VIEW));
	vertex_buffer_view.BufferLocation = vertexBuffer->GetAddress();
	vertex_buffer_view.StrideInBytes = sizeof(T);
	vertex_buffer_view.SizeInBytes = size;

	return vertex_buffer_view;
}

template<class T>
inline D3D12_INDEX_BUFFER_VIEW d3dHelper::IndexBuffer(UploadBuffer<T>* indexBuffer, const UINT& size)
{
	D3D12_INDEX_BUFFER_VIEW index_buffer_view;
	ZeroMemory(&index_buffer_view, sizeof(D3D12_INDEX_BUFFER_VIEW));
	index_buffer_view.BufferLocation = indexBuffer->GetAddress();
	index_buffer_view.Format = DXGI_FORMAT_R32_UINT;
	index_buffer_view.SizeInBytes = size;

	return index_buffer_view;
}
