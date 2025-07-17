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

inline std::string HRResultToString(HRESULT hr)
{
	_com_error err(hr);
	const wchar_t* wideMsg = err.ErrorMessage();

	// Convert to std::string (narrow) from wide
	char narrowMsg[512];
	size_t converted = 0;
	wcstombs_s(&converted, narrowMsg, wideMsg, _TRUNCATE);

	return std::string(narrowMsg);
}

#ifndef ThrowIfFailed
#define ThrowIfFailed(hrCall)                                      \
    {                                                              \
        HRESULT _hr = (hrCall);                                    \
        if (FAILED(_hr))                                           \
        {                                                          \
            throw std::runtime_error(HRResultToString(_hr).c_str());\
        }                                                          \
    }
#endif

struct MaterialConstant
{
	XMFLOAT4 DiffuseAlbedo;
};

struct FramePassConstants
{
	//XMMATRIX World;
	XMMATRIX ViewProj;
	//MaterialConstant Material;
};

struct ObjectPassConstants
{
	XMMATRIX World;
	MaterialConstant Material;
};

template <class T>
class UploadBuffer
{
public:
	UploadBuffer(ID3D12Device* device, T* data, const size_t& strideSize, const size_t& bufferSize);

	D3D12_GPU_VIRTUAL_ADDRESS GetAddress(const UINT& offsetInDescriptorHeap = 0) const;
	ComPtr<ID3D12Resource> GetBuffer() const;
	void CopyData(const UINT& offsetInDescriptorHeap, T* data);
	
private:
	size_t m_iBufferSize;
	void* m_pBytes;
	ComPtr<ID3D12Resource> m_pUploadBuffer;
};

template<class T>
UploadBuffer<T>::UploadBuffer(ID3D12Device* device, T* data, const size_t& strideSize, const size_t& bufferSize)
{
	this->m_iBufferSize = strideSize;
	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&m_pUploadBuffer))
	);
	
	ThrowIfFailed(m_pUploadBuffer->Map(0, nullptr, &m_pBytes));
	CopyMemory(
		reinterpret_cast<UINT8*>(m_pBytes), 
		data,
		m_iBufferSize
	);
	m_pUploadBuffer->Unmap(0, nullptr);
}
template<class T>
D3D12_GPU_VIRTUAL_ADDRESS UploadBuffer<T>::GetAddress(const UINT& offsetInDescriptorHeap) const
{
	return m_pUploadBuffer->GetGPUVirtualAddress() + offsetInDescriptorHeap * 256U;
}

template<class T>
ComPtr<ID3D12Resource> UploadBuffer<T>::GetBuffer() const
{
	return m_pUploadBuffer;
}

template<class T>
void UploadBuffer<T>::CopyData(const UINT& offsetInDescriptorHeap, T* data)
{
	CopyMemory(
		reinterpret_cast<UINT8*>(m_pBytes) + offsetInDescriptorHeap * 256U, 
		data, 
		m_iBufferSize
	);
}
