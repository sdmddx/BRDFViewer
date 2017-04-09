struct PixelShaderInput_PBR
{
	float4 pos : SV_POSITION;
	float3 posModel:TEXCOORD0;
	float3 verNor:TEXCOORD1;
};


float4 main_(PixelShaderInput_PBR input) : SV_TARGET
{
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}

Texture2D Texture:register(t0);

struct VS_OUT
{
	float4 pos:SV_POSITION;
	float2 tex:TEXCOORD;

};

SamplerState Sampler:register(s0);


float4 main(VS_OUT input) : SV_TARGET
{
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
	//return Texture.Sample(Sampler, input.tex);
}
