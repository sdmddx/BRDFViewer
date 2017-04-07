#pragma once

#pragma once
#include "DirectXMath.h"

using namespace DirectX;

// 用于向顶点着色器发送 MVP 矩阵的常量缓冲区。
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
	float fovAngleY;	//视口垂直可视角度
	float aspectRatio;	//视口宽高比
	float nearZ;		//近平面距离
	float farZ;			//远平面距离
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
	XMFLOAT4 baseColor;							//基色
	XMFLOAT3 lightPos;							//光源位置
	float unused0;
	XMFLOAT3 viewPos;							//视点（观察者）位置
	float unused1;

};

struct CB_PBR
{
	float roughness;		//PBR粗糙度
	float metalic;			//PBR金属度
	float specular;								//第一层镜面反射的强度  取值0.0f到0.08f
	float specularTint;							//specular是否有颜色  1.0f时为basecolor 0.0f时为无色（定义颜色）
	float clearCoat;							//第二层镜面反射的强度  取值0.0f到0.25f
	float clearCoatGloss;						//第二层specularD项的roughness
	float D_GTR_C;								//第一层specularD项GTR模型的调整量c
	float D_GTR_N;								//第一层specularD项GTR模型的n    通常取值在1.0f到2.0f
	float anisotropic;							//各向异性
	float unused2;
	float unused3;
	float unused4;

};

// 用于向顶点着色器发送每个顶点的数据。
struct VertexPositionColor
{
	XMFLOAT3 pos;
	XMFLOAT3 color;
};

//顶点数据格式
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
