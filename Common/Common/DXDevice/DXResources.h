#pragma once

using namespace std;
using namespace DirectX;
namespace DX
{
	// �ṩһ���ӿڣ���ʹӵ�� DXResources ��Ӧ�ó������豸��ʧ�򴴽�ʱ�յ�֪ͨ��
	class IDeviceNotify
	{
	public:
		virtual void OnDeviceLost() = 0;
		virtual void OnDeviceRestored() = 0;
	};

	class COMMON_API DXResources
	{
		 
	public:
		DXResources();
		void SetOutputWindow(HWND hwnd);
		Math::Size GetWindowSize();
		/*
		void SetLogicalSize(Math::Size logicalSize);
		void SetDpi(float dpi);
		void SetCompositionScale(float compositionScaleX, float compositionScaleY);
		void ValidateDevice();
		void Trim();
		*/
		void HandleDeviceLost();
		void RegisterDeviceNotify(IDeviceNotify* deviceNotify);
		void Present();

		// ������Ŀ��Ĵ�С���� dip Ϊ��λ��
		Math::Size					GetLogicalSize() const { return m_logicalSize; }

		// D3D ��������
		ID3D11Device3*				GetD3DDevice() const { return m_d3dDevice.Get(); }
		ID3D11DeviceContext3*		GetD3DDeviceContext() const { return m_d3dContext.Get(); }
		IDXGISwapChain3*			GetSwapChain() const { return m_swapChain.Get(); }
		D3D_FEATURE_LEVEL			GetDeviceFeatureLevel() const { return m_d3dFeatureLevel; }
		ID3D11RenderTargetView1*	GetBackBufferRenderTargetView() const { return m_d3dRenderTargetView.Get(); }
		ID3D11DepthStencilView*		GetDepthStencilView() const { return m_d3dDepthStencilView.Get(); }
		D3D11_VIEWPORT				GetScreenViewport() const { return m_screenViewport; }

		// D2D ��������
		ID2D1Factory3*				GetD2DFactory() const { return m_d2dFactory.Get(); }
		ID2D1Device2*				GetD2DDevice() const { return m_d2dDevice.Get(); }
		ID2D1DeviceContext2*		GetD2DDeviceContext() const { return m_d2dContext.Get(); }
		ID2D1Bitmap1*				GetD2DTargetBitmap() const { return m_d2dTargetBitmap.Get(); }
		IDWriteFactory3*			GetDWriteFactory() const { return m_dwriteFactory.Get(); }
		IWICImagingFactory2*		GetWicImagingFactory() const { return m_wicFactory.Get(); }
		D2D1::Matrix3x2F			GetOrientationTransform2D() const { return m_orientationTransform2D; }

	private:
		void CreateDeviceIndependentResources();
		void CreateDeviceResources();
		void CreateWindowSizeDependentResources(Math::Size &s);
		void UpdateRenderTargetSize(Math::Size &s);

		// Direct3D ����
		Microsoft::WRL::ComPtr<ID3D11Device3>			m_d3dDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext3>	m_d3dContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain3>			m_swapChain;

		// Direct3D ��Ⱦ����3D ������ġ�
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView1>	m_d3dRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	m_d3dDepthStencilView;
		D3D11_VIEWPORT									m_screenViewport;

		// Direct2D ���������
		Microsoft::WRL::ComPtr<ID2D1Factory3>			m_d2dFactory;
		Microsoft::WRL::ComPtr<ID2D1Device2>			m_d2dDevice;
		Microsoft::WRL::ComPtr<ID2D1DeviceContext2>		m_d2dContext;
		Microsoft::WRL::ComPtr<ID2D1Bitmap1>			m_d2dTargetBitmap;

		// DirectWrite ���������
		Microsoft::WRL::ComPtr<IDWriteFactory3>			m_dwriteFactory;
		Microsoft::WRL::ComPtr<IWICImagingFactory2>		m_wicFactory;


		// ������豸���ԡ�
		D3D_FEATURE_LEVEL								m_d3dFeatureLevel;
		Math::Size										m_d3dRenderTargetSize;
		Math::Size										m_logicalSize;

		// ������ʾ�����ת����
		D2D1::Matrix3x2F	m_orientationTransform2D;
		DirectX::XMFLOAT4X4	m_orientationTransform3D;

		// IDeviceNotify ��ֱ�ӱ�������Ϊ��ӵ�� DXResources��
		IDeviceNotify* m_deviceNotify;
	
	private:
		HWND m_hwnd;

	};
}

