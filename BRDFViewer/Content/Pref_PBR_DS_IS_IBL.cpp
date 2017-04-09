#include "stdafx.h"

#include <vector>
#include <d3dx11async.h>
#include <d3dcompiler.h>


using namespace Example;

Pref_PBR_DS_IS_IBL_Scene::Pref_PBR_DS_IS_IBL_Scene(const std::shared_ptr<DX::DXResources>& dxResources) :
	m_dxResources(dxResources),
	m_loadingComplete(false)
{

	m_shaderLoader = unique_ptr<Utilities::ShaderLoader>(new Utilities::ShaderLoader(dxResources));
	m_dxBufferCreator = Utilities::DXBufferCreator::Create();
	m_CB_WVP_Data = make_unique<CB_WVP>();
	m_CB_PBR_Data = make_unique<CB_PBR>();
	m_CB_WINDOWINFO_Data = make_unique<CB_WINDOWINFO>();
	m_CB_WVP_Inverse_Data = make_unique<CB_WVP_Inverse>();
	m_CB_Light_Data = make_unique<CB_Light>();

	m_CB_MatInfo_albedo_metalness_Data = make_unique<CB_MatInfo_albedo_metalness>();

	CreateWindowSizeDependentResources();
	CreateDeviceDependentResources();

	m_isLButtonDown = false;
	m_isRButtonDown = false;

	m_mouseWheel = 0;
}

Pref_PBR_DS_IS_IBL_Scene::~Pref_PBR_DS_IS_IBL_Scene()
{
	ReleaseDeviceDependentResources();
}
// 当窗口的大小改变时初始化视图参数。
void Pref_PBR_DS_IS_IBL_Scene::CreateWindowSizeDependentResources()
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
	static const XMVECTORF32 eye = { 0.0f, 0.0f,-4.0f, 1.0f };
	static const XMVECTORF32 at = { 0.0f, 0.0f, 0.0f, 1.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 1.0f };

	m_camera = unique_ptr<Utilities::SdmCamera>(new Utilities::SdmCamera(eye, at, up));
	//SdmCamera* p = (SdmCamera*)::operator new(sizeof(SdmCamera));
	XMStoreFloat4x4(&m_CB_WVP_Data->view, XMMatrixTranspose(m_camera->GetView()));

	XMStoreFloat4x4(&m_CB_WVP_Inverse_Data->view_Inverse, XMMatrixTranspose(m_camera->GetInverseView()));
	XMStoreFloat4(&m_CB_WVP_Inverse_Data->viewPos, XMVector4Transform(XMLoadFloat4(&m_camera->GetEye()), model_inverse));

	XMVECTOR deter;
	model_inverse = XMMatrixInverse(&deter, model);
	XMStoreFloat4x4(&m_CB_WVP_Inverse_Data->model_Inverse, XMMatrixTranspose(model_inverse));
	m_CB_WINDOWINFO_Data->height = float(m_dxResources->GetWindowSize().Height);
	m_CB_WINDOWINFO_Data->width = float(m_dxResources->GetWindowSize().Width);
	m_CB_WINDOWINFO_Data->fovAngleY = fovAngleY / 2.0f;
	m_CB_WINDOWINFO_Data->aspectRatio = aspectRatio;
	m_CB_WINDOWINFO_Data->nearZ = nearZ;
	m_CB_WINDOWINFO_Data->farZ = farZ;
	//XMVECTOR lightPos = eye;
	XMVECTOR lightPos = XMVectorSet(2.0f, 2.0f, -2.0f, 1.0f);

	XMStoreFloat3(&m_CB_Light_Data->lightPos, XMVector4Transform(lightPos, model_inverse));
	m_CB_Light_Data->baseColor = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMStoreFloat3(&m_CB_Light_Data->viewPos, XMVector4Transform(XMLoadFloat4(&m_camera->GetEye()), model_inverse));
	m_CB_PBR_Data->roughness = 0.0f;
	m_CB_PBR_Data->metalic = 0.7f;
	m_CB_PBR_Data->specular = 0.08f;
	m_CB_PBR_Data->specularTint = 1.0f;
	m_CB_PBR_Data->clearCoat = 0.25f;
	m_CB_PBR_Data->clearCoatGloss = 0.5f;
	m_CB_PBR_Data->D_GTR_N = 2.0f;
	m_CB_PBR_Data->D_GTR_C = 0.25f;
	m_CB_PBR_Data->anisotropic = 1.0f;


	m_CB_MatInfo_albedo_metalness_Data->albedo = XMFLOAT3{ 0.9f,0.9f,0.9f };
	m_CB_MatInfo_albedo_metalness_Data->metalness = 1.0f;

}

