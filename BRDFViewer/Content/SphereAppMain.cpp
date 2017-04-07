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

	// ���ֳ�������
	m_scene->Render();
	
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
