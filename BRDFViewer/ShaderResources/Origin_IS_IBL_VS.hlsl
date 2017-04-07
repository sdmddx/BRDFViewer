//��ԭʼ����PBR�ķ��䷽�̽�����Ҫ�Բ������
// �洢������ϼ���ͼ�ε��������������Ⱦ���ĳ�����������
//δ���Ż�
cbuffer CB_MVP : register(b0)
{
	matrix model;
	matrix view;								
	matrix projection;

};




// ����������ɫ�������ÿ����������ݡ�
struct VertexShaderInput_PosverNor
{
	float3 pos : POSITION;
	float3 verNor:VERNOR;
};


//������ɫ������ṹ
struct PixelShaderInput_PBR
{
	float4 pos : SV_POSITION;
	float3 posModel:TEXCOORD0;
	float3 verNor:TEXCOORD1;
};

//������ɫ��-------------------------------------------------------------

// ������ GPU ��ִ�ж��㴦��ļ���ɫ����
PixelShaderInput_PBR VS(VertexShaderInput_PosverNor input)
{
	PixelShaderInput_PBR output;
	float4 pos = float4(input.pos, 1.0f);
	output.verNor = input.verNor;
	output.posModel = input.pos.xyz;

	// ������λ��ת��ΪͶӰ�ռ䡣
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	return output;
}

// ������ GPU ��ִ�ж��㴦��ļ���ɫ����
PixelShaderInput_PBR main(VertexShaderInput_PosverNor input)
{
	PixelShaderInput_PBR output;
	float4 pos = float4(input.pos, 1.0f);
	output.verNor = input.verNor;
	output.posModel = input.pos.xyz;

	// ������λ��ת��ΪͶӰ�ռ䡣
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	return output;
}