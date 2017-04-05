// 存储用于组合几何图形的三个基本列优先矩阵的常量缓冲区。
cbuffer CB_MVP : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;

};

// 用作顶点着色器输入的每个顶点的数据。
struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 color : COLOR0;
};

// 通过像素着色器传递的每个像素的颜色数据。
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 color : COLOR0;
};

// 用于在 GPU 上执行顶点处理的简单着色器。
PixelShaderInput main_(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos = float4(input.pos, 1.0f);

	// 将顶点位置转换为投影空间。
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	// 不加修改地传递颜色。
	output.color = input.color;

	return output;
}


//---------------------------------------------------------

// 用作顶点着色器输入的每个顶点的数据。
struct VertexShaderInput_Pos
{
	float3 pos : POSITION;
};

// 通过像素着色器传递的每个像素的颜色数据。---------顶点着色器输出结构
struct PixelShaderInput_Pos
{
	float4 pos : SV_POSITION;
};

// 用于在 GPU 上执行顶点处理的简单着色器。
PixelShaderInput_Pos main_Pos(VertexShaderInput_Pos input)
{
	PixelShaderInput_Pos output;
	float4 pos = float4(input.pos, 1.0f);
	// 将顶点位置转换为投影空间。
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;


	return output;
}



//--------------------------------------------------------------


// 用作顶点着色器输入的每个顶点的数据。
struct VertexShaderInput_PosverNor
{
	float3 pos : POSITION;
	float3 verNor:VERNOR;
};

// 通过像素着色器传递的每个像素的颜色数据。---------顶点着色器输出结构
struct PixelShaderInput_PosverNor
{
	float4 pos:SV_POSITION;
	float3 verNor:TEXCOORD0;

};

struct PixelShaderInput_PBR
{
	float4 pos : SV_POSITION;
	float3 posModel:TEXCOORD0;
	float3 verNor:TEXCOORD1;
};

// 用于在 GPU 上执行顶点处理的简单着色器。
PixelShaderInput_PosverNor main__(VertexShaderInput_PosverNor input)
{
	PixelShaderInput_PosverNor output;
	float4 pos = float4(input.pos, 1.0f);
	output.verNor = input.verNor;

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
