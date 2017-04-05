
cbuffer CB_Light : register(b0)
{
	float4 baseColor;							//��ɫ
	float3 lightPos;							//��Դλ��
	float unused0;
	float3 viewPos;								//�ӵ㣨�۲��ߣ�λ��
	float unused1;

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
	float unused2;
	float unused3;
	float unused4;
}

//D--------------------------------------------------------------------------------------------
float D_GGX(float roughness, float NDotH)
{
	return 0.318f* roughness*roughness / pow(NDotH*NDotH*(roughness*roughness - 1.0f) + 1.0f, 2.0f);
}

//static float D_GTR_C = 0.25f;
//static float D_GTR_N = 1.0f;

float D_GTR(float roughness, float NDotH, float D_GTR_C, float D_GTR_N)
{
	float cosHSqr = NDotH*NDotH;
	return D_GTR_C / pow((roughness*roughness*cosHSqr + 1.0f - cosHSqr), D_GTR_N);
}

//F-----------------------------------------------------------------------------------------
float3 F_Schlick(float specularColor, float LDotH)
{
	float Fc = pow(1.0f - LDotH, 5.0f);
	return (1.0f - Fc)*specularColor + Fc;
}

float3 F_Schlick_SG(float specularColor, float LDotH)
{
	float Fc = pow(2.0f, (-5.55473f*LDotH - 6.98316f)*LDotH);
	return  (1.0f - Fc)*specularColor + Fc;
}

//G------------------------------------------------------------------------------------------
float G_Schlick(float roughness, float NDotL, float NDotV)
{
	float k = pow((roughness + 1.0f), 2.0f) / 8.0f;
	//float Gl = 1.0f / (1.0f + k + k / NDotL);
	//float Gv = 1.0f / (1.0f + k + k / NDotV);
	return 1.0f / (1.0f + k + k / NDotL) / (1.0f + k + k / NDotV);
}

//BRDF��specular����---------------------------------------------------------------------------
float3 Specular_BRDF(float roughness, float LDotH, float NDotH, float NDotL, float NDotV, float specularColor)
{

	return D_GGX(roughness, NDotH)*F_Schlick_SG(specularColor, LDotH)*G_Schlick(roughness, NDotL, NDotV) / (4.0f*NDotL*NDotV) + specular;
}

float3 Specular_2_BRDF(float LDotH, float NDotH, float NDotL, float NDotV, float specularColor)
{
	return D_GGX(clearCoatGloss, NDotH)*G_Schlick(0.25f, NDotL, NDotV) / (4.0f*NDotL*NDotV) + clearCoat;		//F�����
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


//����PBR���۵�forward rendering��������ɫ��----------------------------------------------------

struct PixelShaderInput_PBR
{
	float4 pos : SV_POSITION;
	float3 posModel:TEXCOORD0;
	float3 verNor:TEXCOORD1;
};

float4 main__(PixelShaderInput_PBR input) : SV_TARGET
{
	float3 n = normalize(input.verNor);
	float3 l = normalize(lightPos - input.posModel);
	float3 v = normalize(viewPos - input.posModel);
	float3 h = normalize(l + v);

	//������Ϣ
	float LDotH = dot(l, h);
	float NDotH = dot(n, h);
	float NDotL = dot(n, l);
	float NDotV = dot(n, v);

	if (NDotL < 0.0f) return float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 specularColor = float3(0.2f, 0.2f, 0.2f);

	//return float4(f.x,f.y,f.z, 1.0f);
	//return float4(F_Schlick(specularColor, LDotH), 1.0f);
	//return float4(Specular_BRDF(roughness, LDotH, NDotH, NDotL, NDotV, specularColor), 1.0f);
	return baseColor*specularTint*float4(Specular_BRDF(roughness,LDotH, NDotH, NDotL, NDotV,specularColor),1.0);
}

//Hammersley����------------------------------------------------------------------------------
float RadicalInverse_VDC(uint bits) {
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits)*2.3283064365386963e-10;

}

float2 Hammersley(uint i, uint N)
{
	return float2(float(i) / float(N), RadicalInverse_VDC(i));
}

