// �洢������ϼ���ͼ�ε��������������Ⱦ���ĳ�����������
cbuffer CB_MVP : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;

};

// ����������ɫ�������ÿ����������ݡ�
struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 color : COLOR0;
};

// ͨ��������ɫ�����ݵ�ÿ�����ص���ɫ���ݡ�
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 color : COLOR0;
};

// ������ GPU ��ִ�ж��㴦��ļ���ɫ����
PixelShaderInput main_(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos = float4(input.pos, 1.0f);

	// ������λ��ת��ΪͶӰ�ռ䡣
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	// �����޸ĵش�����ɫ��
	output.color = input.color;

	return output;
}


//---------------------------------------------------------

// ����������ɫ�������ÿ����������ݡ�
struct VertexShaderInput_Pos
{
	float3 pos : POSITION;
};

// ͨ��������ɫ�����ݵ�ÿ�����ص���ɫ���ݡ�---------������ɫ������ṹ
struct PixelShaderInput_Pos
{
	float4 pos : SV_POSITION;
};

// ������ GPU ��ִ�ж��㴦��ļ���ɫ����
PixelShaderInput_Pos main_Pos(VertexShaderInput_Pos input)
{
	PixelShaderInput_Pos output;
	float4 pos = float4(input.pos, 1.0f);
	// ������λ��ת��ΪͶӰ�ռ䡣
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;


	return output;
}



//--------------------------------------------------------------


// ����������ɫ�������ÿ����������ݡ�
struct VertexShaderInput_PosverNor
{
	float3 pos : POSITION;
	float3 verNor:VERNOR;
};

// ͨ��������ɫ�����ݵ�ÿ�����ص���ɫ���ݡ�---------������ɫ������ṹ
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

// ������ GPU ��ִ�ж��㴦��ļ���ɫ����
PixelShaderInput_PosverNor main__(VertexShaderInput_PosverNor input)
{
	PixelShaderInput_PosverNor output;
	float4 pos = float4(input.pos, 1.0f);
	output.verNor = input.verNor;

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
