#pragma once

#include "main/common.h"
#include "platform/win32/win32_platform.h"
#include <combaseapi.h>
#define COM_NO_WINDOWS_H
#include <d3d12.h>
#include <dxgi1_6.h>
#include "thirdparty/D3DX12/d3dx12.h"
#include "thirdparty/D3D12MemoryAllocator/D3D12MemAlloc.h"
#include <wrl/client.h>
#include <EASTL/string_view.h>

using Microsoft::WRL::ComPtr;

#ifdef NDEBUG
#define D3DCHECKRESULT(expression) expression
#else
#define D3DCHECKRESULT(expression) HRESULT TM_CONCAT(res, __LINE__) = expression; TM_ASSERT(SUCCEEDED(TM_CONCAT(res, __LINE__)) && #expression)
#endif

namespace D3D12
{
#ifdef DEBUG
	TM_INLINE void SetDebugName(IDXGIObject* pObject, eastl::wstring_view name) {
		pObject->SetPrivateData(WKPDID_D3DDebugObjectNameW, UINT(name.length() + 1) * 2, name.data());
	}
	TM_INLINE void SetDebugName(IDXGIObject* pObject, eastl::string_view name) {
		pObject->SetPrivateData(WKPDID_D3DDebugObjectName, UINT(name.length() + 1), name.data());
	}
	TM_INLINE void SetDebugName(ID3D12Object* pObject, eastl::wstring_view name) {
		pObject->SetName(name.data());
	}
	TM_INLINE void SetDebugName(ID3D12Object* pObject, eastl::string_view name) {
		pObject->SetPrivateData(WKPDID_D3DDebugObjectName, UINT(name.length() + 1), name.data());
	}
#else
	TM_INLINE void SetDebugName(IDXGIObject*, eastl::wstring_view) {}
	TM_INLINE void SetDebugName(IDXGIObject*, eastl::string_view) {}
	TM_INLINE void SetDebugName(ID3D12Object*, eastl::wstring_view) {}
	TM_INLINE void SetDebugName(ID3D12Object*, eastl::string_view) {}
#endif

	using DXGIFactoryInterface = IDXGIFactory7; // introduced in 1809
	using DXGIAdapterInterface = IDXGIAdapter4; // introduced in 1703
	using D3D12DeviceInterface = ID3D12Device6; // introduced in 1903?
	using D3D12GraphicsCommandListInterface = ID3D12GraphicsCommandList5; // introduced in 1903?

	static constexpr uint32 BackBufferCount = 2;
	static constexpr D3D_FEATURE_LEVEL MinFeatureLevel = D3D_FEATURE_LEVEL_11_0;
}