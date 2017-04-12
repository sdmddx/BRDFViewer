
cbuffer CB_MVP : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;

};


struct VS_IN
{
	float3 pos:POSITION0;
	float3 verNor:NORMAL0;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
	float3 posWorld:TEXCOORD0;
	float3 verNor:TEXCOORD1;
};

PS_IN VS(VS_IN input)
{
	PS_IN output;
	float4 pos = float4(input.pos, 1.0f);
	output.verNor = mul(input.verNor,model);
	

	// 将顶点位置转换为投影空间。
	pos = mul(pos, model);
	output.posWorld = pos.xyz;
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	return output;
}



float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}