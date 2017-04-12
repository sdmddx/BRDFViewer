cbuffer CB_Light : register(b0)
{
	float4 baseColor;							//��ɫ
	float3 lightPos;							//��Դλ��
	float unused0_CB_Light;
	float3 ViewPos;								//�ӵ㣨�۲��ߣ�λ��-----��ת����ģ�Ϳռ���
	float unused1_CB_Light;

}

cbuffer CB_PBR : register(b1)
{
	float roughness;							//PBR�ֲڶ�
	float metallic;								//PBR������
	float specular;								//��һ�㾵�淴���ǿ��  ȡֵ0.0f��0.08f
	float specularTint;							//specular�Ƿ�����ɫ  1.0fʱΪbasecolor 0.0fʱΪ��ɫ��������ɫ��
	float clearCoat;							//�ڶ��㾵�淴���ǿ��  ȡֵ0.0f��0.25f
	float clearCoatGloss;						//�ڶ���specularD���roughness
	float D_GTR_C;								//��һ��specularD��GTRģ�͵ĵ�����c
	float D_GTR_N;								//��һ��specularD��GTRģ�͵�n    ͨ��ȡֵ��1.0f��2.0f
	float anisotropic;							//��������
	float unused0_CB_PBR;
	float unused1_CB_PBR;
	float unused2_CB_PBR;
}

cbuffer WVP_Inverse:register(b2)
{
	matrix view_Inverse;
	matrix model_Inverse;
	float4 viewPos;								//ģ�Ϳռ�Ĺ۲���λ��

}

//�ڴ��ڴ�С��ͶӰ�任�ı�ʱ����
cbuffer WindowInfo:register(b3)
{
	float height;
	float width;
	float fovAngleY;	//�ӿڴ�ֱ���ӽǶ�
	float aspectRatio;	//�ӿڿ�߱�
	float nearZ;		//��ƽ�����
	float farZ;			//Զƽ�����
	float unused0_WindowInfo;
	float unused1_WindowInfo;
}

//������ɫ������ṹ
struct PixelShaderInput_PBR
{
	float4 pos : SV_POSITION;		//��PS��ֵ���ɶ�Ӧλ�õ�����ֵ���Ͻ�Ϊ��0��0��
	float3 verNor:NORMAL0;
};


//�����廷��ͼ
Texture2D cubeMapRight:register(t1);
Texture2D cubeMapLeft:register(t2);
Texture2D cubeMapTop:register(t3);
Texture2D cubeMapBottom:register(t4);
Texture2D cubeMapBack:register(t5);
Texture2D cubeMapFront:register(t6);

SamplerState samLinear:register(s1);
//----------------------------------------------------------------------------------------------
//��û�����ͼҪ��������ͼ�����  �涨+x -x +y -y +z -z����Ϊ0-5   
uint GetSampleTexIndex(float3 l)
{
	float3 upl = l;
	if (l.x < 0.0f) upl.x = -l.x;
	if (l.y < 0.0f) upl.y = -l.y;
	if (l.z < 0.0f) upl.z = -l.z;


	float max = upl.x;
	if (max < upl.y) max = upl.y;
	if (max < upl.z) max = upl.z;
	if (max == upl.x)
	{
		if (l.x > 0) return 0;
		else return 1;
	}
	if (max == upl.y)
	{
		if (l.y > 0) return 2;
		else return 3;
	}

	if (l.z > 0) return 4;
	else return 5;


}

//���������l�ǵ�λ����		��ò��������廷����ͼ��Ĳ���ֵ
float3 GetCubeMapSample(float3 l)
{
	uint index = GetSampleTexIndex(l);



	if (index == 0)
	{
		float3 ll = l / l.x;
		float2 texPos = (ll.yz + 1.0f) / 2.0f;
		float2 test = float2(texPos.y, 1.0f - texPos.x);
		return cubeMapRight.Sample(samLinear, test);
	}

	if (index == 1)
	{
		float3 ll = l / l.x;
		float2 texPos = (ll.yz + 1.0f) / 2.0f;
		float2 test = float2(texPos.y, texPos.x);
		return cubeMapLeft.Sample(samLinear, test);
	}

	if (index == 2)
	{
		float3 ll = l / l.y;
		float2 texPos = (ll.xz + 1.0f) / 2.0f;
		float2 test = float2(texPos.x, 1.0f - texPos.y);
		return cubeMapTop.Sample(samLinear, test);
	}

	if (index == 3)
	{
		float3 ll = l / l.y;
		float2 texPos = (ll.xz + 1.0f) / 2.0f;
		return cubeMapBottom.Sample(samLinear, texPos);
	}

	if (index == 4)
	{
		float3 ll = l / l.z;
		float2 texPos = (ll.xy + 1.0f) / 2.0f;
		float2 test = float2(1.0f - texPos.x, 1.0f - texPos.y);
		return cubeMapBack.Sample(samLinear, test);

	}

	if (index == 5)
	{
		float3 ll = l / l.z;
		float2 texPos = (ll.xy + 1.0f) / 2.0f;
		float2 test = float2(1.0f - texPos.x, texPos.y);
		return cubeMapFront.Sample(samLinear, test);
	}

	return baseColor;
}

