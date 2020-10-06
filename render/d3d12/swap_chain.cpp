#include "swap_chain.h"
#include "d3d12_main.h"

namespace D3D12
{
    void SwapChain::Init(HWND window, UINT backbuffer_count)
    {
        RenderSystem* rs = RenderSystem::Get();

        BackBufferCount = backbuffer_count;

        // Describe and create the swap chain.
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.BufferCount = BackBufferCount;
        swapChainDesc.Width = 1280;
        swapChainDesc.Height = 720;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.SampleDesc.Count = 1;

        ComPtr<IDXGISwapChain1> swapChain;
        D3DCHECKRESULT(rs->GetDXGIFactory()->CreateSwapChainForHwnd(
            rs->GetGraphicsCommandQueue(),        // Swap chain needs the queue so that it can force a flush on it.
            window,
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapChain
        ));

        // This sample does not support fullscreen transitions.
        D3DCHECKRESULT(rs->GetDXGIFactory()->MakeWindowAssociation(window, DXGI_MWA_NO_ALT_ENTER));

        D3DCHECKRESULT(swapChain.As(&DXGISwapChain));

#if DEBUG
        wchar_t window_class_name[256] = L"Window Class: ";
        RealGetWindowClassW(window, window_class_name + sizeof("Window Class: ") - 1, (UINT)ARRAYCOUNT(window_class_name) - sizeof("Window Class: "));
        SetDebugName(DXGISwapChain.Get(), window_class_name);
#endif

        // Create frame resources.
        RenderTargets = eastl::make_unique<RenderTarget[]>(BackBufferCount);

        // Create a RTV for each frame.
        DescriptorHeap::Iterator rtv_handle_it = rs->GetRTVDescriptorHeap()->begin();
        char debug_name[] = "Main Render Target 0";
        for (UINT n = 0; n < BackBufferCount; n++)
        {
            D3DCHECKRESULT(DXGISwapChain->GetBuffer(n, IID_PPV_ARGS(&RenderTargets[n].D3DResource)));

            rs->GetDevice()->CreateRenderTargetView(RenderTargets[n].D3DResource.Get(), nullptr, *rtv_handle_it);
            ++rtv_handle_it;

            SetDebugName(RenderTargets[n].D3DResource.Get(), debug_name);
            ++debug_name[sizeof(debug_name) - 2];
        }

        CurBackBufferIndex = DXGISwapChain->GetCurrentBackBufferIndex();
    }

    void SwapChain::Shutdown()
    {
        // TODO: Clean up state
    }

    [[nodiscard]] D3D12_RESOURCE_BARRIER SwapChain::GetRenderTargetBarrier(D3D12_RESOURCE_STATES new_state)
    {
        RenderTarget* r = &RenderTargets[CurBackBufferIndex];
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(r->D3DResource.Get(), r->State, new_state);
        r->State = new_state;
        return barrier;
    }
}