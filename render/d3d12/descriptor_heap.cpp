#include "descriptor_heap.h"
#include "d3d12_main.h"

namespace D3D12
{
	void DescriptorHeap::Init(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT count, bool shader_visible)
	{
		ID3D12Device* device = RenderSystem::Get()->GetDevice();

		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = type;
		desc.NumDescriptors = count;
		desc.Flags = shader_visible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		D3DCHECKRESULT(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&D3DHeap)));

		SetDebugName(D3DHeap.Get(), GetDescriptorHeapName(type));

		DescriptorSize = device->GetDescriptorHandleIncrementSize(type);
		DescriptorCount = count;
		Type = type;
		IsShaderVisible = shader_visible;
	}
	void DescriptorHeap::Shutdown()
	{
		D3DHeap.Reset();
	}
}