//根据设备对象创建当前场景需要的着色器 模型 纹理 等资源
void Pref_PBR_DS_IS_IBL_Scene::CreateDeviceDependentResources()
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
	static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};

	hr = m_shaderLoader->CreateVertexShader_IA(L"ShaderResources\\Pref_PBR_DS_IS_IBL_VS.hlsl", "VS_G", "vs_5_0",
		m_VS_G,
		vertexDesc,
		ARRAYSIZE(vertexDesc),
		m_IL_G);

	hr = m_shaderLoader->CreatePixelShader(L"ShaderResources\\Pref_PBR_DS_IS_IBL_PS.hlsl", "PS_G", "ps_5_0",
		m_PS_G);

	//加载S过程着色器及创建输入布局
	static const D3D11_INPUT_ELEMENT_DESC vertexDesc_S[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = m_shaderLoader->CreateVertexShader_IA(L"ShaderResources\\Pref_PBR_DS_IS_IBL_VS.hlsl", "VS_S", "vs_5_0",
		m_VS_S,
		vertexDesc_S,
		ARRAYSIZE(vertexDesc_S),
		m_IL_S);

	hr = m_shaderLoader->CreatePixelShader(L"ShaderResources\\Pref_PBR_DS_IS_IBL_PS.hlsl", "PS_S", "ps_5_0",
		m_PS_S);

	//加载计算着色器
	hr = m_shaderLoader->CreateComputeShader(L"ShaderResources\\Pref_PBR_DS_IS_IBL_CS.hlsl", "CS", "cs_5_0",
		m_computeShader);

	CD3D11_BUFFER_DESC CB_MCP_Desc(sizeof(CB_MVP), D3D11_BIND_CONSTANT_BUFFER);
	m_dxResources->GetD3DDevice()->CreateBuffer(
		&CB_MCP_Desc,
		nullptr,
		&m_D3D_CB_WVP

	);

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

	CD3D11_BUFFER_DESC constantBufferDesc_Light(sizeof(CB_Light), D3D11_BIND_CONSTANT_BUFFER);
	m_dxResources->GetD3DDevice()->CreateBuffer(
		&constantBufferDesc_Light,
		nullptr,
		&m_D3D_CB_Light

	);

	CD3D11_BUFFER_DESC constantBufferDesc_PBR(sizeof(CB_PBR), D3D11_BIND_CONSTANT_BUFFER);
	m_dxResources->GetD3DDevice()->CreateBuffer(
		&constantBufferDesc_PBR,
		nullptr,
		&m_D3D_CB_PBR

	);

	m_dxBufferCreator->CreateContantBuffer<CB_MatInfo_albedo_metalness>(m_dxResources->GetD3DDevice(), &m_D3D_CB_MatInfo_albedo_metalness);



	// 加载两个着色器后，创建网格。
	unique_ptr<BasicGraphic::SphereGenerator> pSphere = unique_ptr<BasicGraphic::SphereGenerator>(new BasicGraphic::SphereGenerator);

	unique_ptr<AssetMagr::DataBuf>	m_dataBuf;
	m_dataBuf = pSphere->GenerateSpherePvN(4);

	unsigned int vertexCount = m_dataBuf->GetBufMeshByIndex(0)->GetCntVertex();

	//创建G过程顶点缓冲区
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
		&m_VB_G

	);

	free(pVertexBuf);

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
		&m_VB_S

	);

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

	hr = D3DX11CreateShaderResourceViewFromFile(m_dxResources->GetD3DDevice(), L"TextureResources\\right.jpg", NULL, 0, &m_cubeMap[0], 0);
	hr = D3DX11CreateShaderResourceViewFromFile(m_dxResources->GetD3DDevice(), L"TextureResources\\left.jpg", NULL, 0, &m_cubeMap[1], 0);
	hr = D3DX11CreateShaderResourceViewFromFile(m_dxResources->GetD3DDevice(), L"TextureResources\\top.jpg", NULL, 0, &m_cubeMap[2], 0);
	hr = D3DX11CreateShaderResourceViewFromFile(m_dxResources->GetD3DDevice(), L"TextureResources\\bottom.jpg", NULL, 0, &m_cubeMap[3], 0);
	hr = D3DX11CreateShaderResourceViewFromFile(m_dxResources->GetD3DDevice(), L"TextureResources\\back.jpg", NULL, 0, &m_cubeMap[4], 0);
	hr = D3DX11CreateShaderResourceViewFromFile(m_dxResources->GetD3DDevice(), L"TextureResources\\front.jpg", NULL, 0, &m_cubeMap[5], 0);


	//绑定像素着色器资源
	m_dxResources->GetD3DDeviceContext()->PSSetShaderResources(1, 6, &m_cubeMap[0]);

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

	UINT s = height*width * 4;
	unique_ptr<BYTE[]> buffer = unique_ptr<BYTE[]>(new BYTE[s]);
	hr = m_convert->CopyPixels(nullptr, width * 4, s, buffer.get());


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
	//sr.pSysMem= m_fileBitmap->
	sr.pSysMem = buffer.get();
	sr.SysMemPitch = width * 4;
	sr.SysMemSlicePitch = 0; // Not needed since this is a 2d texture

	Microsoft::WRL::ComPtr<ID3D11Texture2D>         image_texture;
	hr = m_dxResources->GetD3DDevice()->CreateTexture2D(&tex_desc, &sr, &image_texture);

	// Once the texture is created, we must create a shader resource view of it
	// so that shaders may use it.  In general, the view description will match
	// the texture description.
	D3D11_SHADER_RESOURCE_VIEW_DESC tVDesc;
	ZeroMemory(&tVDesc, sizeof(tVDesc));
	tVDesc.Format = tex_desc.Format;
	tVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	tVDesc.Texture2D.MipLevels = tex_desc.MipLevels;
	tVDesc.Texture2D.MostDetailedMip = 0;


	hr = m_dxResources->GetD3DDevice()->CreateShaderResourceView(
		image_texture.Get(),
		&tVDesc,
		&m_SRV_envMap);



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

	//创建收集BRDF用的纹理及CS的UAV
	ID3D11Texture2D* tex = m_tex_InteBRDF.Get();
	m_dxBufferCreator->CreateTexSRVUAV(m_dxResources->GetD3DDevice(),
		Math::Size{ 256,256 }, DXGI_FORMAT_R32G32_FLOAT, &tex,
		&m_SRV_InteBRDF, &m_UAV_InteBRDF);


	///为G_buffer创建资源
	//为法线纹理图创建渲染目标
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

	hr = m_dxResources->GetD3DDevice()->CreateTexture2D(&tex_desc, NULL, &m_tex_verNor);

	D3D11_RENDER_TARGET_VIEW_DESC RTV_Desc_verNor;

	RTV_Desc_verNor.Format = tex_desc.Format;
	RTV_Desc_verNor.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RTV_Desc_verNor.Texture2D.MipSlice = 0;

	hr = m_dxResources->GetD3DDevice()->CreateRenderTargetView(m_tex_verNor.Get(), &RTV_Desc_verNor, &m_RTV_verNorTex);

	//创建法线纹理图的着色器资源视图
	D3D11_SHADER_RESOURCE_VIEW_DESC Desc_S;
	ZeroMemory(&Desc_S, sizeof(Desc_S));
	Desc_S.Format = tex_desc.Format;
	Desc_S.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	Desc_S.Texture2D.MipLevels = tex_desc.MipLevels;
	Desc_S.Texture2D.MostDetailedMip = 0;

	hr = m_dxResources->GetD3DDevice()->CreateShaderResourceView(
		m_tex_verNor.Get(),
		&Desc_S,
		&m_SRV_verNorTex);

	//为albedometalness纹理图创建渲染目标
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

	hr = m_dxResources->GetD3DDevice()->CreateTexture2D(&tex_desc, NULL, &m_tex_AlbedoMetalness);

	D3D11_RENDER_TARGET_VIEW_DESC RTV_Desc_AlbedoMetalness;

	RTV_Desc_AlbedoMetalness.Format = tex_desc.Format;
	RTV_Desc_AlbedoMetalness.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RTV_Desc_AlbedoMetalness.Texture2D.MipSlice = 0;

	hr = m_dxResources->GetD3DDevice()->CreateRenderTargetView(m_tex_AlbedoMetalness.Get(), &RTV_Desc_AlbedoMetalness, &m_RTV__AlbedoMetalness);

	//创建albedometalness纹理图的着色器资源视图
	Desc_S.Format = tex_desc.Format;
	Desc_S.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	Desc_S.Texture2D.MipLevels = tex_desc.MipLevels;
	Desc_S.Texture2D.MostDetailedMip = 0;

	hr = m_dxResources->GetD3DDevice()->CreateShaderResourceView(
		m_tex_AlbedoMetalness.Get(),
		&Desc_S,
		&m_SRV__AlbedoMetalness);



	IntegrateBRDF();
	//Init_SimpleShader();

	m_loadingComplete = true;

}

