// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 SDMCAMERA_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// SDMCAMERA_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#pragma once

using namespace DirectX;
namespace Utilities
{
	class COMMON_API  SdmCamera
	{

		XMFLOAT4 mEye;
		XMFLOAT4 mAt;
		XMFLOAT4 mNorUp;			//单位化
		XMFLOAT4 mNorRight;			//单位化
		XMFLOAT4 mNorEyeDir;			//单位化

		XMMATRIX mRotation;

		XMMATRIX mView;

		XMFLOAT4 mEyeDir;			//原始向量



	public:

		~SdmCamera() = default;
		SdmCamera(const SdmCamera& camera) = delete;

		SdmCamera(const XMVECTOR& eye = XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f), const XMVECTOR& at = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), const XMVECTOR& up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

		XMMATRIX GetView();
		
		XMMATRIX GetInverseView();

		XMFLOAT4 GetEye();

		//镜头变换模式-----场景编辑器部分  鼠标左右键+(鼠标移动方向)+(ALT)+(鼠标滑轮)
		void TransformModeA(float angle);	//LM+MH

		void TransformModeB(float value);

		void TransformModeC(float angle);

		void TransformModeD(float angle);


		void TransformModeE(float angle);	//LM+MH绕着观察坐标系的X轴旋转

		void TransformModeF(float value);


		void TransformModeG(XMFLOAT4 focus);

		//const XMFLOAT4& GetEyePos();

		void* operator new(size_t size)throw()
		{
			return _aligned_malloc(size, 16);
		}

		void operator delete(void *p)
		{
			_aligned_free(p);
		};
	};

}





