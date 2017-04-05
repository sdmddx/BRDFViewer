#pragma once

#include "Common\Common\Common.h"

namespace Example
{
	using namespace std;
	class SphereAppMain :public App::AppMain
	{

	public:
		SphereAppMain(const shared_ptr<DX::DXResources>& dxResources);

		//���Ҫ��ʾ�ĳ���
		virtual void GetScene(const shared_ptr<Scene::SceneBase>& scene)override;

		virtual void Update()override;
		virtual bool Render()override;
		virtual void StartRenderLoop()override;
		virtual void StopRenderLoop()override;

		//�ڻָ��豸ʱ�����������Ĵ��ڴ�С�����Դ
		virtual void CreateWindowSizeDependentResources();

		virtual void OnDeviceLost()override;
		virtual void OnDeviceRestored()override;	
	private:
		//shared_ptr<SphereScene> m_SphereScene;

		// ��Ⱦѭ����ʱ����
		Utilities::StepTimer m_timer;



	};

}