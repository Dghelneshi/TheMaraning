#include "command_queue.h"
#include "d3d12_main.h"

namespace D3D12
{
	void CommandQueue::Init(D3D12_COMMAND_LIST_TYPE type)
	{
		this->Type = type;

		auto device = RenderSystem::Get()->GetDevice();
		D3D12_COMMAND_QUEUE_DESC desc{};
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Type = type;
		D3DCHECKRESULT(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&D3DQueue)));
		D3D12::SetDebugName(D3DQueue.Get(), GetCommandListName(type, true));
	}
	void CommandQueue::Shutdown()
	{
		D3DQueue.Reset();
	}


	ID3D12CommandAllocator* CommandList::GetAllocator() const {
		return D3DCmdAllocator[D3D12::RenderSystem::Get()->GetCurrentFrameIndex()].Get();
	}

	void GraphicsCommandList::Init(D3D12_COMMAND_LIST_TYPE type, ID3D12PipelineState *init_state)
	{
		this->Type = type;
		auto device = RenderSystem::Get()->GetDevice();

		eastl::string debug_name(GetCommandListName(type, false));
		debug_name += " Allocator ";
		for (size_t i = 0; i < D3D12::BackBufferCount; i++)
		{
			char c = char('0' + i);
			D3DCHECKRESULT(device->CreateCommandAllocator(type, IID_PPV_ARGS(&D3DCmdAllocator[i])));
			D3D12::SetDebugName(D3DCmdAllocator[i].Get(), debug_name + c);
		}

		D3DCHECKRESULT(device->CreateCommandList(
			0, // node mask
			type,
			GetAllocator(),
			init_state,
			IID_PPV_ARGS(&D3DCmdList)));

		D3DGenericCmdList = (ID3D12CommandList*) D3DCmdList.Get();

		D3D12::SetDebugName(D3DCmdList.Get(), GetCommandListName(type, false));

	}

	void GraphicsCommandList::Shutdown()
	{
		D3DCmdList.Reset();
		for (size_t i = 0; i < D3D12::BackBufferCount; i++)
		{
			D3DCmdAllocator[i]->Reset();
			D3DCmdAllocator[i].Reset();
		}
	}
}