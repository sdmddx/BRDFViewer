
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
	float3 verNor:NORMAL0;
};

//像素着色器输入结构
struct PixelShaderInput_PBR
{
	float4 pos : SV_POSITION;
	float3 verNor:NORMAL0;

};

//=-------------------------------------------------------------------------------


//顶点着色器-------------------------------------------------------------

// 用于在 GPU 上执行顶点处理的简单着色器。
PixelShaderInput_PBR VS(VertexShaderInput_PosverNor input)
{
	PixelShaderInput_PBR output;
	float4 pos = float4(input.pos, 1.0f);
	output.verNor = input.verNor;
	// 将顶点位置转换为投影空间。
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	return output;
}


PixelShaderInput_PBR main(VertexShaderInput_PosverNor input)
{
	PixelShaderInput_PBR output;
	float4 pos = float4(input.pos, 1.0f);
	output.verNor = input.verNor;

	// 将顶点位置转换为投影空间。
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	return output;
}