void Pref_PBR_DS_IS_IBL_Scene::Update(Utilities::StepTimer const& timer)
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

	//最后更新观察矩阵
	XMStoreFloat4x4(&m_CB_WVP_Data->view, XMMatrixTranspose(m_camera->GetView()));
	XMStoreFloat4x4(&m_CB_WVP_Inverse_Data->view_Inverse, XMMatrixTranspose(m_camera->GetInverseView()));
	//更新观察者视点位置
	XMStoreFloat4(&m_CB_WVP_Inverse_Data->viewPos, XMVector4Transform(XMLoadFloat4(&m_camera->GetEye()), model_inverse));
	XMStoreFloat3(&m_CB_Light_Data->viewPos, XMVector4Transform(XMLoadFloat4(&m_camera->GetEye()), model_inverse));
}

void Pref_PBR_DS_IS_IBL_Scene::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_VS_G.Reset();
	m_IL_G.Reset();
	m_PS_G.Reset();
	m_computeShader.Reset();
	m_D3D_CB_WINDOWINFO.Reset();
	m_D3D_CB_WVP.Reset();
	m_D3D_CB_WVP_Inverse.Reset();
	m_D3D_CB_PBR.Reset();
	m_D3D_CB_Light.Reset();
	m_D3D_CB_MatInfo_albedo_metalness.Reset();

	m_VB_G.Reset();
	m_indexBuffer.Reset();

	sampler.Reset();
	m_SRV_envMap->Release();

	m_tex_InteBRDF.Reset();
	m_SRV_InteBRDF->Release();
	m_UAV_InteBRDF->Release();
}

