Texture2D<float2> Tex:register(t7);
struct VS_OUT
{
	float4 pos:SV_POSITION;
	float2 tex:TEXCOORD0;

};

SamplerState Sampler:register(s0);

SamplerState sampler_:register(s1)
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;

};


float4 PS(VS_OUT input) : SV_TARGET
{
	return float4(Tex.Sample(Sampler, input.tex),0.0f,1.0f);
}

float4 main(VS_OUT input) : SV_TARGET
{
	//return float4(1.0f, 0.0f, 0.0f, 1.0f);
	return float4(Tex.Sample(Sampler, input.tex),0.0f,1.0f);
}
