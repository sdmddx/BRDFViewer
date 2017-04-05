#include "stdafx.h"

#include "Common\Common\Common.h"

using namespace Example;

SphereAppMain::SphereAppMain(const shared_ptr<DX::DXResources>& dxResources) :
	AppMain(dxResources)
{
	dxResources->RegisterDeviceNotify(this);			//�������ڹ��캯����ʹ��thisָ��


	// TODO: �����ҪĬ�ϵĿɱ�ʱ�䲽��ģʽ֮�������ģʽ������ļ�ʱ�����á�
	// ���磬���� 60 FPS �̶�ʱ�䲽�������߼��������:
	/*
	m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);
	*/
}

void SphereAppMain::GetScene(const shared_ptr<Scene::SceneBase>& scene)
{
	AppMain::GetScene(scene);
}

void SphereAppMain::Update()
{
	// ���³�������
	m_timer.Tick([&]()
	{
		// TODO: �����滻ΪӦ�ó������ݵĸ��º�����
		m_scene->Update(m_timer);
	});
}

bool SphereAppMain::Render()
{
	// ���״θ���ǰ�������Գ����κ����ݡ�
	if (m_timer.GetFrameCount() == 0)
	{
		return false;
	}
	auto context = m_dxResources->GetD3DDeviceContext();

	// ����������Ϊ���������Ļ��
	auto viewport = m_dxResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	// ������Ŀ������Ϊ��Ļ��
	ID3D11RenderTargetView *const targets[1] = { m_dxResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, m_dxResources->GetDepthStencilView());

	// �����̨�����������ģ����ͼ��
	context->ClearRenderTargetView(m_dxResources->GetBackBufferRenderTargetView(), DirectX::Colors::CornflowerBlue);
	context->ClearDepthStencilView(m_dxResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// ���ֳ�������
	// TODO: �����滻ΪӦ�ó������ݵ���Ⱦ������

	m_scene->Render();
	//m_dxResources->Present();
	
	return true;
}

void SphereAppMain::StartRenderLoop()
{
	Update();
	if (Render())
	{
		m_dxResources->Present();
	}
	
}

void SphereAppMain::StopRenderLoop()
{
	//m_renderLoopWorker->Cancel();
}

//���豸��ʧʱ��Ҫ�ͷ��豸��Դ
void SphereAppMain::OnDeviceLost()
{
	m_scene->ReleaseDeviceDependentResources();
}

// ���´����豸��Դ
void SphereAppMain::OnDeviceRestored()
{
	m_scene->CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

// �ڴ��ڴ�С����(���磬�豸�������)ʱ����Ӧ�ó���״̬
void SphereAppMain::CreateWindowSizeDependentResources()
{
	
	m_scene->CreateWindowSizeDependentResources();
}
