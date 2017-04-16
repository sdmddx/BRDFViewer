

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
	return 0.318f* alpha*alpha / pow(NDotH*NDotH*(alpha*alpha - 1.0f) + 1.0f, 2.0f);		//��ȡ��(0��1)֮�䣿
}

float D_GTR(float roughness, float NDotH)
{
	float cosHSqr = NDotH*NDotH;
	return D_GTR_C / pow((roughness*roughness*cosHSqr + 1.0f - cosHSqr), D_GTR_N);
}

float D_BlingPhong(float roughness, float NDotH)
{
	float sigma = roughness;
	return (pow(NDotH,2.0f/(sigma*sigma)-2.0f)) / (3.1415926f*sigma*sigma);
}

float D_UEBlingPhong(float roughness, float3 n,float3 vR)
{
	float alpha = roughness;
	float NDotvR = dot(n, vR);
	float exp = 0.5f / alpha / alpha - 0.5f;
	return pow(NDotvR, exp) / (3.1415926f*alpha*alpha);
}

float D_Beckmann(float roughness, float NDotH)
{
	float sigma = roughness;
	float exp = (1.0f - 1.0f / NDotH / NDotH) / sigma / sigma;
	return pow(2.71828f, exp) / (3.1415926f*sigma*sigma*NDotH*NDotH*NDotH*NDotH);
}

float D_Phong(float roughness, float NDotH)
{
	float ap = roughness*4.0f;	//��roughness�任��ap
	return (2.0f+ap)*(pow(NDotH,ap)) / (3.1415926f*2.0f);
}