//IS-----------------------------------------------------------------------------------------
//����Ϊ2ά���� ����ֲڶ� ����  ��Ҫ�Բ���������PDF��أ�Disney Principled BRDF��ʹ��pdf=D*NDotH/(4*VDotH�������Ҫ�����㷨��D��ѡȡ����
//GGXģ�͵���Ҫ�Բ����㷨  ����ֵΪ��������ϵ�µ�lv������
float3 ImportanceSampleGGX(float2 Xi, float roughness, float3 N)
{
	float a = roughness*roughness;

	float Phi = 6.283f*Xi.x;
	float CosTheta = sqrt((1 - Xi.y) / (1.0f + (a*a - 1)*Xi.y));
	float SinTheta = sqrt(1 - CosTheta*CosTheta);

	//��������ǻ��H����
	float3 H;
	H.x = SinTheta*cos(Phi);
	H.y = SinTheta*sin(Phi);
	H.z = CosTheta;

	//��÷��߿ռ�����ϵ
	//float3 UpVector = abs(N.z) < 0.999f ? float3(0, 0, 1) : float3(1, 0, 0);
	//float3 TangentX = normalize(cross(UpVector, N));
	//float3 TangentY = cross(N, TangentX);


	return H;

}

//GTRģ�͵���Ҫ�Բ����㷨					????
float3 ImportanceSampleGTR(float2 Xi, float roughness, float3 N)
{

}

//��IBL��ʹ��ԭʼBRDF��ɫģ�ͻ�õľ�����ա���(���(L*f/pdf))/N------------------------------------------------------
//��õ�һ�㴦��BRDF specular ��Ӧ��DȡGGXģ��ʱ����Ҫ�Բ����㷨
float3 SpecularIBL_DGGX(float3 specularColor, float roughness, float3 n, float3 v)
{
	float3 specularLighting = 0;
	const uint NumSamples = 1024;			//��������

											//��÷��߿ռ�����ϵ
	float3 UpVector = abs(n.z) < 0.999f ? float3(0, 0, 1) : float3(1, 0, 0);
	float3 TangentX = normalize(cross(UpVector, n));
	float3 TangentY = cross(n, TangentX);

	for (uint i = 0; i < NumSamples; ++i)
	{
		//ÿ�β����������һ�����䷽��Ͱ�����
		float2 Xi = Hammersley(i, NumSamples);
		float3 h_IS = ImportanceSampleGGX(Xi, roughness, n);
		//��h�任������N���ڵ�����ϵ
		float3 h = TangentX*h_IS.x + TangentY*h_IS.y + n*h_IS.z;
		float3 l = 2 * dot(v, h)*h - v;

		//������Ϣ
		float LDotH = dot(l, h);
		float NDotH = dot(n, h);
		float NDotL = dot(n, l);
		float NDotV = dot(n, v);

		if (NDotL > 0.0f)
		{
			//�����ÿ�β���������й��ռ���
			//�����=SampleColor*NDotL
			//BRDF Specular = D*F*G/(4*NDotL*NDotV)
			//pdf=D*NDotH/(4*VDotH)			!!!!

			//����ɫ���е������廷����ͼ����������������ϵ�²����ƶ�����ΪIS�������Ĺ�������λ������N��V����������ռ䣬
			//��ģ�Ϳռ䣬�Ը����������õ�����ֵ�����Ը�������λ��ģ�Ϳռ䡣
			float3 sampleColor = GetCubeMapSample(l);											///????????????
			float G = G_Schlick(roughness, NDotL, NDotV);
			float Fc = pow(1.0f - LDotH, 5.0f);
			float3 F = (1.0f - Fc)*specularColor + Fc;
			//specularLighting += sampleColor*NDotL*(F*G*LDotH / (NDotL*NDotH*NDotV) + Lambertian());
			specularLighting += sampleColor*NDotL*(F*G*LDotH / (NDotL*NDotH*NDotV)
				+ Diffuse_DisneyPrincipledBRDF(LDotH, NDotL, NDotV));
		}
	}
	return specularLighting / NumSamples;
}