//利用CS收集BRDF，之后才执行render()
bool Pref_PBR_DS_IS_IBL_Scene::IntegrateBRDF()
{
	auto context = m_dxResources->GetD3DDeviceContext();

	UINT UAVInitialCounts = 0;
	context->CSSetUnorderedAccessViews(0, 1, &m_UAV_InteBRDF, &UAVInitialCounts);	//？？？？
																					//context->CSSetShaderResources(0, 1, &m_SRV_InteBRDF);
	context->CSSetShader(m_computeShader.Get(), nullptr, 0);
	//context->Dispatch(16, 16, 0);

	D3D11_QUERY_DESC pQueryDesc;
	pQueryDesc.Query = D3D11_QUERY_EVENT;
	pQueryDesc.MiscFlags = 0;
	ID3D11Query *pEventQuery;
	m_dxResources->GetD3DDevice()->CreateQuery(&pQueryDesc, &pEventQuery);

	context->End(pEventQuery); // 在 pushbuffer 中插入一个篱笆
	while (context->GetData(pEventQuery, NULL, 0, 0) == S_FALSE) {} // 自旋等待事件结束

	context->Dispatch(16, 16, 1);// 启动着色器

	context->End(pEventQuery); // 在 pushbuffer 中插入一个篱笆
	while (context->GetData(pEventQuery, NULL, 0, 0) == S_FALSE) {} // 自旋等待事件结束

																	//记得解除绑定UAV 要不PS无法使用
	ID3D11UnorderedAccessView *pNullUAV = NULL;
	context->CSSetUnorderedAccessViews(0, 1, &pNullUAV, &UAVInitialCounts);


	return true;
}

