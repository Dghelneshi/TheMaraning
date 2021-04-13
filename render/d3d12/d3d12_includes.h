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

#if DEBUG
#define D3DCHECKRESULT(expression) HRESULT CONCAT(res, __LINE__); assert(SUCCEEDED(CONCAT(res, __LINE__) = expression))
#else
#define D3DCHECKRESULT(expression) expression
#endif

namespace D3D12
{
#ifdef DEBUG
	FORCE_INLINE void SetDebugName(IDXGIObject* pObject, eastl::wstring_view name) {
		pObject->SetPrivateData(WKPDID_D3DDebugObjectNameW, UINT(name.length() + 1) * 2, name.data());
	}
	FORCE_INLINE void SetDebugName(IDXGIObject* pObject, eastl::string_view name) {
		pObject->SetPrivateData(WKPDID_D3DDebugObjectName, UINT(name.length() + 1), name.data());
	}
	FORCE_INLINE void SetDebugName(ID3D12Object* pObject, eastl::wstring_view name) {
		pObject->SetName(name.data());
	}
	FORCE_INLINE void SetDebugName(ID3D12Object* pObject, eastl::string_view name) {
		pObject->SetPrivateData(WKPDID_D3DDebugObjectName, UINT(name.length() + 1), name.data());
	}
#else
	FORCE_INLINE void SetDebugName(IDXGIObject*, eastl::wstring_view) {}
	FORCE_INLINE void SetDebugName(IDXGIObject*, eastl::string_view) {}
	FORCE_INLINE void SetDebugName(ID3D12Object*, eastl::wstring_view) {}
	FORCE_INLINE void SetDebugName(ID3D12Object*, eastl::string_view) {}
#endif
}