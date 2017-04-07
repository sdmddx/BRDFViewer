#pragma once

#include "stdafx.h"

namespace DX
{

#define SAFERELEASE(p) {if (p) {p->Release();} p = nullptr;}

	// 将使用与设备无关的像素(DIP)表示的长度转换为使用物理像素表示的长度。
	inline float ConvertDipsToPixels(float dips, float dpi)
	{
		static const float dipsPerInch = 96.0f;
		return floorf(dips * dpi / dipsPerInch + 0.5f); // 舍入到最接近的整数。
	}

#if defined(_DEBUG)
	// 请检查 SDK 层支持。
	inline bool SdkLayersAvailable()
	{
		HRESULT hr = D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_NULL,       // 无需创建实际硬件设备。
			0,
			D3D11_CREATE_DEVICE_DEBUG,  // 请检查 SDK 层。
			nullptr,                    // 任何功能级别都会这样。
			0,
			D3D11_SDK_VERSION,          // 对于 Windows 应用商店应用，始终将此值设置为 D3D11_SDK_VERSION。
			nullptr,                    // 无需保留 D3D 设备引用。
			nullptr,                    // 无需知道功能级别。
			nullptr                     // 无需保留 D3D 设备上下文引用。
		);

		return SUCCEEDED(hr);
	}

#endif
}

