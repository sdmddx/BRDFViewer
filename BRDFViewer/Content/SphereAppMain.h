#pragma once

#include "Common\Common\Common.h"

namespace Example
{
	using namespace std;
	class SphereAppMain :public App::AppMain
	{

	public:
		SphereAppMain(const shared_ptr<DX::DXResources>& dxResources);

		//获得要显示的场景
		virtual void GetScene(const shared_ptr<Scene::SceneBase>& scene)override;

		virtual void Update()override;
		virtual bool Render()override;
		virtual void StartRenderLoop()override;
		virtual void StopRenderLoop()override;

		//在恢复设备时创建各场景的窗口大小相关资源
		virtual void CreateWindowSizeDependentResources();

		virtual void OnDeviceLost()override;
		virtual void OnDeviceRestored()override;	
	private:
		//shared_ptr<SphereScene> m_SphereScene;

		// 渲染循环计时器。
		Utilities::StepTimer m_timer;



	};

}