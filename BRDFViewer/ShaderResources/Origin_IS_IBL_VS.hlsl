//对原始基于PBR的反射方程进行重要性采样获得
// 存储用于组合几何图形的三个基本列优先矩阵的常量缓冲区。
//未作优化
cbuffer CB_MVP : register(b0)
{
	matrix model;
	matrix view;								
	matrix projection;

};




// 用作顶点着色器输入的每个顶点的数据。
struct VertexShaderInput_PosverNor
{
	float3 pos : POSITION;
	float3 verNor:VERNOR;
};


//像素着色器输入结构
struct PixelShaderInput_PBR
{
	float4 pos : SV_POSITION;
	float3 posModel:TEXCOORD0;
	float3 verNor:TEXCOORD1;
};

//顶点着色器-------------------------------------------------------------

// 用于在 GPU 上执行顶点处理的简单着色器。
PixelShaderInput_PBR VS(VertexShaderInput_PosverNor input)
{
	PixelShaderInput_PBR output;
	float4 pos = float4(input.pos, 1.0f);
	output.verNor = input.verNor;
	output.posModel = input.pos.xyz;

	// 将顶点位置转换为投影空间。
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	return output;
}

// 用于在 GPU 上执行顶点处理的简单着色器。
PixelShaderInput_PBR main(VertexShaderInput_PosverNor input)
{
	PixelShaderInput_PBR output;
	float4 pos = float4(input.pos, 1.0f);
	output.verNor = input.verNor;
	output.posModel = input.pos.xyz;

	// 将顶点位置转换为投影空间。
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	return output;
}