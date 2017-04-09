Texture2D tex_w_verNor:register(t0);									//��������
Texture2D tex_albedometalness:register(t1);								//��������

cbuffer CB_MVP : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};

//G-Buffer����-------------------------------------------------------------------------------------------------
//VS in
struct VS_IN_G
{
	float3 pos:POSITION;
	float3 verNor:NORMAL0;
};

//VS out
struct VS_OUT_G
{
	float4 pos:SV_POSITION;
	float3 w_verNor:NORMAL0;	//λ������ռ���
							//float2 tex:TEXCOORD0;

};

//G����VS
VS_OUT_G VS_G(VS_IN_G input)
{
	VS_OUT_G output;

	float4 pos = mul(float4(input.pos, 1.0f), model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	//ת��������ռ� �������ռ���ʱ�����Դͨ������������ռ���
	//output.w_verNor = mul(float4(input.verNor, 0.0f), model);
	output.w_verNor = input.verNor;
	return output;
}


//�ڶ�����ɫ����-------------------------------------------------------------------------------------
//VS in
struct VS_IN_S
{
	float3 pos:POSITION;
};

//VS out
struct VS_OUT_S
{
	float4 pos:SV_POSITION;
};

//��ɫ����VS
VS_OUT_S VS_S(VS_IN_S input)
{
	VS_OUT_S output;

	float4 pos = mul(float4(input.pos, 1.0f), model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;


	return output;
}
//--------------------------------------------------------------------------------


//������ɫ��-------------------------------------------------------------

// ������ GPU ��ִ�ж��㴦��ļ���ɫ����
VS_OUT_G VS(VS_IN_G input)
{
	VS_OUT_G output;
	float4 pos = float4(input.pos, 1.0f);
	output.w_verNor = input.verNor;
	// ������λ��ת��ΪͶӰ�ռ䡣
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	return output;
}

//------------------------------------------------------------------------------------------------------
VS_OUT_G main(VS_IN_G input)
{
	VS_OUT_G output;
	float4 pos = float4(input.pos, 1.0f);
	output.w_verNor = input.verNor;

	// ������λ��ת��ΪͶӰ�ռ䡣
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	return output;
}

