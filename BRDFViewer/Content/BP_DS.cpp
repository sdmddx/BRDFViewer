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

	numLights = 100;			//�����Դ����

	CreateWindowSizeDependentResources();
	CreateDeviceDependentResources();

	m_isLButtonDown = false;
	m_isRButtonDown = false;

	m_mouseWheel = 0;
}

BP_DS::~BP_DS()
{

}
// �����ڵĴ�С�ı�ʱ��ʼ����ͼ������
void BP_DS::CreateWindowSizeDependentResources()
{
	Math::Size windowSize = m_dxResources->GetWindowSize();
	float aspectRatio = float(windowSize.Width) / float(windowSize.Height);
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// ����һ���򵥵ĸ���ʾ������Ӧ�ó�����������ͼ�������ͼ��ʱ�����Խ��д˸���
	// ��
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// ��ע�⣬OrientationTransform3D �����ڴ˴��Ǻ�˵ģ�
	// ����ȷȷ�������ķ���ʹ֮����ʾ����ƥ�䡣
	// ���ڽ�������Ŀ��λͼ���е��κλ��Ƶ���
	// ����������Ŀ�ꡣ���ڵ�����Ŀ��Ļ��Ƶ��ã�
	// ��ӦӦ�ô�ת����

	// ��ʾ��ʹ�����������������������ϵ��
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

	// �۾�λ��(0,0.7,1.5)�������� Y ��ʹ������ʸ�����ҵ�(0,-0.1,0)��
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

//�����豸���󴴽���ǰ������Ҫ����ɫ�� ģ�� ���� ����Դ
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

	//����G������ɫ�����������벼��
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

	//����S������ɫ�����������벼��
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

	//��������������
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
	
	//���д�����Ϣ��CB
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

	// ����������ɫ���󣬴�������
	unique_ptr<BasicGraphic::SphereGenerator> pSphere = unique_ptr<BasicGraphic::SphereGenerator>(new BasicGraphic::SphereGenerator);

	unique_ptr<AssetMagr::DataBuf>	m_dataBuf;
	m_dataBuf = pSphere->GenerateSpherePvN(4);

	unsigned int vertexCount = m_dataBuf->GetBufMeshByIndex(0)->GetCntVertex();
	//����G���̶��㻺����
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

	//����S���̶��㻺����
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

	//��������������
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

	//��������
	hr = D3DX11CreateShaderResourceViewFromFile(m_dxResources->GetD3DDevice(), L"TextureResources\\sdm.bmp", NULL, 0, &m_SRV_envMap, 0);
	
	//��������ɫ����Դ
	m_dxResources->GetD3DDeviceContext()->PSSetShaderResources(0, 1, &m_SRV_envMap);

	//���ò�����״̬
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

	//����ͼ������
	m_dxResources->GetD3DDeviceContext()->PSSetSamplers(0, 1, &sampler);

	//Ϊ��������ͼ������ȾĿ��
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

	//�����ڶ������̵���ɫ����Դ��ͼ
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

	//������Դ��Ϣ�ṹ
	srand(10);
	LightInfo* plightInfo = (LightInfo*)malloc(sizeof(LightInfo)*numLights);
	for (UINT index = 0; index < numLights; ++index)
	{
	
		//���������Դ��Ϣ
		//��Դxyz�����ڣ�-10��10)����
		auto a = [index]( float multiple=1.0f) {
			//��Դλ�����ɷ���  
			return XMFLOAT3(float(rand()-16384) / 16384.0f*multiple, float(rand() - 16384) / 16384.0f*multiple, float(rand() - 16384) / 16384.0f*multiple);
		};
		auto b = []() {
			//��Դ��ɫ���ɷ���
			//(0,1)�����
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
	else if (m_isRButtonDown)		//���Ҽ�ͬʱ����ʱȷ��������ȼ�����
	{
		long deltaX = m_rightPointMovement.x - m_rightPoint.x;
		long deltaY = m_rightPointMovement.y - m_rightPoint.y;
		m_camera->TransformModeC(deltaX*0.003f);
		m_camera->TransformModeD(deltaY*0.003f);
		m_rightPoint = m_rightPointMovement;
	}

	//������껬�ֹ�����Ϣ
	short sum = m_mouseWheel / 120;
	if (sum)
	{
		float distance = sum / 5.0f;
		m_camera->TransformModeF(distance);
		m_mouseWheel = 0;
	}

	//�����¹۲�����������
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

// ʹ�ö����������ɫ������һ��֡��
void BP_DS::Render()
{
	// �������첽�ġ����ڼ��ؼ���ͼ�κ�Ż��������
	if (!m_loadingComplete)
	{
		return;
	}

	auto context = m_dxResources->GetD3DDeviceContext();

	//������ȾĿ��
	ID3D11RenderTargetView *const targets_Nor[1] = { m_RTV_verNorTex.Get() };
	context->OMSetRenderTargets(1, targets_Nor, m_dxResources->GetDepthStencilView());

	// �����̨�����������ģ����ͼ��
	context->ClearRenderTargetView(m_RTV_verNorTex.Get(), DirectX::Colors::CornflowerBlue);
	context->ClearDepthStencilView(m_dxResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// ׼�������������Խ��䷢�͵�ͼ���豸��
	context->UpdateSubresource(
		m_D3D_CB_WVP.Get(),
		0,
		NULL,
		m_CB_WVP_Data.get(),
		0,
		0
	);

	// ÿ�����㶼�� Vertex_PosverNorTex �ṹ��һ��ʵ����
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
		DXGI_FORMAT_R32_UINT, // ÿ����������һ�� 32 λ�޷�������(short)��
		0
	);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_IL_G.Get());

	// �������ǵĶ�����ɫ����
	context->VSSetShader(
		m_VS_G.Get(),
		nullptr,
		0
	);

	// ���������������͵�ͼ���豸��
	context->VSSetConstantBuffers(
		0,
		1,
		m_D3D_CB_WVP.GetAddressOf()
	);

	// �������ǵ�������ɫ����
	context->PSSetShader(
		m_PS_G.Get(),
		nullptr,
		0
	);

	// ���ƶ���
	context->DrawIndexed(
		m_indexCount,
		0,
		0
	);

	//�ڶ���  ���չ���

	// ����������Ϊ���������Ļ��
	auto viewport = m_dxResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	//������ȾĿ����ͼΪ��Ļ
	ID3D11RenderTargetView *const  targets[1] = { m_dxResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, m_dxResources->GetDepthStencilView());

	// �����̨�����������ģ����ͼ��
	context->ClearRenderTargetView(m_dxResources->GetBackBufferRenderTargetView(), DirectX::Colors::CornflowerBlue);
	context->ClearDepthStencilView(m_dxResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// ÿ�����㶼�� Vertex_PosTex �ṹ��һ��ʵ����
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

	// �������ǵĶ�����ɫ����
	context->VSSetShader(
		m_VS_S.Get(),
		nullptr,
		0
	);

	// �������ǵ�������ɫ����
	context->PSSetShader(
		m_PS_S.Get(),
		nullptr,
		0
	);

	//����ɫ����Դ
	context->PSSetShaderResources(1, 1, &m_SRV_verNorTex);	

	//�󶨹�Դ��Ϣ
	context->PSSetShaderResources(2, 1, &m_SRV_LightInfo);


	// ���ƶ���
	context->DrawIndexed(
		m_indexCount,
		0,
		0
	);


}