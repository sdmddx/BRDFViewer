#include "stdafx.h"

#include <vector>
#include <d3dx11async.h>
#include <d3dcompiler.h>


using namespace Example;

BP_DS::BP_DS(const std::shared_ptr<DX::DXResources>& dxResources) :
	m_dxResources(dxResources),
	m_loadingComplete(false)
{

	m_shaderLoader = unique_ptr<Utilities::ShaderLoader>(new Utilities::ShaderLoader(dxResources));
	m_dxBufferCreator = Utilities::DXBufferCreator::Create();


	m_CB_WVP_Data = make_unique<CB_WVP>();
	m_CB_WINDOWINFO_Data = make_unique<CB_WINDOWINFO>();
	m_CB_WVP_Inverse_Data = make_unique<CB_WVP_Inverse>();
	m_CB_LightsInfo_Data = make_unique<LightsInfo>();

	numLights = 100;			//定义光源数量

	CreateWindowSizeDependentResources();
	CreateDeviceDependentResources();

	m_isLButtonDown = false;
	m_isRButtonDown = false;

	m_mouseWheel = 0;
}

BP_DS::~BP_DS()
{

}
// 当窗口的大小改变时初始化视图参数。
void BP_DS::CreateWindowSizeDependentResources()
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
	float nearZ = 0.01f;
	float farZ = 100.0f;
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovLH(
		fovAngleY,
		aspectRatio,
		nearZ,
		farZ
	);


	XMMATRIX model = XMMatrixIdentity();

	XMStoreFloat4x4(&m_CB_WVP_Data->model, model);

	XMStoreFloat4x4(
		&m_CB_WVP_Data->projection,
		XMMatrixTranspose(perspectiveMatrix)
	);

	// 眼睛位于(0,0.7,1.5)，并沿着 Y 轴使用向上矢量查找点(0,-0.1,0)。
	static const XMVECTORF32 eye = { 0.0f, 0.0f,-2.0f, 1.0f };
	static const XMVECTORF32 at = { 0.0f, 0.0f, 0.0f, 1.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 1.0f };

	m_camera = unique_ptr<Utilities::SdmCamera>(new Utilities::SdmCamera(eye, at, up));
	//SdmCamera* p = (SdmCamera*)::operator new(sizeof(SdmCamera));
	XMStoreFloat4x4(&m_CB_WVP_Data->view, XMMatrixTranspose(m_camera->GetView()));
	XMStoreFloat4x4(&m_CB_WVP_Inverse_Data->view_Inverse, XMMatrixTranspose(m_camera->GetInverseView()));
	m_CB_WVP_Inverse_Data->viewPos = m_camera->GetEye();

	XMVECTOR deter;
	model_inverse = XMMatrixInverse(&deter, model);

	m_CB_WINDOWINFO_Data->height= float(m_dxResources->GetWindowSize().Height);
	m_CB_WINDOWINFO_Data->width = float(m_dxResources->GetWindowSize().Width);
	m_CB_WINDOWINFO_Data->fovAngleY = fovAngleY/2.0f;
	m_CB_WINDOWINFO_Data->aspectRatio = aspectRatio;
	m_CB_WINDOWINFO_Data->nearZ = nearZ;
	m_CB_WINDOWINFO_Data->farZ = farZ;

}

