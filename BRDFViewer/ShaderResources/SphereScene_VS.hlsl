
cbuffer CB_MVP : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};


// ����������ɫ�������ÿ����������ݡ�
struct VS_IN
{
	float3 pos : POSITION0;
	float3 verNor:NORMAL0;
};

//������ɫ������ṹ
struct PS_IN
{
	float4 pos : SV_POSITION;
	float3 posModel:POSITION0;
	float3 verNor:NORMAL0;

};

PS_IN VS(VS_IN input)
{
	PS_IN output;
	float4 pos = float4(input.pos, 1.0f);
	output.verNor = input.verNor;
	output.posModel = input.pos;
	// ������λ��ת��ΪͶӰ�ռ�
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	return output;
}

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}