//��ͳ�ӳ���ɫ Deferred Shading  ��������
//ʹ��Blinn-Phong����ģ��

Texture2D tex_Color:register(t0);		//ģ����ͼ
Texture2D tex_w_verNor:register(t1);	//��������


SamplerState Sampler:register(s0);     

//G-Buffer����-------------------------------------------------------------------------------------------------
//PS in
struct VS_OUT_G
{
	float4 pos:SV_POSITION;
	float3 w_verNor:NORMAL0;	//λ������ռ���
	//float2 tex:TEXCOORD0;

};

//PS out
struct PS_OUT_G
{
	float4 w_verNor:SV_TARGET0;		//����ֻ�洢������Ϣ
};

//G����PS
PS_OUT_G PS_G(VS_OUT_G input) :SV_TARGET
{
	PS_OUT_G output;
	output.w_verNor = (float4(normalize(input.w_verNor),0.0f));
	return output;
}

//��ɫ����----------------------------------------------------------------------------------------------------
//�ڴ��ڴ�С��ͶӰ�任�ı�ʱ����
cbuffer WindowInfo:register(b0)
{
	float height;
	float width;
	float fovAngleY;	//�ӿڴ�ֱ���ӽǶ�
	float aspectRatio;	//�ӿڿ�߱�
	float nearZ;		//��ƽ�����
	float farZ;			//Զƽ�����
	float unused0;
	float unused1;
}

cbuffer WVP_Inverse:register(b1)
{
	matrix view_Inverse;
	matrix model_Inverse;
	float4 viewPos;					//�۲���λ��
	//float unused0_WVP_Inverse;
}

cbuffer LightsInfo:register(b2)
{
	uint numLights;					//��Դ����
	float3 ambientColor;
}

//PS in
struct VS_OUT_S
{
	float4 pos:SV_POSITION;
	//float2 tex:TEXCOORD0;
};

struct PointLightInfo
{
	float3 lightPos;
	float3 lightColor;
};

StructuredBuffer<PointLightInfo> light:register(t2);

//������ģ��
float Lambert(float3 l, float3 n)
{
	return dot(l, n);
}

//Phongģ��
static const float ex_Phong = 2.0f;
float Phong(float3 l, float3 n, float3 v)
{
	return pow(max((2 * dot(l, n)*n - l)*v,0.0000001), ex_Phong);

}

//Bling-Phongģ��
const uniform float ex_BP = 2.0f;
float Bling_Phong(float3 l, float3 n, float3 v)
{
	return pow(max(normalize(l+v)*n, 0.0000001f), ex_BP);
}

//��ɫ����PS
float4 PS_S(VS_OUT_S input):SV_TARGET
{

	float2 tex = float2(input.pos.x/width,input.pos.y/height);
	//���ͶӰ�ռ��е�xy����
	float2 posInProj = float2(tex.x*2.0f - 1.0f, -tex.y*2.0f + 1.0f);
	
	//��ù۲�ռ��еĶ�������
	float z = input.pos.w;	//����ľ������
	float zMulTanFovAngleY = z*tan(fovAngleY);
	float4 pos = float4(posInProj.x*aspectRatio*zMulTanFovAngleY, posInProj.y*zMulTanFovAngleY, z, 1.0f);

	//�������ռ��еĶ�������
	pos = mul(pos, view_Inverse);
	//��������
	float3 w_verNor = normalize(tex_w_verNor.Sample(Sampler,tex).xyz);

	//���й��ռ��� 
	
	float3 colorDiffuse = (0.0f, 0.0f, 0.0f);
	float3 colorSpecular = (0.0f, 0.0f, 0.0f);

	float numDiffuse = 0;
	float numSpecular = 0;

	for (uint index = 0; index < numLights; ++index)
	{
		float3 l = normalize(light[index].lightPos - pos.xyz);
		float3 v = normalize(viewPos.xyz - pos.xyz);
		float3 h = normalize(l + v);
		float NDotL = dot(w_verNor, l);
		float NDotH = dot(w_verNor, h);
		//������
		if (NDotL > 0.0f)
		{
			colorDiffuse += NDotL*light[index].lightColor;
			numDiffuse++;
		}

		//BP ����
		if (NDotH > 0.0f)
		{
			colorSpecular += pow(NDotH, 1.0f)*light[index].lightColor;
			++numSpecular;
		}
	
	}

	numDiffuse = max(numDiffuse, 1);
	numSpecular = max(numSpecular, 1);
	float3 color = (colorDiffuse / numDiffuse + colorSpecular / numSpecular);

	return saturate(float4(color,0.0f));

	

}


//----------------------------------------------------------------------------------------------------------------
float4 main(VS_OUT_G input) : SV_TARGET
{
	return float4(input.w_verNor,1.0f);
}
