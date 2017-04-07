#include "stdafx.h"


using namespace DX;
using namespace Microsoft::WRL;

DXResources::DXResources() :
	m_screenViewport(),
	m_d3dFeatureLevel(D3D_FEATURE_LEVEL_9_1),
	m_d3dRenderTargetSize(),
	m_logicalSize(),
	m_deviceNotify(nullptr)
{

	CreateDeviceIndependentResources();
	CreateDeviceResources();
}

//�����������
void DXResources::SetOutputWindow(HWND hwnd)
{
	m_hwnd = hwnd;
	RECT rect;
	GetClientRect(hwnd, &rect);
	Math::Size s;
	s.Height = rect.bottom - rect.top;
	s.Width = rect.right - rect.left;
	CreateWindowSizeDependentResources(s);
}


//�����豸�޹���Դ
void DXResources::CreateDeviceIndependentResources()
{
	// ��ʼ�� Direct2D ��Դ��
	D2D1_FACTORY_OPTIONS options;
	ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

#if defined(_DEBUG)
	// �����Ŀ���ڵ������ɽ׶Σ���ͨ�� SDK ������ Direct2D ���ԡ�
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

	// ��ʼ�� Direct2D ������
		D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED,
			__uuidof(ID2D1Factory3),
			&options,
			&m_d2dFactory
		
	);

	// ��ʼ�� DirectWrite ������
		DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory3),
			&m_dwriteFactory
		
	);

	// ��ʼ�� Windows ͼ�������(WIC)������
		CoCreateInstance(
			CLSID_WICImagingFactory2,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&m_wicFactory)	);

	


}

//�����豸��Դ
void DXResources::CreateDeviceResources()
{
	// �˱�־Ϊ��ɫͨ�������� API Ĭ�����ò�ͬ��ͼ��
	// ���֧�֡�Ҫ�� Direct2D ���ݣ����������Ҫ��
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
	if (DX::SdkLayersAvailable())
	{
		// �����Ŀ���ڵ������ɹ����У���ͨ�����д˱�־�� SDK �����õ��ԡ�
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}
#endif

	// �����鶨���Ӧ�ó���֧�ֵ� DirectX Ӳ�����ܼ����顣
	// ��ע�⣬Ӧ����˳��
	// �벻Ҫ������Ӧ�ó����˵���������������
	// ��͹��ܼ��𡣳�������˵��������ٶ�����Ӧ�ó����֧�� 9.1��
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	// ���� Direct3D 11 API �豸����Ͷ�Ӧ�������ġ�
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	HRESULT hr = D3D11CreateDevice(
		nullptr,					// ָ�� nullptr ��ʹ��Ĭ����������
		D3D_DRIVER_TYPE_HARDWARE,	// ����ʹ��Ӳ��ͼ������������豸��
		0,							// ӦΪ 0���������������� D3D_DRIVER_TYPE_SOFTWARE��
		creationFlags,				// ���õ��Ժ� Direct2D �����Ա�־��
		featureLevels,				// ��Ӧ�ó������֧�ֵĹ��ܼ�����б�
		ARRAYSIZE(featureLevels),	// ������б�Ĵ�С��
		D3D11_SDK_VERSION,			// ���� Windows Ӧ���̵�Ӧ�ã�ʼ�ս���ֵ����Ϊ D3D11_SDK_VERSION��
		&device,					// ���ش����� Direct3D �豸��
		&m_d3dFeatureLevel,			// �����������豸�Ĺ��ܼ���
		&context					// �����豸�ļ�ʱ�����ġ�
	);

	if (FAILED(hr))
	{
		// �����ʼ��ʧ�ܣ�����˵� WARP �豸��
			D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_WARP, // ���� WARP �豸������Ӳ���豸��
				0,
				creationFlags,
				featureLevels,
				ARRAYSIZE(featureLevels),
				D3D11_SDK_VERSION,
				&device,
				&m_d3dFeatureLevel,
				&context
			
		);
	}

	// ��ָ��洢�� Direct3D 11.3 API �豸�ͼ�ʱ�������С�
	device.As(&m_d3dDevice);

		context.As(&m_d3dContext);

	// ���� Direct2D �豸����Ͷ�Ӧ�������ġ�
	ComPtr<IDXGIDevice3> dxgiDevice;
		m_d3dDevice.As(&dxgiDevice);

		m_d2dFactory->CreateDevice(dxgiDevice.Get(), &m_d2dDevice);

		m_d2dDevice->CreateDeviceContext(
			D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
			&m_d2dContext
		);
}

