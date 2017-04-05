#include "stdafx.h"

#include "Common\Common\Common.h"

using namespace Example;

SphereAppMain::SphereAppMain(const shared_ptr<DX::DXResources>& dxResources) :
	AppMain(dxResources)
{
	dxResources->RegisterDeviceNotify(this);			//不建议在构造函数中使用this指针


	// TODO: 如果需要默认的可变时间步长模式之外的其他模式，请更改计时器设置。
	// 例如，对于 60 FPS 固定时间步长更新逻辑，请调用:
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
	// 更新场景对象。
	m_timer.Tick([&]()
	{
		// TODO: 将此替换为应用程序内容的更新函数。
		m_scene->Update(m_timer);
	});
}

bool SphereAppMain::Render()
{
	// 在首次更新前，请勿尝试呈现任何内容。
	if (m_timer.GetFrameCount() == 0)
	{
		return false;
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

	// 呈现场景对象。
	// TODO: 将此替换为应用程序内容的渲染函数。

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

//在设备丢失时需要释放设备资源
void SphereAppMain::OnDeviceLost()
{
	m_scene->ReleaseDeviceDependentResources();
}

// 重新创建设备资源
void SphereAppMain::OnDeviceRestored()
{
	m_scene->CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

// 在窗口大小更改(例如，设备方向更改)时更新应用程序状态
void SphereAppMain::CreateWindowSizeDependentResources()
{
	
	m_scene->CreateWindowSizeDependentResources();
}
