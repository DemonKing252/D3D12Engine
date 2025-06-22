#include "d3dHelper.h"
#include "D3D12Engine.h"

UINT d3dHelper::CBVSRVDescriptorSize(ComPtr<ID3D12Device> dev)
{
    return dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

CD3DX12_CPU_DESCRIPTOR_HANDLE d3dHelper::CPUHandleAt(ComPtr<ID3D12DescriptorHeap> heap, ComPtr<ID3D12Device> dev, UINT offset)
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE handle(heap->GetCPUDescriptorHandleForHeapStart());
    handle.Offset(offset, CBVSRVDescriptorSize(dev));

    return handle;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE d3dHelper::GPUHandleAt(ComPtr<ID3D12DescriptorHeap> heap, ComPtr<ID3D12Device> dev, UINT offset)
{
    CD3DX12_GPU_DESCRIPTOR_HANDLE handle(heap->GetGPUDescriptorHandleForHeapStart());
    handle.Offset(offset, CBVSRVDescriptorSize(dev));

    return handle;
}

