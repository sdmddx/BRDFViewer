#pragma once

#include "stdafx.h"

namespace DX
{

#define SAFERELEASE(p) {if (p) {p->Release();} p = nullptr;}

	// ��ʹ�����豸�޹ص�����(DIP)��ʾ�ĳ���ת��Ϊʹ���������ر�ʾ�ĳ��ȡ�
	inline float ConvertDipsToPixels(float dips, float dpi)
	{
		static const float dipsPerInch = 96.0f;
		return floorf(dips * dpi / dipsPerInch + 0.5f); // ���뵽��ӽ���������
	}

#if defined(_DEBUG)
	// ���� SDK ��֧�֡�
	inline bool SdkLayersAvailable()
	{
		HRESULT hr = D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_NULL,       // ���贴��ʵ��Ӳ���豸��
			0,
			D3D11_CREATE_DEVICE_DEBUG,  // ���� SDK �㡣
			nullptr,                    // �κι��ܼ��𶼻�������
			0,
			D3D11_SDK_VERSION,          // ���� Windows Ӧ���̵�Ӧ�ã�ʼ�ս���ֵ����Ϊ D3D11_SDK_VERSION��
			nullptr,                    // ���豣�� D3D �豸���á�
			nullptr,                    // ����֪�����ܼ���
			nullptr                     // ���豣�� D3D �豸���������á�
		);

		return SUCCEEDED(hr);
	}

#endif
}

