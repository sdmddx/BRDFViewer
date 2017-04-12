#include "stdafx.h"

#include <vector>
#include <d3dx11async.h>
#include <d3dcompiler.h>


using namespace Example;

Origin_PBR_ML_Scene::Origin_PBR_ML_Scene(const std::shared_ptr<DX::DXResources>& dxResources) :
	m_dxResources(dxResources),
	m_loadingComplete(false)
{

	m_shaderLoader = unique_ptr<Utilities::ShaderLoader>(new Utilities::ShaderLoader(dxResources));
	m_dxBufferCreator = Utilities::DXBufferCreator::Create();

	m_CB_WVP_Data = make_unique<CB_WVP>();
	m_CB_PBR_Data = make_unique<CB_PBR>();
	m_CB_WVP_Inverse_Data = make_unique<CB_WVP_Inverse>();
	m_CB_LightsInfo_Data = make_unique<LightsInfo>();
	m_CB_MatInfo_Data = make_unique<CB_MatInfo>();
	m_CB_DFGChoice_Data = make_unique<CB_DFGChoice>();

	numLights = 1;

	CreateWindowSizeDependentResources();
	CreateDeviceDependentResources();

	m_isLButtonDown = false;
	m_isRButtonDown = false;

	m_mouseWheel = 0;
}

Origin_PBR_ML_Scene::~Origin_PBR_ML_Scene()
{
	ReleaseDeviceDependentResources();
}
// 当窗口的大小改变时初始化视图参数。
void Origin_PBR_ML_Scene::CreateWindowSizeDependentResources()
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

	XMStoreFloat4x4(&m_CB_WVP_Inverse_Data->view_Inverse, XMMatrixTranspose(m_camera->GetInverseView()));
	m_CB_WVP_Inverse_Data->viewPos = m_camera->GetEye();

	XMVECTOR deter;
	XMMATRIX model_inverse = XMMatrixInverse(&deter, model);
	XMStoreFloat4x4(&m_CB_WVP_Inverse_Data->model_Inverse, XMMatrixTranspose(model_inverse));

	m_CB_PBR_Data->roughness = 0.2f;
	m_CB_PBR_Data->metalic = 0.7f;
	m_CB_PBR_Data->specular = 0.08f;
	m_CB_PBR_Data->specularTint = 1.0f;
	m_CB_PBR_Data->clearCoat = 0.25f;
	m_CB_PBR_Data->clearCoatGloss = 0.5f;
	m_CB_PBR_Data->D_GTR_N = 2.0f;
	m_CB_PBR_Data->D_GTR_C = 0.25f;
	m_CB_PBR_Data->anisotropic = 1.0f;
	m_CB_PBR_Data->baseColor = XMFLOAT3{ 1.0f,1.0f,1.0f };

	m_CB_MatInfo_Data->diffuse = XMFLOAT3{ 1.0f,0.0f,0.0f };
	m_CB_MatInfo_Data->specular = XMFLOAT3{ 0.9f,0.9f,0.9f };

	m_CB_DFGChoice_Data->Dchoice = 0;
	m_CB_DFGChoice_Data->Fchoice = 0;
	m_CB_DFGChoice_Data->Gchoice = 0;
}

