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

//设置输出窗口
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


//创建设备无关资源
void DXResources::CreateDeviceIndependentResources()
{
	// 初始化 Direct2D 资源。
	D2D1_FACTORY_OPTIONS options;
	ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

#if defined(_DEBUG)
	// 如果项目处于调试生成阶段，请通过 SDK 层启用 Direct2D 调试。
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

	// 初始化 Direct2D 工厂。
		D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED,
			__uuidof(ID2D1Factory3),
			&options,
			&m_d2dFactory
		
	);

	// 初始化 DirectWrite 工厂。
		DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory3),
			&m_dwriteFactory
		
	);

	// 初始化 Windows 图像处理组件(WIC)工厂。
		CoCreateInstance(
			CLSID_WICImagingFactory2,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&m_wicFactory)	);

	


}

//创建设备资源
void DXResources::CreateDeviceResources()
{
	// 此标志为颜色通道排序与 API 默认设置不同的图面
	// 添加支持。要与 Direct2D 兼容，必须满足此要求。
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
	if (DX::SdkLayersAvailable())
	{
		// 如果项目处于调试生成过程中，请通过带有此标志的 SDK 层启用调试。
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}
#endif

	// 此数组定义此应用程序将支持的 DirectX 硬件功能级别组。
	// 请注意，应保留顺序。
	// 请不要忘记在应用程序的说明中声明其所需的
	// 最低功能级别。除非另行说明，否则假定所有应用程序均支持 9.1。
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

	// 创建 Direct3D 11 API 设备对象和对应的上下文。
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	HRESULT hr = D3D11CreateDevice(
		nullptr,					// 指定 nullptr 以使用默认适配器。
		D3D_DRIVER_TYPE_HARDWARE,	// 创建使用硬件图形驱动程序的设备。
		0,							// 应为 0，除非驱动程序是 D3D_DRIVER_TYPE_SOFTWARE。
		creationFlags,				// 设置调试和 Direct2D 兼容性标志。
		featureLevels,				// 此应用程序可以支持的功能级别的列表。
		ARRAYSIZE(featureLevels),	// 上面的列表的大小。
		D3D11_SDK_VERSION,			// 对于 Windows 应用商店应用，始终将此值设置为 D3D11_SDK_VERSION。
		&device,					// 返回创建的 Direct3D 设备。
		&m_d3dFeatureLevel,			// 返回所创建设备的功能级别。
		&context					// 返回设备的即时上下文。
	);

	if (FAILED(hr))
	{
		// 如果初始化失败，则回退到 WARP 设备。
			D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_WARP, // 创建 WARP 设备而不是硬件设备。
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

	// 将指针存储到 Direct3D 11.3 API 设备和即时上下文中。
	device.As(&m_d3dDevice);

		context.As(&m_d3dContext);

	// 创建 Direct2D 设备对象和对应的上下文。
	ComPtr<IDXGIDevice3> dxgiDevice;
		m_d3dDevice.As(&dxgiDevice);

		m_d2dFactory->CreateDevice(dxgiDevice.Get(), &m_d2dDevice);

		m_d2dDevice->CreateDeviceContext(
			D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
			&m_d2dContext
		);
}

// 每次更改窗口大小时需要重新创建这些资源
void DXResources::CreateWindowSizeDependentResources(Math::Size &s)
{
	// 清除特定于上一窗口大小的上下文。
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
		// 如果交换链已存在，请调整其大小。
		HRESULT hr = m_swapChain->ResizeBuffers(
			2, // 双缓冲交换链。
			lround(m_d3dRenderTargetSize.Width),
			lround(m_d3dRenderTargetSize.Height),
			DXGI_FORMAT_B8G8R8A8_UNORM,
			0
		);

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			// 如果出于任何原因移除了设备，将需要创建一个新的设备和交换链。
			HandleDeviceLost();

			// 现在一切都已设置完毕。不要继续执行此方法。HandleDeviceLost 将重新呈现此方法 
			// 并正确设置新设备。
			return;
		}
		return;
	}
	else
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
		
		swapChainDesc.Width = lround(m_d3dRenderTargetSize.Width);		// 匹配窗口的大小。
		swapChainDesc.Height = lround(m_d3dRenderTargetSize.Height);
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;				// 这是最常用的交换链格式。
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1;								// 请不要使用多采样。
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;									// 使用双缓冲最大程度地减小延迟。
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		swapChainDesc.Flags = 0;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		

		// 此序列获取用来创建上面的 Direct3D 设备的 DXGI 工厂。
		ComPtr<IDXGIDevice3> dxgiDevice;
			m_d3dDevice.As(&dxgiDevice);

		ComPtr<IDXGIAdapter> dxgiAdapter;
			dxgiDevice->GetAdapter(&dxgiAdapter);

		ComPtr<IDXGIFactory4> dxgiFactory;
			dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));

		//创建交换链
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

		// 确保 DXGI 不会一次对多个帧排队。这样既可以减小延迟，
		// 又可以确保应用程序将只在每个 VSync 之后渲染，从而最大程度地降低功率消耗。
			dxgiDevice->SetMaximumFrameLatency(1);
	}

	// 创建交换链后台缓冲区的渲染目标视图。
	ComPtr<ID3D11Texture2D1> backBuffer;
		m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

		m_d3dDevice->CreateRenderTargetView1(
			backBuffer.Get(),
			nullptr,
			&m_d3dRenderTargetView
		);

	// 根据需要创建用于 3D 渲染的深度模板视图。
	CD3D11_TEXTURE2D_DESC1 depthStencilDesc(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		lround(m_d3dRenderTargetSize.Width),
		lround(m_d3dRenderTargetSize.Height),
		1, // 此深度模具视图只有一个纹理。
		1, // 使用单一 mipmap 级别。
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

	// 设置用于确定整个窗口的 3D 渲染视区。
	m_screenViewport = CD3D11_VIEWPORT(
		0.0f,
		0.0f,
		m_d3dRenderTargetSize.Width,
		m_d3dRenderTargetSize.Height
	);

	m_d3dContext->RSSetViewports(1, &m_screenViewport);

	
	// 创建与交换链后台缓冲区关联的 Direct2D 目标位图
	// 并将其设置为当前目标。

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

// 确定呈现器目标的尺寸
void DXResources::UpdateRenderTargetSize(Math::Size &s)
{
	m_logicalSize.Width = s.Width;
	m_logicalSize.Height = s.Height;

}

// 重新创建所有设备资源并将其设置回当前状态。
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

// 注册我们的 DeviceNotify 以便在设备丢失和创建时收到通知。
void DXResources::RegisterDeviceNotify(IDeviceNotify* deviceNotify)
{
	m_deviceNotify = deviceNotify;
}

// 将交换链的内容显示到屏幕上。
void DXResources::Present()
{
	// 第一个参数指示 DXGI 进行阻止直到 VSync，这使应用程序
	// 在下一个 VSync 前进入休眠。这将确保我们不会浪费任何周期渲染
	// 从不会在屏幕上显示的帧。
	DXGI_PRESENT_PARAMETERS parameters = { 0 };
	HRESULT hr = m_swapChain->Present1(1, 0, &parameters);

	// 放弃呈现目标的内容。
	// 这是仅在完全覆盖现有内容时
	//已覆盖。如果使用脏矩形或滚动矩形，则应修改此调用。
	m_d3dContext->DiscardView1(m_d3dRenderTargetView.Get(), nullptr, 0);

	// 放弃深度模具的内容。
	m_d3dContext->DiscardView1(m_d3dDepthStencilView.Get(), nullptr, 0);

	// 如果通过断开连接或升级驱动程序移除了设备，则必须
	// 必须重新创建所有设备资源。
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		HandleDeviceLost();
	}

}

Math::Size DXResources::GetWindowSize()
{
	return m_logicalSize;
}