float D_GGX_Anisotropic(float3 n, float3 h)
{
	//2������ĸ������
	float alphax = 0.1f;
	float alphay = 0.9f;

	//��÷��߿ռ�����ϵ
	float3 UpVector = abs(n.z) < 0.999f ? float3(0, 0, 1) : float3(1, 0, 0);
	float3 TangentX = normalize(cross(UpVector, n));
	float3 TangentY = cross(n, TangentX);

	float matter0 = 3.1415926f*alphax*alphay;
	float matter1 = pow(pow(dot(TangentX, h)/ alphax,2.0f)+pow(dot(TangentY, h) / alphay,2.0f)+ pow(dot(n,h),2.0f) , 2.0f);

	return 1.0f / matter0 / matter1;
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

float3 F_CookTorrance(float3 specularColor, float LDotH)
{
	float3 ita = float3((1.0f+sqrt(specularColor.x)) / (1.0f - sqrt(specularColor.x)), (1.0f + sqrt(specularColor.y)) / (1.0f - sqrt(specularColor.y)), (1.0f + sqrt(specularColor.z)) / (1.0f - sqrt(specularColor.z)));
	float cqrtplusone = LDotH*LDotH - 1.0f;
	float3 g = float3(sqrt(max(ita.x*ita.x+ cqrtplusone,0.0f)), sqrt(max(ita.y*ita.y + cqrtplusone, 0.0f)), sqrt(max(ita.z*ita.z + cqrtplusone, 0.0f)));

	float3 F = float3(0.0f, 0.0f, 0.0f);
	F.x = 0.5f*pow((g.x- LDotH)/(g.x- LDotH), 2.0f)*(1.0f + pow(((g.x+ LDotH)*LDotH -1.0f)/ ((g.x - LDotH)*LDotH + 1.0f), 2.0f));
	F.y = 0.5f*pow((g.y - LDotH) / (g.y - LDotH), 2.0f)*(1.0f + pow(((g.y + LDotH)*LDotH - 1.0f) / ((g.y - LDotH)*LDotH + 1.0f), 2.0f));
	F.z = 0.5f*pow((g.z - LDotH) / (g.z - LDotH), 2.0f)*(1.0f + pow(((g.z + LDotH)*LDotH - 1.0f) / ((g.z - LDotH)*LDotH + 1.0f), 2.0f));
	return  F;
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

float G_Neumann(float NDotL, float NDotV)
{
	return min(NDotL, NDotV);

}

float G_CookTorrance(float3 n, float3 l, float3 v, float3 h)
{
	//������Ϣ
	float LDotH = dot(l, h);
	float NDotH = dot(n, h);
	float NDotL = dot(n, l);
	float NDotV = dot(n, v);

	return min(1.0f, min(2*NDotH*NDotV/ LDotH, 2 * NDotH*NDotL / LDotH));

}

float G_Kelemen(float3 n, float3 l, float3 v, float3 h)
{
	return dot(n,l)*dot(n, v) / dot(v, h)/ dot(v, h);
}

float G_ClassicSchlick(float roughness, float NDotV)
{
	float k = roughness;
	return 1.0f / (1.0f-k+k/ NDotV);

}

float G_SchlickBeckmann(float roughness, float NDotV)
{
	float k = roughness*sqrt(2.0f/3.1415926f);
	return 1.0f / (1.0f - k + k / NDotV);

}

float G_SchlickGGX(float roughness, float NDotV)
{
	float k = roughness/2.0f;
	return 1.0f / (1.0f - k + k / NDotV);

}

float G_UESchlick(float roughness, float NDotL, float NDotV)
{
	float k = (roughness+1.0f)*(roughness + 1.0f)/8.0f;
	//float k = pow((roughness + 1.0f), 2.0f) / 8.0f;
	//float Gl = 1.0f / (1.0f - k + k / NDotL);
	//float Gv = 1.0f / (1.0f - k + k / NDotV);
	return 1.0f / (1.0f - k + k / NDotL) / (1.0f - k + k / NDotV);
}

float G_Beckmann(float roughness, float NDotV)
{
	float c = NDotV / roughness / sqrt(1 - NDotV*NDotV);
	//float k = pow((roughness + 1.0f), 2.0f) / 8.0f;
	//float Gl = 1.0f / (1.0f - k + k / NDotL);
	//float Gv = 1.0f / (1.0f - k + k / NDotV);
	if (c < 1.6)
	{
		return (3.535f+2.181*c) / (1.0f/c+2.276f+2.577*c);
	}
	return 1.0f;
}

float G_GGX(float roughness, float NDotV)
{
	float alpha = roughness;
	return 2.0f*NDotV / (NDotV+sqrt(alpha*alpha+(1.0f- alpha*alpha)*NDotV*NDotV));
}

float G_SmithJointGGX(float roughness, float NDotL, float NDotV)
{
	float alpha = roughness;
	float lambda_v = (sqrt(alpha*alpha*(1.0f /(NDotL*NDotL)-1.0f)+1.0f) - 1.0f) / 2.0f;
	float lambda_l = (sqrt(alpha*alpha*(1.0f / (NDotV*NDotV) - 1.0f) + 1.0f) - 1.0f) / 2.0f;
	return 1.0f / (1.0f + lambda_v + lambda_l);
}

float G_SmithJointGGXImproved(float roughness, float NDotL, float NDotV)
{
	float alpha = roughness;
	float lambda_v = NDotL*(NDotV*(1- alpha)+ alpha);
	float lambda_l = NDotV*(NDotL*(1 - alpha) + alpha);
	return 2.0f / (1.00001f + lambda_v + lambda_l);
}

//specular BRDF����---------------------------------------------------------------------------
float3 Specular_BRDF(float roughness, float3 n, float3 l, float3 v, float3 h, float3 specularColor)
{

	//������Ϣ
	float LDotH = dot(l, h);
	float NDotH = dot(n, h);
	float NDotL = dot(n, l);
	float NDotV = dot(n, v);

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

	else if (Dchoice == 2)
	{
		D = D_BlingPhong(roughness, NDotH);
	}

	else if (Dchoice == 3)
	{
		D = D_UEBlingPhong(roughness, n, 2*dot(n,v)*n-v);
	}

	else if (Dchoice == 4)
	{
		D = D_Beckmann(roughness, NDotH);
	}

	else if (Dchoice == 5)
	{
		D = D_Phong(roughness, NDotH);
	}

	else if (Dchoice == 6)
	{
		D = D_GGX_Anisotropic(n, h);
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

	else if (Fchoice == 2)
	{
		F = F_CookTorrance(specularColor, LDotH);
	}

	//ѡ��G
	if (Gchoice == 0)
	{
		G = G_Schlick(roughness, NDotL, NDotV);
	}
	else if (Gchoice == 1)
	{
		G= G_Neumann(NDotL, NDotV);
	}

	else if (Gchoice == 2)
	{
		G = G_CookTorrance(n, l, v, h);
	}

	else if (Gchoice == 3)
	{
		G = G_Kelemen(n, l, v, h);
	}

	else if (Gchoice == 4)
	{
		G = G_ClassicSchlick(roughness, NDotV);
	}

	else if (Gchoice == 5)
	{
		G = G_SchlickBeckmann(roughness, NDotV);
	}

	else if (Gchoice == 6)
	{
		G = G_SchlickGGX(roughness, NDotV);
	}

	else if (Gchoice == 7)
	{
		G = G_UESchlick(roughness, NDotL, NDotV);
	}

	else if (Gchoice == 8)
	{
		G = G_Beckmann(roughness, NDotV);
	}

	else if (Gchoice == 9)
	{
		G = G_GGX(roughness, NDotV);
	}

	else if (Gchoice == 10)
	{
		G = G_SmithJointGGX(roughness, NDotL, NDotV);
	}

	else if (Gchoice == 11)
	{
		G = G_SmithJointGGXImproved(roughness, NDotL, NDotV);
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
float Diffuse_DisneyPrincipledBRDF(float3 n, float3 l, float3 v, float3 h)
{
	float LDotH = dot(l, h);
	float NDotL = dot(n, l);
	float NDotV = dot(n, v);

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

	//��baseColor��Metallic��ֵ�õ�������ǿ�Ⱥ;��淴��ǿ��
	float3 diffuseIntensity = baseColor*(1.0f - metallic);
	float3 specularIntensity = lerp(specular,lerp(0.04f, baseColor, metallic), specularTint);

	//float3 specularIntensity = lerp(specular, baseColor, specularTint);
	float testLightColor = float3(1.0f, 1.0f, 1.0f);
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
			//specularColor += testLightColor*Specular_BRDF(roughness, n, l, v, h, specularIntensity);
			specularColor += light[index].lightColor*(Specular_BRDF(roughness, n, l, v, h, specularIntensity) + Specular_2_BRDF(LDotH, NDotH, NDotL, NDotV, specularIntensity) + diffuse_intensity*Diffuse_DisneyPrincipledBRDF(n, l, v,h));

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

float4 main(PS_IN input) : SV_TARGET
{
	float3 n = normalize(input.verNor);
	float3 v = normalize(viewPos.xyz - input.posWorld);

	float3 specularColor = float3(0.0f, 0.0f, 0.0f);

	//��baseColor��Metallic��ֵ�õ�������ǿ�Ⱥ;��淴��ǿ��
	float3 diffuseIntensity = baseColor*(1.0f - metallic);
	float3 specularIntensity = lerp(specular,lerp(0.04f, baseColor, metallic), specularTint);

	//float3 specularIntensity = lerp(specular, baseColor, specularTint);
	float testLightColor = float3(1.0f, 1.0f, 1.0f);
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
			//specularColor += testLightColor*Specular_BRDF(roughness, n, l, v, h, specularIntensity);
			specularColor += light[index].lightColor*(Specular_BRDF(roughness, n, l, v, h, specularIntensity) + Specular_2_BRDF(LDotH, NDotH, NDotL, NDotV, specularIntensity) + diffuse_intensity*Diffuse_DisneyPrincipledBRDF(n, l, v,h));

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


