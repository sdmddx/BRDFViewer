#include "stdafx.h"

#include "SphereScene.h"
#include <vector>
#include <d3dx11async.h>
#include <d3dcompiler.h>


using namespace Example;

SphereScene::SphereScene(const std::shared_ptr<DX::DXResources>& dxResources) :
	m_dxResources(dxResources),
	m_loadingComplete(false)
{

	m_shaderLoader = unique_ptr<Utilities::ShaderLoader>(new Utilities::ShaderLoader(dxResources));

	m_CB_WVP_Data = make_unique<CB_WVP>();
	m_CB_Light_Data = make_unique<CB_Light>();

	CreateWindowSizeDependentResources();
	CreateDeviceDependentResources();

	m_isLButtonDown = false;
	m_isRButtonDown = false;
	
	m_mouseWheel = 0;
}

SphereScene::~SphereScene()
{
	ReleaseDeviceDependentResources();
}
// 当窗口的大小改变时初始化视图参数。
void SphereScene::CreateWindowSizeDependentResources()
{
	Math::Size windowSize = m_dxResources->GetWindowSize();
	float aspectRatio = float(windowSize.Width) / float(windowSize.Height);
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// 这是一个简单的更改示例，当应用程序在纵向视图或对齐视图中时，可以进行此更改
	// 。
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// 请注意，OrientationTransform3D 矩阵在此处是后乘的，
	// 以正确确定场景的方向，使之与显示方向匹配。
	// 对于交换链的目标位图进行的任何绘制调用
	// 交换链呈现目标。对于到其他目标的绘制调用，
	// 不应应用此转换。

	// 此示例使用行主序矩阵利用右手坐标系。
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovLH(
		fovAngleY,
		aspectRatio,
		0.01f,
		100.0f
	);


	XMMATRIX model = XMMatrixIdentity();

	XMStoreFloat4x4(&m_CB_WVP_Data->model, model);

	XMStoreFloat4x4(
		&m_CB_WVP_Data->projection,
		XMMatrixTranspose(perspectiveMatrix)
	);

	// 眼睛位于(0,0.7,1.5)，并沿着 Y 轴使用向上矢量查找点(0,-0.1,0)。
	static const XMVECTORF32 eye = { 0.0f, 0.0f,-4.0f, 1.0f };
	static const XMVECTORF32 at = { 0.0f, 0.0f, 0.0f, 1.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 1.0f };

	m_camera = unique_ptr<Utilities::SdmCamera>(new Utilities::SdmCamera(eye, at, up));
	//SdmCamera* p = (SdmCamera*)::operator new(sizeof(SdmCamera));
	XMStoreFloat4x4(&m_CB_WVP_Data->view, XMMatrixTranspose(m_camera->GetView()));

	XMVECTOR deter;
	model_inverse = XMMatrixInverse(&deter, model);
	//XMVECTOR lightPos = eye;
	XMVECTOR lightPos = XMVectorSet(-2.0f, 0.0f, -2.0f, 1.0f);

	XMStoreFloat3(&m_CB_Light_Data->lightPos, XMVector4Transform(lightPos, model_inverse));
	m_CB_Light_Data->baseColor = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMStoreFloat3(&m_CB_Light_Data->viewPos, XMVector4Transform(XMLoadFloat4(&m_camera->GetEye()), model_inverse));

}