//根据设备对象创建当前场景需要的着色器 模型 纹理 等资源
void Origin_PBR_ML_Scene::CreateDeviceDependentResources()
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

	hr = m_shaderLoader->CreateVertexShader_IA(L"ShaderResources\\Origin_PBR_ML_VS.hlsl", "VS", "vs_5_0",
		m_vertexShader,
		vertexDesc,
		ARRAYSIZE(vertexDesc),
		m_inputLayout);

	hr = m_shaderLoader->CreatePixelShader(L"ShaderResources\\Origin_PBR_ML_PS.hlsl", "main", "ps_5_0",
		m_pixelShader);

	CD3D11_BUFFER_DESC CB_MVP_Desc(sizeof(CB_MVP), D3D11_BIND_CONSTANT_BUFFER);
	m_dxResources->GetD3DDevice()->CreateBuffer(
		&CB_MVP_Desc,
		nullptr,
		&m_D3D_CB_WVP

	);
	CD3D11_BUFFER_DESC CB_MVP_Inverse_Desc(sizeof(CB_WVP_Inverse), D3D11_BIND_CONSTANT_BUFFER);
	m_dxResources->GetD3DDevice()->CreateBuffer(
		&CB_MVP_Inverse_Desc,
		nullptr,
		&m_D3D_CB_WVP_Inverse);

	CD3D11_BUFFER_DESC CB_LightsInfo_Desc(sizeof(CB_WINDOWINFO), D3D11_BIND_CONSTANT_BUFFER);
	m_dxResources->GetD3DDevice()->CreateBuffer(
		&CB_LightsInfo_Desc,
		nullptr,
		&m_D3D_CB_LightsInfo);

	m_dxBufferCreator->CreateContantBuffer<CB_MatInfo>(m_dxResources->GetD3DDevice(), &m_D3D_CB_MatInfo);

	m_dxBufferCreator->CreateContantBuffer<CB_DFGChoice>(m_dxResources->GetD3DDevice(), &m_D3D_CB_DFGChoice);

	CD3D11_BUFFER_DESC constantBufferDesc_PBR(sizeof(CB_PBR), D3D11_BIND_CONSTANT_BUFFER);
	m_dxResources->GetD3DDevice()->CreateBuffer(
		&constantBufferDesc_PBR,
		nullptr,
		&m_D3D_CB_PBR

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

	//创建光源信息结构
	srand(10);
	LightInfo* plightInfo = (LightInfo*)malloc(sizeof(LightInfo)*numLights);
	for (UINT index = 0; index < numLights; ++index)
	{

		//产生随机光源信息
		//光源xyz坐标在（-10，10)区间
		auto a = [index](float multiple = 1.0f) {
			//光源位置生成方法  
			return XMFLOAT3(float(rand() - 16384) / 16384.0f*multiple, float(rand() - 16384) / 16384.0f*multiple, float(rand() - 16384) / 16384.0f*multiple);
		};
		auto b = []() {
			//光源颜色生成方法
			//(0,1)随机数
			return XMFLOAT3(float(rand()) / 32767.0f, float(rand()) / 32767.0f, float(rand()) / 32767.0f);
		};
		plightInfo[index].lightPos = a(10.0f);
		plightInfo[index].lightColor = b();

	}

	ID3D11Buffer* pbuffer_LightInfo;
	m_dxBufferCreator->CreateStructuredBuffer<LightInfo>(m_dxResources->GetD3DDevice(), numLights, &pbuffer_LightInfo,
		&m_SRV_LightInfo, plightInfo);

	m_CB_LightsInfo_Data->numLights = numLights;
	m_CB_LightsInfo_Data->ambientColor = XMFLOAT3(1.0f, 0.0f, 0.0f);

	m_loadingComplete = true;

}

void Origin_PBR_ML_Scene::Update(Utilities::StepTimer const& timer)
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
	//最后更新观察矩阵及其逆矩阵
	XMStoreFloat4x4(&m_CB_WVP_Data->view, XMMatrixTranspose(m_camera->GetView()));
	XMStoreFloat4x4(&m_CB_WVP_Inverse_Data->view_Inverse, XMMatrixTranspose(m_camera->GetInverseView()));
	m_CB_WVP_Inverse_Data->viewPos = m_camera->GetEye();
}

void Origin_PBR_ML_Scene::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_pixelShader.Reset();
	m_D3D_CB_WVP.Reset();
	m_D3D_CB_PBR.Reset();
	m_D3D_CB_WVP_Inverse.Reset();
	m_D3D_CB_LightsInfo.Reset();
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();
	m_D3D_CB_MatInfo.Reset();
	m_D3D_CB_DFGChoice.Reset();
	m_SRV_LightInfo->Release();

}

// 使用顶点和像素着色器呈现一个帧。
void Origin_PBR_ML_Scene::Render()
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

	context->UpdateSubresource(
		m_D3D_CB_WVP_Inverse.Get(),
		0,
		NULL,
		m_CB_WVP_Inverse_Data.get(),
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

	context->UpdateSubresource(
		m_D3D_CB_MatInfo.Get(), 0, NULL, m_CB_MatInfo_Data.get(), 0, 0
	);

	context->UpdateSubresource(
		m_D3D_CB_PBR.Get(), 0, NULL, m_CB_PBR_Data.get(), 0, 0
	);

	context->UpdateSubresource(
		m_D3D_CB_DFGChoice.Get(), 0, NULL, m_CB_DFGChoice_Data.get(), 0, 0
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
		0,
		1,
		m_D3D_CB_MatInfo.GetAddressOf()
	);

	context->PSSetConstantBuffers(
		2,
		1,
		m_D3D_CB_WVP_Inverse.GetAddressOf()
	);

	context->PSSetConstantBuffers(
		3,
		1,
		m_D3D_CB_LightsInfo.GetAddressOf()
	);

	context->PSSetConstantBuffers(
		4,
		1,
		m_D3D_CB_DFGChoice.GetAddressOf()
	);

	context->PSSetConstantBuffers(
		1, 1, m_D3D_CB_PBR.GetAddressOf()
	);

	// 附加我们的像素着色器。
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
	);

	//绑定光源信息
	context->PSSetShaderResources(0, 1, &m_SRV_LightInfo);


	// 绘制对象。
	context->DrawIndexed(
		m_indexCount,
		0,
		0
	);
}