// ÿ�θ��Ĵ��ڴ�Сʱ��Ҫ���´�����Щ��Դ
void DXResources::CreateWindowSizeDependentResources(Math::Size &s)
{
	// ����ض�����һ���ڴ�С�������ġ�
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	m_d3dContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
	m_d3dRenderTargetView = nullptr;
	m_d2dContext->SetTarget(nullptr);
	m_d2dTargetBitmap = nullptr;
	m_d3dDepthStencilView = nullptr;
	m_d3dContext->Flush1(D3D11_CONTEXT_TYPE_ALL, nullptr);

	UpdateRenderTargetSize(s);

	m_d3dRenderTargetSize.Height = m_logicalSize.Height;
	m_d3dRenderTargetSize.Width = m_logicalSize.Width;

	if (m_swapChain != nullptr)
	{
		// ����������Ѵ��ڣ���������С��
		HRESULT hr = m_swapChain->ResizeBuffers(
			2, // ˫���彻������
			lround(m_d3dRenderTargetSize.Width),
			lround(m_d3dRenderTargetSize.Height),
			DXGI_FORMAT_B8G8R8A8_UNORM,
			0
		);

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			// ��������κ�ԭ���Ƴ����豸������Ҫ����һ���µ��豸�ͽ�������
			HandleDeviceLost();

			// ����һ�ж���������ϡ���Ҫ����ִ�д˷�����HandleDeviceLost �����³��ִ˷��� 
			// ����ȷ�������豸��
			return;
		}
		return;
	}
	else
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
		
		swapChainDesc.Width = lround(m_d3dRenderTargetSize.Width);		// ƥ�䴰�ڵĴ�С��
		swapChainDesc.Height = lround(m_d3dRenderTargetSize.Height);
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;				// ������õĽ�������ʽ��
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1;								// �벻Ҫʹ�ö������
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;									// ʹ��˫�������̶ȵؼ�С�ӳ١�
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		swapChainDesc.Flags = 0;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		

		// �����л�ȡ������������� Direct3D �豸�� DXGI ������
		ComPtr<IDXGIDevice3> dxgiDevice;
			m_d3dDevice.As(&dxgiDevice);

		ComPtr<IDXGIAdapter> dxgiAdapter;
			dxgiDevice->GetAdapter(&dxgiAdapter);

		ComPtr<IDXGIFactory4> dxgiFactory;
			dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));

		//����������
		ComPtr<IDXGISwapChain1> swapChain;
			dxgiFactory->CreateSwapChainForHwnd(
				m_d3dDevice.Get(),
				m_hwnd,
				&swapChainDesc,
				nullptr,
				nullptr,
				&swapChain
			);

			swapChain.As(&m_swapChain);

		// ȷ�� DXGI ����һ�ζԶ��֡�Ŷӡ������ȿ��Լ�С�ӳ٣�
		// �ֿ���ȷ��Ӧ�ó���ֻ��ÿ�� VSync ֮����Ⱦ���Ӷ����̶ȵؽ��͹������ġ�
			dxgiDevice->SetMaximumFrameLatency(1);
	}

	// ������������̨����������ȾĿ����ͼ��
	ComPtr<ID3D11Texture2D1> backBuffer;
		m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

		m_d3dDevice->CreateRenderTargetView1(
			backBuffer.Get(),
			nullptr,
			&m_d3dRenderTargetView
		);

	// ������Ҫ�������� 3D ��Ⱦ�����ģ����ͼ��
	CD3D11_TEXTURE2D_DESC1 depthStencilDesc(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		lround(m_d3dRenderTargetSize.Width),
		lround(m_d3dRenderTargetSize.Height),
		1, // �����ģ����ͼֻ��һ������
		1, // ʹ�õ�һ mipmap ����
		D3D11_BIND_DEPTH_STENCIL
	);

	ComPtr<ID3D11Texture2D1> depthStencil;
		m_d3dDevice->CreateTexture2D1(
			&depthStencilDesc,
			nullptr,
			&depthStencil
		);

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
		m_d3dDevice->CreateDepthStencilView(
			depthStencil.Get(),
			&depthStencilViewDesc,
			&m_d3dDepthStencilView
		);

	// ��������ȷ���������ڵ� 3D ��Ⱦ������
	m_screenViewport = CD3D11_VIEWPORT(
		0.0f,
		0.0f,
		m_d3dRenderTargetSize.Width,
		m_d3dRenderTargetSize.Height
	);

	m_d3dContext->RSSetViewports(1, &m_screenViewport);

	
	// �����뽻������̨������������ Direct2D Ŀ��λͼ
	// ����������Ϊ��ǰĿ�ꡣ

	float dpiX, dpiY;
	m_d2dFactory->GetDesktopDpi(&dpiX, &dpiY);

	D2D1_BITMAP_PROPERTIES1 bitmapProperties =
	D2D1::BitmapProperties1(
	D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
	D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
		,dpiX, dpiY
	
	);

	ComPtr<IDXGISurface2> dxgiBackBuffer;
	m_swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));
	
	/*
	ID2D1HwndRenderTarget* pRenderTarget = NULL;   // Render target
	m_d2dFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), 
		D2D1::HwndRenderTargetProperties(m_hwnd, D2D1::SizeU(m_logicalSize.Width, m_logicalSize.Height)
			, D2D1_PRESENT_OPTIONS_NONE),
		&pRenderTarget);
		*/



	HRESULT hr=  m_d2dContext->CreateBitmapFromDxgiSurface(
	dxgiBackBuffer.Get(),
	&bitmapProperties,
	&m_d2dTargetBitmap
	);


	m_d2dContext->SetTarget(m_d2dTargetBitmap.Get());
	
	m_d2dContext->SetDpi(dpiX, dpiY);

	m_d2dContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
	
}

