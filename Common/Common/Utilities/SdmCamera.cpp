// SdmCamera.cpp : 定义 DLL 应用程序的导出函数。
//
#include "stdafx.h"


using namespace DirectX;
using namespace Utilities;


SdmCamera::SdmCamera(const XMVECTOR& eye, const XMVECTOR& at, const XMVECTOR& up)
{
	XMStoreFloat4(&mEye, eye);
	XMStoreFloat4(&mAt, at);
	XMStoreFloat4(&mEyeDir, XMVectorSubtract(at, eye));
	XMStoreFloat4(&mNorEyeDir, XMVector3Normalize(XMLoadFloat4(&mEyeDir)));
	XMStoreFloat4(&mNorRight, XMVector3Normalize(XMVector3Cross(up, XMLoadFloat4(&mNorEyeDir))));
	XMStoreFloat4(&mNorUp, XMVector3Normalize(-XMVector3Cross(XMLoadFloat4(&mNorRight), XMLoadFloat4(&mNorEyeDir))));
	mView = XMMatrixLookAtRH(XMLoadFloat4(&mEye), XMLoadFloat4(&mAt), XMLoadFloat4(&mNorUp));
}

XMMATRIX SdmCamera::GetView()
{
	mView = XMMatrixLookAtLH(XMLoadFloat4(&mEye), XMLoadFloat4(&mAt), XMLoadFloat4(&mNorUp));
	return mView;
}

 XMMATRIX SdmCamera::GetInverseView()
{
	mView = XMMatrixLookAtLH(XMLoadFloat4(&mEye), XMLoadFloat4(&mAt), XMLoadFloat4(&mNorUp));
	XMVECTOR deter;
	return XMMatrixInverse(&deter, mView);
}

void SdmCamera::TransformModeA(float angle)
{
	//mRotation = XMMatrixTranslationFromVector(XMLoadFloat4(&mEye))*XMMatrixRotationY(angle)*XMMatrixTranslationFromVector(-XMLoadFloat4(&mEye));
	XMStoreFloat4(&mNorRight, XMVector4Transform(XMLoadFloat4(&mNorRight), XMMatrixRotationY(angle)));
	XMStoreFloat4(&mNorUp, XMVector4Transform(XMLoadFloat4(&mNorUp), XMMatrixRotationY(angle)));


	XMStoreFloat4(&mEyeDir, XMVector4Transform(XMLoadFloat4(&mEyeDir), XMMatrixRotationY(angle)));
	XMStoreFloat4(&mNorEyeDir, XMVector4Transform(XMLoadFloat4(&mNorEyeDir), XMMatrixRotationY(angle)));
	XMStoreFloat4(&mAt, (XMLoadFloat4(&mEye) + XMLoadFloat4(&mEyeDir)));

}


void SdmCamera::TransformModeB(float value)
{
	XMVECTOR DirShadow = XMVectorSet(mAt.x - mEye.x, 0.0, mAt.z - mEye.z, 0.0f);
	//SLFLOAT4 Step;
	XMStoreFloat4(&mEye, value*XMVector3Normalize(DirShadow) + XMLoadFloat4(&mEye));
	XMStoreFloat4(&mAt, XMLoadFloat4(&mEye) + XMLoadFloat4(&mEyeDir));

}

void SdmCamera::TransformModeC(float angle)
{
	XMStoreFloat4(&mEyeDir, -XMVector4Transform(-XMLoadFloat4(&mEyeDir), XMMatrixRotationY(angle)));
	XMStoreFloat4(&mNorEyeDir, -XMVector4Transform(-XMLoadFloat4(&mNorEyeDir), XMMatrixRotationY(angle)));
	XMStoreFloat4(&mEye, (XMLoadFloat4(&mAt) - XMLoadFloat4(&mEyeDir)));
	XMStoreFloat4(&mNorRight, XMVector4Transform(XMLoadFloat4(&mNorRight), XMMatrixRotationY(angle)));
	XMStoreFloat4(&mNorUp, XMVector4Transform(XMLoadFloat4(&mNorUp), XMMatrixRotationY(angle)));


}

void SdmCamera::TransformModeD(float angle)
{

	XMMATRIX view = GetView();
	XMVECTOR deter;
	XMStoreFloat4(&mNorRight, XMVector4Transform(XMLoadFloat4(&mNorRight), view*XMMatrixTranslationFromVector(XMLoadFloat4(&mEyeDir))*XMMatrixRotationX(angle)*XMMatrixTranslationFromVector(XMLoadFloat4(&mEyeDir))*XMMatrixInverse(&deter, view)));
	XMStoreFloat4(&mNorUp, XMVector4Transform(XMLoadFloat4(&mNorUp), view*XMMatrixTranslationFromVector(XMLoadFloat4(&mEyeDir))*XMMatrixRotationX(angle)*XMMatrixTranslationFromVector(XMLoadFloat4(&mEyeDir))*XMMatrixInverse(&deter, view)));
	XMStoreFloat4(&mEyeDir, XMVector4Transform(XMLoadFloat4(&mEyeDir), view*XMMatrixTranslationFromVector(XMLoadFloat4(&mEyeDir))*XMMatrixRotationX(angle)*XMMatrixTranslationFromVector(XMLoadFloat4(&mEyeDir))*XMMatrixInverse(&deter, view)));
	XMStoreFloat4(&mNorEyeDir, XMVector4Transform(XMLoadFloat4(&mNorEyeDir), view*XMMatrixTranslationFromVector(XMLoadFloat4(&mEyeDir))*XMMatrixRotationX(angle)*XMMatrixTranslationFromVector(XMLoadFloat4(&mEyeDir))*XMMatrixInverse(&deter, view)));

	//根据不变量FocusPosition求出EyePosition
	XMStoreFloat4(&mEye, (XMLoadFloat4(&mAt) - XMLoadFloat4(&mEyeDir)));


}

void SdmCamera::TransformModeE(float angle)
{
	XMMATRIX view = GetView();
	XMVECTOR deter;
	XMStoreFloat4(&mNorRight, XMVector4Transform(XMLoadFloat4(&mNorRight), view*XMMatrixRotationX(angle)*XMMatrixInverse(&deter, view)));
	XMStoreFloat4(&mNorUp, XMVector4Transform(XMLoadFloat4(&mNorUp), view*XMMatrixRotationX(angle)*XMMatrixInverse(&deter, view)));
	XMStoreFloat4(&mEyeDir, XMVector4Transform(XMLoadFloat4(&mEyeDir), view*XMMatrixRotationX(angle)*XMMatrixInverse(&deter, view)));
	XMStoreFloat4(&mNorEyeDir, XMVector4Transform(XMLoadFloat4(&mNorEyeDir), view*XMMatrixRotationX(angle)*XMMatrixInverse(&deter, view)));

	//根据不变量EyePosition求出FocusPosition
	XMStoreFloat4(&mAt, (XMLoadFloat4(&mEye) + XMLoadFloat4(&mEyeDir)));

}

void SdmCamera::TransformModeF(float value)
{
	XMStoreFloat4(&mEye, value*XMLoadFloat4(&mNorEyeDir) + XMLoadFloat4(&mEye));
	XMStoreFloat4(&mAt, value*XMLoadFloat4(&mNorEyeDir) + XMLoadFloat4(&mAt));

}

void SdmCamera::TransformModeG(XMFLOAT4 focus)
{
	mAt = focus;
	XMStoreFloat4(&mEye, XMLoadFloat4(&mAt) - XMLoadFloat4(&mEyeDir));

}

XMFLOAT4 SdmCamera::GetEye()
{
	return mEye;
}
