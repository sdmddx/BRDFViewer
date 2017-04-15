// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� SDMCAMERA_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// SDMCAMERA_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#pragma once

using namespace DirectX;
namespace Utilities
{
	class COMMON_API  SdmCamera
	{

		XMFLOAT4 mEye;
		XMFLOAT4 mAt;
		XMFLOAT4 mNorUp;			//��λ��
		XMFLOAT4 mNorRight;			//��λ��
		XMFLOAT4 mNorEyeDir;			//��λ��

		XMMATRIX mRotation;

		XMMATRIX mView;

		XMFLOAT4 mEyeDir;			//ԭʼ����



	public:

		~SdmCamera() = default;
		SdmCamera(const SdmCamera& camera) = delete;

		SdmCamera(const XMVECTOR& eye = XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f), const XMVECTOR& at = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), const XMVECTOR& up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

		XMMATRIX GetView();
		
		XMMATRIX GetInverseView();

		XMFLOAT4 GetEye();

		//��ͷ�任ģʽ-----�����༭������  ������Ҽ�+(����ƶ�����)+(ALT)+(��껬��)
		void TransformModeA(float angle);	//LM+MH

		void TransformModeB(float value);

		void TransformModeC(float angle);

		void TransformModeD(float angle);


		void TransformModeE(float angle);	//LM+MH���Ź۲�����ϵ��X����ת

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