// ȷ��������Ŀ��ĳߴ�
void DXResources::UpdateRenderTargetSize(Math::Size &s)
{
	m_logicalSize.Width = s.Width;
	m_logicalSize.Height = s.Height;

}

// ���´��������豸��Դ���������ûص�ǰ״̬��
void DXResources::HandleDeviceLost()
{
	m_swapChain = nullptr;

	if (m_deviceNotify != nullptr)
	{
		m_deviceNotify->OnDeviceLost();
	}

	CreateDeviceResources();

	CreateWindowSizeDependentResources(m_logicalSize);

	if (m_deviceNotify != nullptr)
	{
		m_deviceNotify->OnDeviceRestored();
	}
}

// ע�����ǵ� DeviceNotify �Ա����豸��ʧ�ʹ���ʱ�յ�֪ͨ��
void DXResources::RegisterDeviceNotify(IDeviceNotify* deviceNotify)
{
	m_deviceNotify = deviceNotify;
}

// ����������������ʾ����Ļ�ϡ�
void DXResources::Present()
{
	// ��һ������ָʾ DXGI ������ֱֹ�� VSync����ʹӦ�ó���
	// ����һ�� VSync ǰ�������ߡ��⽫ȷ�����ǲ����˷��κ�������Ⱦ
	// �Ӳ�������Ļ����ʾ��֡��
	DXGI_PRESENT_PARAMETERS parameters = { 0 };
	HRESULT hr = m_swapChain->Present1(1, 0, &parameters);

	// ��������Ŀ������ݡ�
	// ���ǽ�����ȫ������������ʱ
	//�Ѹ��ǡ����ʹ������λ�������Σ���Ӧ�޸Ĵ˵��á�
	m_d3dContext->DiscardView1(m_d3dRenderTargetView.Get(), nullptr, 0);

	// �������ģ�ߵ����ݡ�
	m_d3dContext->DiscardView1(m_d3dDepthStencilView.Get(), nullptr, 0);

	// ���ͨ���Ͽ����ӻ��������������Ƴ����豸�������
	// �������´��������豸��Դ��
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		HandleDeviceLost();
	}

}

Math::Size DXResources::GetWindowSize()
{
	return m_logicalSize;
}