void Pref_PBR_DS_IS_IBL_Scene::Init_SimpleShader()
{
	HRESULT hr = S_FALSE;

	static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};

	hr = m_shaderLoader->CreateVertexShader_IA(L"ShaderResources\\SimpleShader_VS.hlsl", "main", "vs_5_0",
		m_simpleVS,
		vertexDesc,
		ARRAYSIZE(vertexDesc),
		m_simpleIL);

	hr = m_shaderLoader->CreatePixelShader(L"ShaderResources\\SimpleShader_PS.hlsl", "main", "ps_5_0",
		m_simplePS);

	//

	Vertex_PosTex* pvertices = (Vertex_PosTex*)malloc(sizeof(Vertex_PosTex) * 4);
	Vertex_PosTex v0 = { XMFLOAT3(1.0f,1.0f,0.0f),XMFLOAT2(1.0f,0.0f) };
	Vertex_PosTex v1 = { XMFLOAT3(-1.0f,1.0f,0.0f),XMFLOAT2(0.0f,0.0f) };
	Vertex_PosTex v2 = { XMFLOAT3(-1.0f,-1.0f,0.0f),XMFLOAT2(0.0f,1.0f) };
	Vertex_PosTex v3 = { XMFLOAT3(1.0f,-1.0f,0.0f),XMFLOAT2(1.0f,1.0f) };


	const Vertex_PosTex vertices[4] = {
		v0,v3,v2,v1

	};

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = vertices;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(4 * sizeof(Vertex_PosVernor), D3D11_BIND_VERTEX_BUFFER);
	m_dxResources->GetD3DDevice()->CreateBuffer(
		&vertexBufferDesc,
		&vertexBufferData,
		&m_simpleVB

	);

	unsigned int indices[6] = {
		0,1,2,2,3,0
	};

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = indices;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC indexBufferDesc(6 * sizeof(unsigned int), D3D11_BIND_INDEX_BUFFER);
	m_dxResources->GetD3DDevice()->CreateBuffer(
		&indexBufferDesc,
		&indexBufferData,
		&m_simpleIB

	);

}

void Pref_PBR_DS_IS_IBL_Scene::Render_SampleShader()
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
	context->ClearRenderTargetView(m_dxResources->GetBackBufferRenderTargetView(), DirectX::Colors::CornflowerBlue);
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

	// 每个顶点都是 VertexPositionColor 结构的一个实例。
	UINT stride = sizeof(Vertex_PosTex);
	UINT offset = 0;
	context->IASetVertexBuffers(
		0,
		1,
		m_simpleVB.GetAddressOf(),
		&stride,
		&offset
	);

	context->IASetIndexBuffer(
		m_simpleIB.Get(),
		DXGI_FORMAT_R32_UINT, // 每个索引都是一个 32 位无符号整数(short)。
		0
	);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_simpleIL.Get());

	// 附加我们的顶点着色器。
	context->VSSetShader(
		m_simpleVS.Get(),
		nullptr,
		0
	);

	context->PSSetShaderResources(7, 1, &m_SRV_InteBRDF);
	context->PSSetSamplers(0, 1, &sampler);

	// 将常量缓冲区发送到图形设备。
	context->VSSetConstantBuffers(
		0,
		1,
		m_D3D_CB_WVP.GetAddressOf()
	);

	// 附加我们的像素着色器。
	context->PSSetShader(
		m_simplePS.Get(),
		nullptr,
		0
	);



	// 绘制对象。
	context->DrawIndexed(
		6,
		0,
		0
	);
}

