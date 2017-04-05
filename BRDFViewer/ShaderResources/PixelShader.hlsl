struct PixelShaderInput_PBR
{
	float4 pos : SV_POSITION;
	float3 posModel:TEXCOORD0;
	float3 verNor:TEXCOORD1;
};


float4 main(PixelShaderInput_PBR input) : SV_TARGET
{
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}