//根据设备对象创建当前场景需要的着色器 模型 纹理 等资源
void BP_DS::CreateDeviceDependentResources()
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

	//加载G过程着色器及创建输入布局
	static const D3D11_INPUT_ELEMENT_DESC vertexDesc_G[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,       0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = m_shaderLoader->CreateVertexShader_IA(L"ShaderResources\\BP_DS_VS.hlsl", "VS_G", "vs_5_0", dwShaderFlags,
		m_VS_G, 
		vertexDesc_G,
		ARRAYSIZE(vertexDesc_G),
		m_IL_G);

	hr = m_shaderLoader->CreatePixelShader(L"ShaderResources\\BP_DS_PS.hlsl", "PS_G", "ps_5_0", dwShaderFlags,
		m_PS_G);

	//加载S过程着色器及创建输入布局
	static const D3D11_INPUT_ELEMENT_DESC vertexDesc_S[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,       0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = m_shaderLoader->CreateVertexShader_IA(L"ShaderResources\\BP_DS_VS.hlsl", "VS_S", "vs_5_0", dwShaderFlags,
		m_VS_S,
		vertexDesc_S,
		ARRAYSIZE(vertexDesc_S),
		m_IL_S);

	hr = m_shaderLoader->CreatePixelShader(L"ShaderResources\\BP_DS_PS.hlsl", "PS_S", "ps_5_0", dwShaderFlags,
		m_PS_S);

	//创建常量缓冲区
	CD3D11_BUFFER_DESC CB_MVP_Desc(sizeof(CB_WVP), D3D11_BIND_CONSTANT_BUFFER);
		m_dxResources->GetD3DDevice()->CreateBuffer(
			&CB_MVP_Desc,
			nullptr,
			&m_D3D_CB_WVP);

	CD3D11_BUFFER_DESC CB_MVP_Inverse_Desc(sizeof(CB_WVP_Inverse), D3D11_BIND_CONSTANT_BUFFER);
		m_dxResources->GetD3DDevice()->CreateBuffer(
			&CB_MVP_Inverse_Desc,
			nullptr,
			&m_D3D_CB_WVP_Inverse);
	
	//含有窗口信息的CB
	CD3D11_BUFFER_DESC CB_WINDOWINFO_Desc(sizeof(CB_WINDOWINFO), D3D11_BIND_CONSTANT_BUFFER);
		m_dxResources->GetD3DDevice()->CreateBuffer(
			&CB_WINDOWINFO_Desc,
			nullptr,
			&m_D3D_CB_WINDOWINFO);

		CD3D11_BUFFER_DESC CB_LightsInfo_Desc(sizeof(CB_WINDOWINFO), D3D11_BIND_CONSTANT_BUFFER);
		m_dxResources->GetD3DDevice()->CreateBuffer(
			&CB_LightsInfo_Desc,
			nullptr,
			&m_D3D_CB_LightsInfo);

	// 加载两个着色器后，创建网格。
	unique_ptr<BasicGraphic::SphereGenerator> pSphere = unique_ptr<BasicGraphic::SphereGenerator>(new BasicGraphic::SphereGenerator);

	unique_ptr<AssetMagr::DataBuf>	m_dataBuf;
	m_dataBuf = pSphere->GenerateSpherePvN(4);

	unsigned int vertexCount = m_dataBuf->GetBufMeshByIndex(0)->GetCntVertex();
	//创建G过程顶点缓冲区
	Vertex_PosVernor* pVertexBuf_G = (Vertex_PosVernor*)malloc(sizeof(Vertex_PosVernor)*vertexCount);
	for (unsigned int index = 0; index < vertexCount; ++index)
	{
		pVertexBuf_G[index].pos = m_dataBuf->GetBufMeshByIndex(0)->pVertex[index];
		pVertexBuf_G[index].vernor = m_dataBuf->GetBufMeshByIndex(0)->pVertexNormal[index];
		//pVertexBuf_G[index].tex = m_dataBuf->GetBufMeshByIndex(0)->pPositiveFaceIndexUV[index];

	}

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = pVertexBuf_G;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc_G(vertexCount * sizeof(Vertex_PosVernor), D3D11_BIND_VERTEX_BUFFER);
		m_dxResources->GetD3DDevice()->CreateBuffer(
			&vertexBufferDesc_G,
			&vertexBufferData,
			&m_vertexBuffer_G
		
	);

	free(pVertexBuf_G);

	//创建S过程顶点缓冲区
	Vertex_Pos* pVertexBuf_S = (Vertex_Pos*)malloc(sizeof(Vertex_Pos)*vertexCount);
	for (unsigned int index = 0; index < vertexCount; ++index)
	{
		pVertexBuf_S[index].pos = m_dataBuf->GetBufMeshByIndex(0)->pVertex[index];
		//pVertexBuf_S[index].tex = m_dataBuf->GetBufMeshByIndex(0)->pPositiveFaceIndexUV[index];

	}

	vertexBufferData.pSysMem = pVertexBuf_S;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc_S(vertexCount * sizeof(Vertex_Pos), D3D11_BIND_VERTEX_BUFFER);
	m_dxResources->GetD3DDevice()->CreateBuffer(
		&vertexBufferDesc_S,
		&vertexBufferData,
		&m_vertexBuffer_S

	);

	free(pVertexBuf_S);

	//创建索引缓冲区
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
	hr = D3DX11CreateShaderResourceViewFromFile(m_dxResources->GetD3DDevice(), L"TextureResources\\sdm.bmp", NULL, 0, &m_SRV_envMap, 0);
	
	//绑定像素着色器资源
	m_dxResources->GetD3DDeviceContext()->PSSetShaderResources(0, 1, &m_SRV_envMap);

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

	//为法线纹理图创建渲染目标
	D3D11_TEXTURE2D_DESC tex_desc = { 0 };
	tex_desc.ArraySize = 1;
	tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	tex_desc.CPUAccessFlags = 0;
	tex_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tex_desc.Height = m_dxResources->GetWindowSize().Height;
	tex_desc.Width = m_dxResources->GetWindowSize().Width;
	tex_desc.MipLevels = 1;
	tex_desc.MiscFlags = 0;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;
	tex_desc.Usage = D3D11_USAGE_DEFAULT;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> verNorTex;

	hr = m_dxResources->GetD3DDevice()->CreateTexture2D(&tex_desc, NULL, &verNorTex);

	D3D11_RENDER_TARGET_VIEW_DESC RTV_Desc_verNor;

	RTV_Desc_verNor.Format = tex_desc.Format;
	RTV_Desc_verNor.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RTV_Desc_verNor.Texture2D.MipSlice = 0;

	hr = m_dxResources->GetD3DDevice()->CreateRenderTargetView(verNorTex.Get(), &RTV_Desc_verNor, &m_RTV_verNorTex);

	//创建第二个过程的着色器资源视图
	D3D11_SHADER_RESOURCE_VIEW_DESC Desc_S;
	ZeroMemory(&Desc_S, sizeof(Desc_S));
	Desc_S.Format = tex_desc.Format;
	Desc_S.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	Desc_S.Texture2D.MipLevels = tex_desc.MipLevels;
	Desc_S.Texture2D.MostDetailedMip = 0;

	hr = m_dxResources->GetD3DDevice()->CreateShaderResourceView(
		verNorTex.Get(),
		&Desc_S,
		&m_SRV_verNorTex);

	//创建光源信息结构
	srand(10);
	LightInfo* plightInfo = (LightInfo*)malloc(sizeof(LightInfo)*numLights);
	for (UINT index = 0; index < numLights; ++index)
	{
	
		//产生随机光源信息
		//光源xyz坐标在（-10，10)区间
		auto a = [index]( float multiple=1.0f) {
			//光源位置生成方法  
			return XMFLOAT3(float(rand()-16384) / 16384.0f*multiple, float(rand() - 16384) / 16384.0f*multiple, float(rand() - 16384) / 16384.0f*multiple);
		};
		auto b = []() {
			//光源颜色生成方法
			//(0,1)随机数
			return XMFLOAT3(float(rand()) / 32767.0f, float(rand()) / 32767.0f, float(rand()) / 32767.0f);
		};
		plightInfo[index].lightPos =  a(10.0f);
		plightInfo[index].lightColor = b();

	}

	ID3D11Buffer* pbuffer_LightInfo;
	m_dxBufferCreator->CreateStructuredBuffer<LightInfo>(m_dxResources->GetD3DDevice(), numLights, &pbuffer_LightInfo,
		&m_SRV_LightInfo,plightInfo);

	m_CB_LightsInfo_Data->numLights = numLights;
	m_CB_LightsInfo_Data->ambientColor=XMFLOAT3(1.0f, 0.0f, 0.0f);

	m_loadingComplete = true;

}

void BP_DS::Update(Utilities::StepTimer const& timer)
{
	if (m_isLButtonDown)
	{
		long deltaX = m_leftPointMovement.x - m_leftPoint.x;
		long deltaY = m_leftPointMovement.y - m_leftPoint.y;
		m_camera->TransformModeA(deltaX*0.003f);
		m_camera->TransformModeB(deltaY*0.003f);
		m_leftPoint = m_leftPointMovement;

	}
	else if (m_isRButtonDown)		//左右键同时按下时确保左键优先级更高
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
		float distance = sum / 5.0f;
		m_camera->TransformModeF(distance);
		m_mouseWheel = 0;
	}

	//最后更新观察矩阵及其逆矩阵
	XMStoreFloat4x4(&m_CB_WVP_Data->view, XMMatrixTranspose(m_camera->GetView()));
	XMStoreFloat4x4(&m_CB_WVP_Inverse_Data->view_Inverse, XMMatrixTranspose(m_camera->GetInverseView()));
	m_CB_WVP_Inverse_Data->viewPos = m_camera->GetEye();

}

