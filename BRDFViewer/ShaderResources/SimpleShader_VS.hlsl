

cbuffer CB_MVP : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;

};

struct VS_IN
{
	float3 pos:POSITION0;
	float2 tex:TEXCOORD0;
};

struct VS_OUT
{
	float4 pos:SV_POSITION;
	float2 tex:TEXCOORD0;

};

VS_OUT VS(VS_IN input) 
{
	VS_OUT output;
	float4 pos = float4(input.pos, 1.0f);

// 将顶点位置转换为投影空间。
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	output.tex = input.tex;

	return output;
}

VS_OUT main(VS_IN input) 
{
	VS_OUT output;
float4 pos = float4(input.pos, 1.0f);

// 将顶点位置转换为投影空间。
pos = mul(pos, model);
pos = mul(pos, view);
pos = mul(pos, projection);
output.pos = pos;

output.tex = input.tex;

return output;
}