//��IBL��ʹ��BRDF��ɫģ�͵�forward rendering��������ɫ��--------------------------------------------------------------- ������������
Texture2D EnvMap:register(t0);

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;

};


float4 main(PixelShaderInput_PBR input) : SV_TARGET
{
	float3 n = normalize(input.verNor);
	float3 v = normalize(viewPos - input.posModel);

	float3 specularColor = float3(0.7f, 0.7f, 0.7f);

	return float4(SpecularIBL_DGGX(specularColor, roughness, n, v),1.0f);

}


//��ԭʼ������ɫ��������� ��һ����Ϊ(���(L)/N)��Ҳ�����������ȣ�ֻ�ͻ�������ͼ���--------------------------------
//UE4�е����������˲��������⣬���ѻ���GGX��pdf�ŵ���Ԥ�����pdf��n,v�й� ������ǵ�һ���ֵ���͡�����PDF��Ҫvn,�������v=n=r
float3 PrefilterEnvMap(float roughness, float3 R)
{
	float3 N = R;
	float3 V = R;
	float3 PrefilteredColor = 0;
	float TotalWeight = 0.0f;
	const uint NumSamples = 256;

	//��÷��߿ռ�����ϵ
	float3 UpVector = abs(N.z) < 0.999f ? float3(0, 0, 1) : float3(1, 0, 0);
	float3 TangentX = normalize(cross(UpVector, N));
	float3 TangentY = cross(N, TangentX);

	for (uint i = 0; i < NumSamples; i++)
	{
		float2 Xi = Hammersley(i, NumSamples);
		float3 h_IS = ImportanceSampleGGX(Xi, roughness, N);
		//��h�任������N���ڵ�����ϵ
		float3 H = TangentX*h_IS.x + TangentY*h_IS.y + N*h_IS.z;
		float3 L = 2 * dot(V, H) * H - V;
		float NDotL = saturate(dot(N, L));
		if (NDotL > 0)
		{
			PrefilteredColor += GetCubeMapSample(L) * NDotL;
			TotalWeight += NDotL;
		}
	}
	return PrefilteredColor / TotalWeight;
}

//�ڶ������ǶԲ�ͬl��f��һ������ �μ�����----------------------------------------------------------------------------------------
//д������ͼ
float2 IntegrateBRDF(float roughness, float NDotV, float N)
{

	//��÷�������ϵ�µ�V
	float3 V;
	V.x = sqrt(1.0f - NDotV * NDotV); // sin
	V.y = 0;
	V.z = NDotV; // cos
	float A = 0;
	float B = 0;
	const uint NumSamples = 1024;
	for (uint i = 0; i < NumSamples; i++)
	{
		float2 Xi = Hammersley(i, NumSamples);
		float3 H = ImportanceSampleGGX(Xi, roughness, N);			//H����������ռ��µģ���V���ڷ�������ռ��µģ���������������������
		float3 L = 2 * dot(V, H) * H - V;
		float NDotL = saturate(L.z);
		float NDotH = saturate(H.z);
		float VDotH = saturate(dot(V, H));
		if (NDotL > 0)
		{
			float G = G_Schlick(roughness, NDotV, NDotL);
			float G_Vis = G * VDotH / (NDotH * NDotV);					//����֮ǰ����Ҫ�Բ���ʹ��DȡGGXģ��
			float Fc = pow(1 - VDotH, 5);
			A += (1 - Fc) * G_Vis;
			B += Fc * G_Vis;
		}
	}
	return float2(A, B) / NumSamples;
}

//֮���ǰ���˲���Ļ���ͼ�͵ڶ����ֻ��2��������ˣ���ʵ��ֻ��Ҫ����2������Ϳ�����----------------------------------
float3 ApproximateSpecularIBL(float3 specularColor, float roughness, float3 N, float3 V)
{
	float NDotV = saturate(dot(N, V));
	float3 R = 2 * dot(V, N) * N - V;					//R��ȡֵ������������������������
	float3 PrefilteredColor = PrefilterEnvMap(roughness, R);
	float2 EnvBRDF = IntegrateBRDF(roughness, NDotV, N);
	return PrefilteredColor * (specularColor * EnvBRDF.x + EnvBRDF.y);
}






