#pragma once

namespace App
{

	class COMMON_API AppMain : public DX::IDeviceNotify
	{
	protected:
		AppMain(const std::shared_ptr<DX::DXResources>& dxResources);

	public:

		virtual void GetScene(const shared_ptr<Scene::SceneBase>& scene);

		// 每帧更新一次应用程序状态
		virtual void Update() = 0;
		virtual bool Render() = 0;
		virtual void StartRenderLoop() = 0;
		virtual void StopRenderLoop() = 0;
		
		

	protected:
		std::shared_ptr<DX::DXResources> m_dxResources;
		std::shared_ptr<Scene::SceneBase> m_scene;
	};
}
