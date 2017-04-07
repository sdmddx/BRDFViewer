
cbuffer CB_Light : register(b0)
{
	float4 baseColor;							//��ɫ
	float3 lightPos;							//��Դλ��
	float unused0;
	float3 viewPos;								//�ӵ㣨�۲��ߣ�λ��-----��ת����ģ�Ϳռ���
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

//������ɫ������ṹ
struct PixelShaderInput_PBR
{
	float4 pos : SV_POSITION;		//��PS��ֵ���ɶ�Ӧλ�õ�����ֵ���Ͻ�Ϊ��0��0��
	float3 posModel:TEXCOORD0;
	float3 verNor:TEXCOORD1;
};

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
//GGXģ�͵���Ҫ�Բ����㷨  ����ֵΪ��������ϵ��H����
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


//G------------------------------------------------------------------------------------------
float G_Schlick(float roughness, float NDotL, float NDotV)
{
	float k = pow((roughness + 1.0f), 2.0f) / 8.0f;
	//float Gl = 1.0f / (1.0f + k + k / NDotL);
	//float Gv = 1.0f / (1.0f + k + k / NDotV);
	return 1.0f / (1.0f + k + k / NDotL) / (1.0f + k + k / NDotV);
}


//��IBL��ʹ��BRDF��ɫģ�͵�forward rendering��������ɫ��--------------------------------------------------------------- ������������
Texture2D EnvMap:register(t0);

Texture2D cubeMapRight:register(t1);
Texture2D cubeMapLeft:register(t2);
Texture2D cubeMapTop:register(t3);
Texture2D cubeMapBottom:register(t4);
Texture2D cubeMapBack:register(t5);
Texture2D cubeMapFront:register(t6);


SamplerState samLinear:register(s0);

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
//Pre-Filtered EnvMap+Integrate Env BRDF----------------------------------------------------------------------

//��ԭʼ������ɫ��������� ��һ����Ϊ(���(L)/N)��Ҳ�����������ȣ�ֻ�ͻ�����ͼ���
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

//�ڶ������ǶԲ�ͬl��f��һ������ ,�������----------------------------------------------------------------------------------------
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
	const uint NumSamples = 256;
	for (uint i = 0; i < NumSamples; i++)
	{
		float2 Xi = Hammersley(i, NumSamples);
		float3 H = ImportanceSampleGGX(Xi, roughness, N);
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
	return PrefilteredColor * ((specularColor * EnvBRDF.x + EnvBRDF.y)+0.6f);

}

//������ɫ��----------------------------------------------------------------------------------
float4 PS(PixelShaderInput_PBR input) : SV_TARGET
{
	float3 n = normalize(input.verNor);
	float3 v = normalize(viewPos - input.posModel);

	float3 specularColor = float3(0.7f, 0.7f, 0.7f);

	return float4(ApproximateSpecularIBL(specularColor, roughness, n, v),1.0f);

}

float4 main(PixelShaderInput_PBR input) : SV_TARGET
{
	return float4(GetCubeMapSample(input.verNor),1.0f);
}