/*
void Pref_PBR_DS_IS_IBL_Scene::Render()
{
Render_SampleShader();
}
*/

// 使用顶点和像素着色器呈现一个帧。
void Pref_PBR_DS_IS_IBL_Scene::Render()
{
	if (!m_loadingComplete)
	{
		return;
	}

	auto context = m_dxResources->GetD3DDeviceContext();

	//G过程
	//设置渲染目标
	ID3D11RenderTargetView *const targets_G[2] = { m_RTV_verNorTex.Get() ,m_RTV__AlbedoMetalness.Get()};
	context->OMSetRenderTargets(2, targets_G, m_dxResources->GetDepthStencilView());

	// 清除后台缓冲区和深度模具视图。
	context->ClearRenderTargetView(m_RTV__AlbedoMetalness.Get(), DirectX::Colors::CornflowerBlue);
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
		m_VB_G.GetAddressOf(),
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
	//第二步  着色过程

	// 将视区重置为针对整个屏幕。
	auto viewport = m_dxResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	// 将呈现目标重置为屏幕。
	ID3D11RenderTargetView *const targets[1] = { m_dxResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, m_dxResources->GetDepthStencilView());

	// 清除后台缓冲区和深度模具视图。
	context->ClearRenderTargetView(m_dxResources->GetBackBufferRenderTargetView(), DirectX::Colors::CornflowerBlue);
	context->ClearDepthStencilView(m_dxResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 准备常量缓冲区以将其发送到图形设备。
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
		m_D3D_CB_Light.Get(), 0, NULL, m_CB_Light_Data.get(), 0, 0
	);

	context->UpdateSubresource(
		m_D3D_CB_PBR.Get(), 0, NULL, m_CB_PBR_Data.get(), 0, 0
	);

	context->UpdateSubresource(
		m_D3D_CB_MatInfo_albedo_metalness.Get(), 0, NULL, m_CB_MatInfo_albedo_metalness_Data.get(), 0, 0
	);

	// 每个顶点都是 VertexPositionColor 结构的一个实例。
	stride = sizeof(Vertex_Pos);
	offset = 0;
	context->IASetVertexBuffers(
		0,
		1,
		m_VB_S.GetAddressOf(),
		&stride,
		&offset
	);

	context->IASetInputLayout(m_IL_S.Get());

	
	//context->PSSetSamplers(0, 1, &sampler);

	// 将常量缓冲区发送到图形设备。
	context->PSSetConstantBuffers(
		0, 1, m_D3D_CB_Light.GetAddressOf()
	);

	context->PSSetConstantBuffers(
		1, 1, m_D3D_CB_PBR.GetAddressOf()
	);

	context->PSSetConstantBuffers(
		2,
		1,
		m_D3D_CB_WVP_Inverse.GetAddressOf()
	);

	context->PSSetConstantBuffers(
		3,
		1,
		m_D3D_CB_WINDOWINFO.GetAddressOf()
	);

	context->PSSetConstantBuffers(
		4,
		1,
		m_D3D_CB_MatInfo_albedo_metalness.GetAddressOf()
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

	//绑定像素着色器资源
	m_dxResources->GetD3DDeviceContext()->PSSetShaderResources(0, 1, &m_SRV_InteBRDF);

	//绑定着色器资源
	context->PSSetShaderResources(7, 1, &m_SRV_verNorTex);

	//绑定光源信息
	context->PSSetShaderResources(8, 1, &m_SRV__AlbedoMetalness);

	// 绘制对象。
	context->DrawIndexed(
		m_indexCount,
		0,
		0
	);
}