//lambert������ģ��-----------------------------------------------------------------------------
//float Lambertian(float3 upl, float3 v)
float Lambertian()
{
	return 0.318f;
}
//BRDF��diffuse����----------------------------------------------------------------------------
//��2��Fresnel������������Ƕȵ�ʱ����Ĺ�������ǿ��
float Diffuse_DisneyPrincipledBRDF(float LDotH, float NDotL, float NDotV)
{
	float oneMinusCosL = 1.0f - NDotL;
	float oneMinusCosV = 1.0f - NDotV;
	float oneMinusCosLSqr = oneMinusCosL*oneMinusCosL;
	float oneMinusCosVSqr = oneMinusCosV*oneMinusCosV;
	float F_D90 = 0.5f + 2.0f*LDotH*LDotH*roughness;
	return 0.318f*(1.0f + (F_D90 - 1.0f)*oneMinusCosLSqr*oneMinusCosLSqr*oneMinusCosL)*(1.0f + (F_D90 - 1.0f)*oneMinusCosVSqr*oneMinusCosVSqr*oneMinusCosV);

}

//---------------------------��IBL��ʹ��BRDF��ɫģ�͵�deferred rendering��������ɫ��--------------------------------------------------------------- 

//

cbuffer matInfo_albedo_metalness:register(b4)
{
	float3 albedo;
	float metalness;
};

//��CS��õ�BRDF��ص� LUT 
Texture2D<float2> tex_IntegrateBRDF:register(t0);
Texture2D tex_w_verNor:register(t7);			//��������
Texture2D tex_albedometalness:register(t8);		//albedometalness����

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
	float4 w_verNor:SV_TARGET0;							//�洢������Ϣ
	float4 albedo_metalness:SV_TARGET1;					//�洢����ǿ�Ⱥͽ�������Ϣ
};

//G����PS
PS_OUT_G PS_G(VS_OUT_G input) :SV_TARGET
{
	PS_OUT_G output;
	output.w_verNor = float4(normalize(input.w_verNor), 0.0f);
	output.albedo_metalness = float4(albedo, metalness);
	return output;
}

//��ɫ����----------------------------------------------------------------------------------------------------
//PS in
struct VS_OUT_S
{
	float4 pos:SV_POSITION;
};

struct PointLightInfo
{
	float3 lightPos;
	float3 lightColor;
};

//��ɫ����PS
float4 PS_S(VS_OUT_S input) :SV_TARGET
{

	float2 tex = float2(input.pos.x / width,input.pos.y / height);
	//���ͶӰ�ռ��е�xy����
	float2 posInProj = float2(tex.x*2.0f - 1.0f, -tex.y*2.0f + 1.0f);

	//��ù۲�ռ��еĶ�������
	float z = input.pos.w;	//����ľ������
	float zMulTanFovAngleY = z*tan(fovAngleY);
	float4 pos = float4(posInProj.x*aspectRatio*zMulTanFovAngleY, posInProj.y*zMulTanFovAngleY, z, 1.0f);

	//�������ռ��еĶ�������
	pos = mul(pos, view_Inverse);

	//���ģ�Ϳռ�Ķ�������
	pos = mul(pos, model_Inverse);

	//��������
	float3 w_verNor = tex_w_verNor.Sample(Sampler,tex).xyz;

	//�������albedometalness
	float4 albedometalness = tex_albedometalness.Sample(Sampler, tex);

	float3 tex_albedo = albedometalness.xyz;
	float tex_metalness = albedometalness.w;
	
	//������ɫ����
	float3 diffuse = tex_albedo*(1.0f - tex_metalness);
	float3 specular = lerp(0.04f, tex_albedo, tex_metalness);
	float3 n = normalize(w_verNor);
	float3 v = normalize((viewPos - pos).xyz);
	float NDotV = dot(n, v);
	float3 rv = 2 * dot(v, n)*n - v;

	float2 EnvBRDF = tex_IntegrateBRDF.Sample(samLinear, float2(roughness, NDotV));

	float3 sampleColor = GetCubeMapSample(rv);
	//float3 sampleColor =float3(1.0f,0.0f,0.0f);

	float4 finalcolor = float4(sampleColor*((specular*EnvBRDF.x + EnvBRDF.y)+diffuse*NDotV), 1.0f);

	return finalcolor;
	//return float4(rv, 1.0f);
}


//----------------------------------------------------------------------------------------------------------------
float4 main(VS_OUT_G input) : SV_TARGET
{
	return float4(input.w_verNor,1.0f);
}



//������ɫ��----------------------------------------------------------------------------------
float4 PS(PixelShaderInput_PBR input) : SV_TARGET
{
	float2 tex = float2(input.pos.x / width,input.pos.y / height);
	//���ͶӰ�ռ��е�xy����
	float2 posInProj = float2(tex.x*2.0f - 1.0f, -tex.y*2.0f + 1.0f);

	//��ù۲�ռ��еĶ�������
	float z = input.pos.w;	//����ľ������
	float zMulTanFovAngleY = z*tan(fovAngleY);
	float4 pos = float4(posInProj.x*aspectRatio*zMulTanFovAngleY, posInProj.y*zMulTanFovAngleY, z, 1.0f);

	//�������ռ��еĶ�������
	pos = mul(pos, view_Inverse);

	//���ģ�Ϳռ�Ķ�������
	pos = mul(pos, model_Inverse);

	float3 n = normalize(input.verNor);
	float3 v = normalize((viewPos - pos).xyz);
	float NDotV = dot(n, v);

	float3 rv = 2 * dot(v, n)*n - v;

	float2 EnvBRDF = tex_IntegrateBRDF.Sample(samLinear,float2(roughness, NDotV));



	return float4(GetCubeMapSample(rv)*(float3(0.9f,0.9f,0.9f)*EnvBRDF.x + EnvBRDF.y), 1.0f);
	//return float4(-ViewPos,0.0f);
}

float4 main(PixelShaderInput_PBR input) : SV_TARGET
{
	return float4(GetCubeMapSample(input.verNor),1.0f);
}