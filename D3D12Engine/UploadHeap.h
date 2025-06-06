#pragma once
#include <DirectXMath.h>
#include <Windows.h>
#include <d3d12.h>
#include <dxgi.h>
#include <wrl.h>
#include <string>
#include <comdef.h>
#include <stdexcept>
#include <dxgi1_4.h>
#include "d3dx12.h"
#include <d3dcompiler.h>
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
using namespace DirectX;
using Microsoft::WRL::ComPtr;
struct PassConstants
{
	XMMATRIX World;
};

template <class T>
class UploadBuffer
{
public:
	UploadBuffer(ID3D12Device* device, T* data, size_t size);

	D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const;
	ComPtr<ID3D12Resource> GetBuffer() const;
	void CopyData(T* data);
	
private:
	size_t m_size;
	void* m_pBytes;
	ComPtr<ID3D12Resource> m_pUploadBuffer;
};

template<class T>
UploadBuffer<T>::UploadBuffer(ID3D12Device* device, T* data, size_t size)
{
	this->m_size = size;
	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(size),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&m_pUploadBuffer))
	);
	ThrowIfFailed(m_pUploadBuffer->Map(0, nullptr, &m_pBytes));
	CopyMemory(reinterpret_cast<UINT8*>(m_pBytes), data, size);
	m_pUploadBuffer->Unmap(0, nullptr);
}
template<class T>
D3D12_GPU_VIRTUAL_ADDRESS UploadBuffer<T>::GetAddress() const
{
	return m_pUploadBuffer->GetGPUVirtualAddress();
}

template<class T>
ComPtr<ID3D12Resource> UploadBuffer<T>::GetBuffer() const
{
	return m_pUploadBuffer;
}

template<class T>
void UploadBuffer<T>::CopyData(T* data)
{
	CopyMemory(m_pBytes, data, m_size);
}