//根据设备对象创建当前场景需要的着色器 模型 纹理 等资源
void SphereScene::CreateDeviceDependentResources()
{
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	HRESULT hr = S_FALSE;

	static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};
	
	hr = m_shaderLoader->CreateVertexShader_IA(L"ShaderResources\\SphereScene_VS.hlsl", "VS", "vs_5_0",
		m_vertexShader, 
		vertexDesc,
		ARRAYSIZE(vertexDesc),
		m_inputLayout);

	hr = m_shaderLoader->CreatePixelShader(L"ShaderResources\\SphereScene_PS.hlsl", "PS_AnisotropicWard", "ps_5_0",
		m_pixelShader);

	CD3D11_BUFFER_DESC CB_MCP_Desc(sizeof(CB_MVP), D3D11_BIND_CONSTANT_BUFFER);
		m_dxResources->GetD3DDevice()->CreateBuffer(
			&CB_MCP_Desc,
			nullptr,
			&m_D3D_CB_WVP
		
	);

	CD3D11_BUFFER_DESC constantBufferDesc_Light(sizeof(CB_Light), D3D11_BIND_CONSTANT_BUFFER);
		m_dxResources->GetD3DDevice()->CreateBuffer(
			&constantBufferDesc_Light,
			nullptr,
			&m_D3D_CB_Light
		
	);

	

	// 加载两个着色器后，创建网格。
	unique_ptr<BasicGraphic::SphereGenerator> pSphere = unique_ptr<BasicGraphic::SphereGenerator>(new BasicGraphic::SphereGenerator);

	unique_ptr<AssetMagr::DataBuf>	m_dataBuf;
	m_dataBuf = pSphere->GenerateSpherePvN(4);
	
	unsigned int vertexCount = m_dataBuf->GetBufMeshByIndex(0)->GetCntVertex();
	Vertex_PosVernor* pVertexBuf = (Vertex_PosVernor*)malloc(sizeof(Vertex_PosVernor)*vertexCount);
	for (unsigned int index = 0; index < vertexCount; ++index)
	{
		pVertexBuf[index].pos = m_dataBuf->GetBufMeshByIndex(0)->pVertex[index];
		pVertexBuf[index].vernor = m_dataBuf->GetBufMeshByIndex(0)->pVertexNormal[index];

	}


	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = pVertexBuf;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(vertexCount * sizeof(Vertex_PosVernor), D3D11_BIND_VERTEX_BUFFER);
		m_dxResources->GetD3DDevice()->CreateBuffer(
			&vertexBufferDesc,
			&vertexBufferData,
			&m_vertexBuffer
		
	);

	free(pVertexBuf);

	unsigned int faceCount = m_dataBuf->GetBufMeshByIndex(0)->GetCntFace();
	m_indexCount = faceCount * 3;

	unsigned int* psphereIndices = (unsigned int*)malloc(m_indexCount * sizeof(unsigned int));
	unsigned int indexIndices = 0;
	unsigned int indexFace = 0;
	SDMuint3* pFace = m_dataBuf->GetBufMeshByIndex(0)->pFace;
	for (; indexIndices < m_indexCount;)
	{
		psphereIndices[indexIndices++] = pFace[indexFace].a[0];
		psphereIndices[indexIndices++] = pFace[indexFace].a[1];
		psphereIndices[indexIndices++] = pFace[indexFace++].a[2];

	}

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = psphereIndices;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC indexBufferDesc(m_indexCount * sizeof(unsigned int), D3D11_BIND_INDEX_BUFFER);
		m_dxResources->GetD3DDevice()->CreateBuffer(
			&indexBufferDesc,
			&indexBufferData,
			&m_indexBuffer
		
	);

	free(psphereIndices);

	//加载纹理
	//hr = D3DX11CreateShaderResourceViewFromFile(m_dxResources->GetD3DDevice(), L"TextureResources\\sdm.bmp", NULL, 0, &m_SRV_envMap, 0);
	//绑定像素着色器资源
	//m_dxResources->GetD3DDeviceContext()->PSSetShaderResources(0, 1, &m_SRV_envMap);
	//Microsoft::WRL::ComPtr<ID3D11Resource> envMap = NULL;
	//Microsoft::WRL::ComPtr<ID3D11Resource> pTex = NULL;
	//hr = D3DX11CreateTextureFromFile(m_dxResources->GetD3DDevice(), L"TextureResources\\sdm.jpg", 0, 0, &envMap, 0);
	//D3D11_SHADER_RESOURCE_VIEW_DESC envMap_Desc;
	

	//hr = m_dxResources->GetD3DDevice()->CreateShaderResourceView(envMap.Get(), NULL, &m_SRV_envMap);
	
	// 加载立方体后，就可以呈现该对象了。

	

	Microsoft::WRL::ComPtr<IWICBitmapDecoder> m_wicDecoder = NULL;

	IWICImagingFactory2* m_wicFactory = m_dxResources->GetWicImagingFactory();
	hr = m_wicFactory->CreateDecoderFromFilename(L"TextureResources\\sdm.bmp", NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&m_wicDecoder);

	Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> m_frame;
	hr = m_wicDecoder->GetFrame(0, &m_frame);

	Microsoft::WRL::ComPtr<IWICFormatConverter>		m_convert;
	hr = m_wicFactory->CreateFormatConverter(&m_convert);

	hr = m_convert->Initialize(m_frame.Get(), GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom);
	
	
	UINT height, width;
	m_frame->GetSize(&width, &height);

	//获得图片的像素格式
	//WICPixelFormatGUID pixelFormat;
	//ZeroMemory(&pixelFormat, sizeof(pixelFormat));
	//m_frame->GetPixelFormat(&pixelFormat);

	UINT s = height*width*4;
	unique_ptr<BYTE[]> buffer = unique_ptr<BYTE[]>(new BYTE[s]);
	hr = m_convert->CopyPixels(nullptr, width*4, s, buffer.get());


	//Microsoft::WRL::ComPtr<IDXGISurface> dxgi_surface;
	//hr = image_texture.As(&dxgi_surface);


	// 创建D3D纹理用于3D渲染
	D3D11_TEXTURE2D_DESC tex_desc = { 0 };
	tex_desc.ArraySize = 1;
	tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	tex_desc.CPUAccessFlags = 0;
	tex_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	tex_desc.Height = height;
	tex_desc.Width = width;
	tex_desc.MipLevels = 1;
	tex_desc.MiscFlags = 0;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;
	tex_desc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA sr;
	sr.pSysMem = buffer.get();
	sr.SysMemPitch = width*4;
	sr.SysMemSlicePitch = 0; // Not needed since this is a 2d texture

	Microsoft::WRL::ComPtr<ID3D11Texture2D>         image_texture;
	hr = m_dxResources->GetD3DDevice()->CreateTexture2D(&tex_desc, &sr, &image_texture);


	//设置采样器状态
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	// The sampler does not use anisotropic filtering, so this parameter is ignored.
	samplerDesc.MaxAnisotropy = 0;

	// Specify how texture coordinates outside of the range 0..1 are resolved.
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	// Use no special MIP clamping or bias.
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Don't use a comparison function.
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	// Border address mode is not used, so this parameter is ignored.
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 0.0f;

	
	
	hr = m_dxResources->GetD3DDevice()->CreateSamplerState(
			&samplerDesc,
			&sampler
	
	);

	//绑定贴图采样器
	m_dxResources->GetD3DDeviceContext()->PSSetSamplers(0, 1, &sampler);

	m_loadingComplete = true;
	
}

