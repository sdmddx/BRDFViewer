#pragma once


namespace Scene
{
	class COMMON_API SceneBase
	{

	public:

		virtual void CreateDeviceDependentResources() = 0;
		virtual void CreateWindowSizeDependentResources() = 0;
		virtual void ReleaseDeviceDependentResources() = 0;
		virtual void Update(Utilities::StepTimer const& timer) = 0;
		virtual void Render() = 0;

	protected:

	};
}
