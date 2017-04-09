//使用Blinn-Phong光照模型
//传统延迟着色 Deferred Shading  绘制两次
//场景包含多个光源  这里是点光源



//CB
cbuffer CB_MVP:register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};

//G-Buffer过程-------------------------------------------------------------------------------------------------
//VS in
struct VS_IN_G
{
	float3 pos:POSITION;
	float3 verNor:NORMAL0;
	//float2 tex:TEXCOORD0;
};

//VS out
struct VS_OUT_G
{
	float4 pos:SV_POSITION;
	float3 w_verNor:NORMAL;	//位于世界空间下
	//float2 tex:TEXCOORD0;
	
};

//G过程VS
VS_OUT_G VS_G(VS_IN_G input)
{
	VS_OUT_G output;

	float4 pos = mul(float4(input.pos, 1.0f), model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	//转换到世界空间 ？？光照计算时多个光源通常定义在世界空间下
	output.w_verNor = mul(float4(input.verNor, 0.0f), model);
	//output.tex = input.tex;

	return output;
}


//第二步着色过程-------------------------------------------------------------------------------------
//VS in
struct VS_IN_S
{
	float3 pos:POSITION;
	//float2 tex:TEXCOORD0;
};

//VS out
struct VS_OUT_S
{
	float4 pos:SV_POSITION;
	//float2 tex:TEXCOORD0;
};

//着色过程VS
VS_OUT_S VS_S(VS_IN_S input)
{
	VS_OUT_S output;

	float4 pos = mul(float4(input.pos, 1.0f), model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	//output.tex = input.tex;

	return output;
}

//-----------------------------------------------------------------------------------------------
VS_OUT_G main(VS_IN_G input)
{
	VS_OUT_G output;

	float4 pos = mul(float4(input.pos, 1.0f), model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	output.w_verNor = input.verNor;
	//output.tex = input.tex;

	return output;
}



