#pragma once

#pragma once
#include "DirectXMath.h"

using namespace DirectX;

// �����򶥵���ɫ������ MVP ����ĳ�����������
struct CB_MVP
{
	DirectX::XMFLOAT4X4 model;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};

struct CB_WVP
{
	XMFLOAT4X4 model;
	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;


};

struct CB_WINDOWINFO
{
	float height;
	float width;
	float fovAngleY;	//�ӿڴ�ֱ���ӽǶ�
	float aspectRatio;	//�ӿڿ�߱�
	float nearZ;		//��ƽ�����
	float farZ;			//Զƽ�����
	float unused0;
	float unused1;
};

struct CB_WVP_Inverse
{
	XMFLOAT4X4 view_Inverse;
	XMFLOAT4 viewPos;
};

struct LightInfo
{
	XMFLOAT3 lightPos;
	XMFLOAT3 lightColor;
};

struct LightsInfo
{
	UINT numLights;
	XMFLOAT3 ambientColor;
};

struct CB_Light
{
	XMFLOAT4 baseColor;							//��ɫ
	XMFLOAT3 lightPos;							//��Դλ��
	float unused0;
	XMFLOAT3 viewPos;							//�ӵ㣨�۲��ߣ�λ��
	float unused1;

};

struct CB_PBR
{
	float roughness;		//PBR�ֲڶ�
	float metalic;			//PBR������
	float specular;								//��һ�㾵�淴���ǿ��  ȡֵ0.0f��0.08f
	float specularTint;							//specular�Ƿ�����ɫ  1.0fʱΪbasecolor 0.0fʱΪ��ɫ��������ɫ��
	float clearCoat;							//�ڶ��㾵�淴���ǿ��  ȡֵ0.0f��0.25f
	float clearCoatGloss;						//�ڶ���specularD���roughness
	float D_GTR_C;								//��һ��specularD��GTRģ�͵ĵ�����c
	float D_GTR_N;								//��һ��specularD��GTRģ�͵�n    ͨ��ȡֵ��1.0f��2.0f
	float anisotropic;							//��������
	float unused2;
	float unused3;
	float unused4;

};

// �����򶥵���ɫ������ÿ����������ݡ�
struct VertexPositionColor
{
	XMFLOAT3 pos;
	XMFLOAT3 color;
};

//�������ݸ�ʽ
struct Vertex_PosVernor
{
	XMFLOAT3 pos;
	XMFLOAT3 vernor;
};

struct Vertex_PosverNorTex
{
	XMFLOAT3 pos;
	XMFLOAT3 vernor;
	XMFLOAT2 tex;
};

struct Vertex_PosTex
{
	XMFLOAT3 pos;
	XMFLOAT2 tex;
};

struct Vertex_Pos
{
	XMFLOAT3 pos;

};