void SphereScene::Update(Utilities::StepTimer const& timer)
{
	if (m_isLButtonDown)
	{
		long deltaX = m_leftPointMovement.x - m_leftPoint.x;
		long deltaY = m_leftPointMovement.y - m_leftPoint.y;
		m_camera->TransformModeA(deltaX*0.003f);
		m_camera->TransformModeB(deltaY*0.003f);
		m_leftPoint = m_leftPointMovement;

	}
	else if(m_isRButtonDown)		//左右键同时按下时确保左键优先级更高
	{
		long deltaX = m_rightPointMovement.x - m_rightPoint.x;
		long deltaY = m_rightPointMovement.y - m_rightPoint.y;
		m_camera->TransformModeC(deltaX*0.003f);
		m_camera->TransformModeD(deltaY*0.003f);
		m_rightPoint = m_rightPointMovement;
	}

	//更新鼠标滑轮滚动消息
	short sum = m_mouseWheel / 120;
	if (sum)
	{
		float distance = sum/5.0f;
		m_camera->TransformModeF(distance);
		m_mouseWheel = 0;
	}

	//最后更新观察矩阵
	XMStoreFloat4x4(&m_CB_WVP_Data->view, XMMatrixTranspose(m_camera->GetView()));

	//更新观察者视点位置
	XMStoreFloat3(&m_CB_Light_Data->viewPos, XMVector4Transform(XMLoadFloat4(&m_camera->GetEye()), model_inverse));
}

void SphereScene::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_pixelShader.Reset();
	m_D3D_CB_WVP.Reset();
	m_D3D_CB_Light.Reset();
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();

	sampler.Reset();	
}

// 使用顶点和像素着色器呈现一个帧。
void SphereScene::Render()
{
	if (!m_loadingComplete)
	{
		return;
	}
	auto context = m_dxResources->GetD3DDeviceContext();
	// 将视区重置为针对整个屏幕。
	auto viewport = m_dxResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	// 将呈现目标重置为屏幕。
	ID3D11RenderTargetView *const targets[1] = { m_dxResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, m_dxResources->GetDepthStencilView());

	// 清除后台缓冲区和深度模具视图。
	FLOAT color[4]{ 0.22f,0.4f,0.35f,1.0f };
	context->ClearRenderTargetView(m_dxResources->GetBackBufferRenderTargetView(), color);
	context->ClearDepthStencilView(m_dxResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 准备常量缓冲区以将其发送到图形设备。
	context->UpdateSubresource(
		m_D3D_CB_WVP.Get(),
		0,
		NULL,
		m_CB_WVP_Data.get(),
		0,
		0
	);

	context->UpdateSubresource(
		m_D3D_CB_Light.Get(), 0, NULL, m_CB_Light_Data.get(), 0, 0
	);

	// 每个顶点都是 VertexPositionColor 结构的一个实例。
	UINT stride = sizeof(Vertex_PosVernor);
	UINT offset = 0;
	context->IASetVertexBuffers(
		0,
		1,
		m_vertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	context->IASetIndexBuffer(
		m_indexBuffer.Get(),
		DXGI_FORMAT_R32_UINT, // 每个索引都是一个 32 位无符号整数(short)。
		0
	);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_inputLayout.Get());

	// 附加我们的顶点着色器。
	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
	);

	
	//context->PSSetSamplers(0, 1, &sampler);

	// 将常量缓冲区发送到图形设备。
	context->VSSetConstantBuffers(
		0,
		1,
		m_D3D_CB_WVP.GetAddressOf()
	);

	context->PSSetConstantBuffers(
		0, 1, m_D3D_CB_Light.GetAddressOf()
	);


	// 附加我们的像素着色器。
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
	);



	
	// 绘制对象。
	context->DrawIndexed(
		m_indexCount,
		0,
		0
	);
}