void BP_DS::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_IL_G.Reset();
	m_IL_S.Reset();
	m_VS_G.Reset();
	m_VS_S.Reset();
	m_PS_G.Reset();
	m_PS_S.Reset();
	m_vertexBuffer_G.Reset();
	m_vertexBuffer_S.Reset();
	m_D3D_CB_WINDOWINFO.Reset();
	m_D3D_CB_WVP.Reset();
	m_D3D_CB_WVP_Inverse.Reset();
	m_D3D_CB_LightsInfo.Reset();

	m_indexBuffer.Reset();

	m_camera = nullptr;
	sampler.Reset();
	m_SRV_envMap->Release();
	m_SRV_verNorTex->Release();
	m_SRV_LightInfo->Release();

	m_RTV_verNorTex.Reset();

}

// 使用顶点和像素着色器呈现一个帧。
void BP_DS::Render()
{
	// 加载是异步的。仅在加载几何图形后才会绘制它。
	if (!m_loadingComplete)
	{
		return;
	}

	auto context = m_dxResources->GetD3DDeviceContext();

	//设置渲染目标
	ID3D11RenderTargetView *const targets_Nor[1] = { m_RTV_verNorTex.Get() };
	context->OMSetRenderTargets(1, targets_Nor, m_dxResources->GetDepthStencilView());

	// 清除后台缓冲区和深度模具视图。
	context->ClearRenderTargetView(m_RTV_verNorTex.Get(), DirectX::Colors::CornflowerBlue);
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

	// 每个顶点都是 Vertex_PosverNorTex 结构的一个实例。
	UINT stride = sizeof(Vertex_PosVernor);
	UINT offset = 0;
	context->IASetVertexBuffers(
		0,
		1,
		m_vertexBuffer_G.GetAddressOf(),
		&stride,
		&offset
	);

	context->IASetIndexBuffer(
		m_indexBuffer.Get(),
		DXGI_FORMAT_R32_UINT, // 每个索引都是一个 32 位无符号整数(short)。
		0
	);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_IL_G.Get());

	// 附加我们的顶点着色器。
	context->VSSetShader(
		m_VS_G.Get(),
		nullptr,
		0
	);

	// 将常量缓冲区发送到图形设备。
	context->VSSetConstantBuffers(
		0,
		1,
		m_D3D_CB_WVP.GetAddressOf()
	);

	// 附加我们的像素着色器。
	context->PSSetShader(
		m_PS_G.Get(),
		nullptr,
		0
	);

	// 绘制对象。
	context->DrawIndexed(
		m_indexCount,
		0,
		0
	);

	//第二步  光照过程

	// 将视区重置为针对整个屏幕。
	auto viewport = m_dxResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	//更新渲染目标视图为屏幕
	ID3D11RenderTargetView *const  targets[1] = { m_dxResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, m_dxResources->GetDepthStencilView());

	// 清除后台缓冲区和深度模具视图。
	context->ClearRenderTargetView(m_dxResources->GetBackBufferRenderTargetView(), DirectX::Colors::CornflowerBlue);
	context->ClearDepthStencilView(m_dxResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 每个顶点都是 Vertex_PosTex 结构的一个实例。
	context->UpdateSubresource(
		m_D3D_CB_WVP_Inverse.Get(),
		0,
		NULL,
		m_CB_WVP_Inverse_Data.get(),
		0,
		0
	);

	context->UpdateSubresource(
		m_D3D_CB_WINDOWINFO.Get(),
		0,
		NULL,
		m_CB_WINDOWINFO_Data.get(),
		0,
		0
	);

	context->UpdateSubresource(
		m_D3D_CB_LightsInfo.Get(),
		0,
		NULL,
		m_CB_LightsInfo_Data.get(),
		0,
		0
	);

	stride = sizeof(Vertex_Pos);
	context->IASetVertexBuffers(
		0,
		1,
		m_vertexBuffer_S.GetAddressOf(),
		&stride,
		&offset
	);

	context->IASetInputLayout(m_IL_S.Get());

	context->PSSetConstantBuffers(
		0,
		1,
		m_D3D_CB_WINDOWINFO.GetAddressOf()
	);
	context->PSSetConstantBuffers(
		1,
		1,
		m_D3D_CB_WVP_Inverse.GetAddressOf()
	);

	context->PSSetConstantBuffers(
		2,
		1,
		m_D3D_CB_LightsInfo.GetAddressOf()
	);

	// 附加我们的顶点着色器。
	context->VSSetShader(
		m_VS_S.Get(),
		nullptr,
		0
	);

	// 附加我们的像素着色器。
	context->PSSetShader(
		m_PS_S.Get(),
		nullptr,
		0
	);

	//绑定着色器资源
	context->PSSetShaderResources(1, 1, &m_SRV_verNorTex);	

	//绑定光源信息
	context->PSSetShaderResources(2, 1, &m_SRV_LightInfo);


	// 绘制对象。
	context->DrawIndexed(
		m_indexCount,
		0,
		0
	);


}