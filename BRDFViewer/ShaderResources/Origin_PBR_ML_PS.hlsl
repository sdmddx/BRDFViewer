

//�����������ʺ;��淴����
cbuffer matInfo:register(b0)
{
	float3 diffuse_intensity;
	float unused0_matInfo;
	float3 specular_intensity;
	float unused1_matInfo;
};


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
	float3 baseColor;				            //��ɫ    ��metallic��ֵ��������ǿ�Ⱥ;��淴��ǿ��			
}


cbuffer DFGChoice:register(b4)
{
	uint Dchoice;
	uint Fchoice;
	uint Gchoice;
	uint unused0_DFGChoice;
}

//D--------------------------------------------------------------------------------------------
float D_GGX(float roughness, float NDotH)
{
	float alpha = roughness*roughness;
	return 0.318f* alpha*alpha / pow(NDotH*NDotH*(alpha*alpha - 1.0f) + 1.0f, 2.0f);
}

float D_GTR(float roughness, float NDotH)
{
	float cosHSqr = NDotH*NDotH;
	return D_GTR_C / pow((roughness*roughness*cosHSqr + 1.0f - cosHSqr), D_GTR_N);
}

//F-----------------------------------------------------------------------------------------
float3 F_Schlick(float3 specularColor, float LDotH)
{
	float Fc = pow(1.0f - LDotH, 5.0f);
	return (1.0f - Fc)*specularColor + Fc;
}

float3 F_Schlick_SG(float3 specularColor, float LDotH)
{
	float Fc = pow(2.0f, (-5.55473f*LDotH - 6.98316f)*LDotH);
	return  (1.0f - Fc)*specularColor + Fc;
}

//G------------------------------------------------------------------------------------------
float G_Schlick(float roughness, float NDotL, float NDotV)
{
	float k = 0.5f + (0.5f*roughness);
	//float k = pow((roughness + 1.0f), 2.0f) / 8.0f;
	//float Gl = 1.0f / (1.0f - k + k / NDotL);
	//float Gv = 1.0f / (1.0f - k + k / NDotV);
	return 1.0f / (1.0f - k + k / NDotL) / (1.0f - k + k / NDotV);
}

//specular BRDF����---------------------------------------------------------------------------
float3 Specular_BRDF(float roughness, float LDotH, float NDotH, float NDotL, float NDotV, float3 specularColor)
{
	float D = 0.0f;
	float3 F = float3(0.0f, 0.0f, 0.0f);
	float G = 0.0f;
	//ѡ��D
	if (Dchoice == 0)
	{
		D = D_GGX(roughness, NDotH);
	}
	else if (Dchoice == 1)
	{
		D = D_GTR(roughness, NDotH);
	}

	//ѡ��F
	if (Fchoice == 0)
	{
		F = F_Schlick(specularColor, LDotH);
	}
	else if (Fchoice == 1)
	{
		F = F_Schlick_SG(specularColor, LDotH);
	}

	//ѡ��G
	if (Gchoice == 0)
	{
		G = G_Schlick(roughness, NDotL, NDotV);
	}

	return D*F*G / (4.0f*NDotL*NDotV);
}

//�ڶ��㾵�����
float3 Specular_2_BRDF(float LDotH, float NDotH, float NDotL, float NDotV, float3 specularColor)
{
	return D_GGX(clearCoatGloss, NDotH)*F_Schlick(specularColor, LDotH)*G_Schlick(0.25f, NDotL, NDotV) / (4.0f*NDotL*NDotV) + clearCoat;		//F�����
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

//lambert������ģ��-----------------------------------------------------------------------------
//float Lambertian(float3 l, float3 v)
float Lambertian()
{
	return 0.318f;
}

cbuffer WVP_Inverse:register(b2)
{
	matrix view_Inverse;
	matrix model_Inverse;
	float4 viewPos;								//�۲���λ��  λ����������ϵ��
												//float unused0_WVP_Inverse;
}

cbuffer LightsInfo:register(b3)
{
	uint numLights;								//��Դ����
	float3 ambientColor;
}




struct PointLightInfo
{
	float3 lightPos;
	float3 lightColor;
};

StructuredBuffer<PointLightInfo> light:register(t0);

//����PBR���۵�forward rendering��������ɫ��----------------------------------------------------
struct PS_IN
{
	float4 pos : SV_POSITION;
	float3 posWorld:TEXCOORD0;
	float3 verNor:TEXCOORD1;
};

float4 PS(PS_IN input) : SV_TARGET
{
	float3 n = normalize(input.verNor);
	float3 v = normalize(viewPos.xyz - input.posWorld);

	float3 specularColor = float3(0.0f, 0.0f, 0.0f);

	for (uint index = 0; index < numLights; ++index)
	{
		float3 l = normalize(light[index].lightPos - input.posWorld);
		float3 h = normalize(l + v);

		//������Ϣ
		float LDotH = dot(l, h);
		float NDotH = dot(n, h);
		float NDotL = dot(n, l);
		float NDotV = dot(n, v);

		if (NDotL > 0.0f)
		{
			//specularColor += light[index].lightColor*Specular_BRDF(roughness, LDotH, NDotH, NDotL, NDotV, specular_intensity);
			//specularColor += light[index].lightColor*(Specular_BRDF(roughness, LDotH, NDotH, NDotL, NDotV, specular_intensity)+ diffuse_intensity*NDotL);
			specularColor += light[index].lightColor*(Specular_BRDF(roughness, LDotH, NDotH, NDotL, NDotV, specular_intensity) + diffuse_intensity*Diffuse_DisneyPrincipledBRDF(LDotH,NDotL,NDotV));


		}
	}	


	return saturate(float4(specularColor,1.0f));
}

float4 main(PS_IN input) : SV_TARGET
{
	float3 n = normalize(input.verNor);
	float3 v = normalize(viewPos.xyz - input.posWorld);

	float3 specularColor = float3(0.0f, 0.0f, 0.0f);

	//��baseColor��Metallic��ֵ�õ�������ǿ�Ⱥ;��淴��ǿ��
	float3 diffuseIntensity = baseColor*(1.0f - metallic);
	float3 specularIntensity = lerp(specular,lerp(0.04f, baseColor, metallic), specularTint);

	//float3 specularIntensity = lerp(specular, baseColor, specularTint);

	for (uint index = 0; index < numLights; ++index)
	{
		float3 l = normalize(light[index].lightPos - input.posWorld);
		float3 h = normalize(l + v);

		//������Ϣ
		float LDotH = dot(l, h);
		float NDotH = dot(n, h);
		float NDotL = dot(n, l);
		float NDotV = dot(n, v);

		if (NDotL > 0.0f)
		{
			//specularColor += light[index].lightColor*Specular_BRDF(roughness, LDotH, NDotH, NDotL, NDotV, specular_intensity);
			specularColor += light[index].lightColor*(Specular_BRDF(roughness, LDotH, NDotH, NDotL, NDotV, specularIntensity) + Specular_2_BRDF(LDotH, NDotH, NDotL, NDotV, specularIntensity) + diffuse_intensity*Diffuse_DisneyPrincipledBRDF(LDotH, NDotL, NDotV));

		}
	}
	//return float4(specularIntensity, 1.0f);
	//��׼ȷ������
	float c = max(max(specularColor.x, specularColor.y), specularColor.z);
	if (c > 1.0f)
	{	
		return float4(specularColor / c,1.0f);
	}
	return float4(specularColor, 1.0f);
}


