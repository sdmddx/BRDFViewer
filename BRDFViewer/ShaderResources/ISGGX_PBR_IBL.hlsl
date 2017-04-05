
cbuffer CB_Light : register(b0)
{
	float4 baseColor;							//基色
	float3 lightPos;							//光源位置
	float unused0;
	float3 viewPos;								//视点（观察者）位置
	float unused1;

}

cbuffer CB_PBR : register(b1)
{
	float roughness;							//PBR粗糙度
	float metallic;								//PBR金属度
	float specular;								//第一层镜面反射的强度  取值0.0f到0.08f
	float specularTint;							//specular是否有颜色  1.0f时为basecolor 0.0f时为无色（定义颜色）
	float clearCoat;							//第二层镜面反射的强度  取值0.0f到0.25f
	float clearCoatGloss;						//第二层specularD项的roughness
	float D_GTR_C;								//第一层specularD项GTR模型的调整量c
	float D_GTR_N;								//第一层specularD项GTR模型的n    通常取值在1.0f到2.0f
	float anisotropic;							//各向异性
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

//BRDF的specular分量---------------------------------------------------------------------------
float3 Specular_BRDF(float roughness, float LDotH, float NDotH, float NDotL, float NDotV, float specularColor)
{

	return D_GGX(roughness, NDotH)*F_Schlick_SG(specularColor, LDotH)*G_Schlick(roughness, NDotL, NDotV) / (4.0f*NDotL*NDotV) + specular;
}

float3 Specular_2_BRDF(float LDotH, float NDotH, float NDotL, float NDotV, float specularColor)
{
	return D_GGX(clearCoatGloss, NDotH)*G_Schlick(0.25f, NDotL, NDotV) / (4.0f*NDotL*NDotV) + clearCoat;		//F项？？？
}

//BRDF的diffuse分量----------------------------------------------------------------------------
//用2个Fresnel项来增加入射角度低时反射的光线能量强度
float Diffuse_DisneyPrincipledBRDF(float LDotH, float NDotL, float NDotV)
{
	float oneMinusCosL = 1.0f - NDotL;
	float oneMinusCosV = 1.0f - NDotV;
	float oneMinusCosLSqr = oneMinusCosL*oneMinusCosL;
	float oneMinusCosVSqr = oneMinusCosV*oneMinusCosV;
	float F_D90 = 0.5f + 2.0f*LDotH*LDotH*roughness;
	return 0.318f*(1.0f + (F_D90 - 1.0f)*oneMinusCosLSqr*oneMinusCosLSqr*oneMinusCosL)*(1.0f + (F_D90 - 1.0f)*oneMinusCosVSqr*oneMinusCosVSqr*oneMinusCosV);

}

//lambert漫反射模型-----------------------------------------------------------------------------
//float Lambertian(float3 l, float3 v)
float Lambertian()
{
	return 0.318f;
}


//基于PBR理论的forward rendering的像素着色器----------------------------------------------------

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

	//辅助信息
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

//Hammersley采样------------------------------------------------------------------------------
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
//输入为2维向量 顶点粗糙度 法线  重要性采样由于与PDF相关，Disney Principled BRDF中使用pdf=D*NDotH/(4*VDotH）因此重要采样算法因D的选取而定
//GGX模型的重要性采样算法  返回值为世界坐标系下的lv半向量
float3 ImportanceSampleGGX(float2 Xi, float roughness, float3 N)
{
	float a = roughness*roughness;

	float Phi = 6.283f*Xi.x;
	float CosTheta = sqrt((1 - Xi.y) / (1.0f + (a*a - 1)*Xi.y));
	float SinTheta = sqrt(1 - CosTheta*CosTheta);

	//根据球面角获得H向量
	float3 H;
	H.x = SinTheta*cos(Phi);
	H.y = SinTheta*sin(Phi);
	H.z = CosTheta;

	//获得法线空间坐标系
	//float3 UpVector = abs(N.z) < 0.999f ? float3(0, 0, 1) : float3(1, 0, 0);
	//float3 TangentX = normalize(cross(UpVector, N));
	//float3 TangentY = cross(N, TangentX);


	return H;

}

//GTR模型的重要性采样算法					????
float3 ImportanceSampleGTR(float2 Xi, float roughness, float3 N)
{

}

//在IBL中使用原始BRDF着色模型获得的镜面光照。即(求和(L*f/pdf))/N------------------------------------------------------
//获得单一点处的BRDF specular 对应于D取GGX模型时的重要性采样算法
float3 SpecularIBL_DGGX(float3 specularColor, float roughness, float3 n, float3 v)
{
	float3 specularLighting = 0;
	const uint NumSamples = 1024;			//采样次数

											//获得法线空间坐标系
	float3 UpVector = abs(n.z) < 0.999f ? float3(0, 0, 1) : float3(1, 0, 0);
	float3 TangentX = normalize(cross(UpVector, n));
	float3 TangentY = cross(n, TangentX);

	for (uint i = 0; i < NumSamples; ++i)
	{
		//每次采样都将获得一个入射方向和半向量
		float2 Xi = Hammersley(i, NumSamples);
		float3 h_IS = ImportanceSampleGGX(Xi, roughness, n);
		//吧h变换到输入N所在的坐标系
		float3 h = TangentX*h_IS.x + TangentY*h_IS.y + n*h_IS.z;
		float3 l = 2 * dot(v, h)*h - v;

		//辅助信息
		float LDotH = dot(l, h);
		float NDotH = dot(n, h);
		float NDotL = dot(n, l);
		float NDotV = dot(n, v);

		if (NDotL > 0.0f)
		{
			//这里对每次采样结果进行光照计算
			//入射光=SampleColor*NDotL
			//BRDF Specular = D*F*G/(4*NDotL*NDotV)
			//pdf=D*NDotH/(4*VDotH)			!!!!

			//该着色器中的立方体环境贴图的物体在世界坐标系下不能移动，因为IS采样到的光照向量位于输入N（V）所在坐标空间，
			//即模型空间，对该向量采样得到纹理值，所以该立方体位于模型空间。
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

//在IBL中使用BRDF着色模型的forward rendering的像素着色器--------------------------------------------------------------- ？？？？？？
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


//吧原始采样着色方程做拆分 第一部分为(求和(L)/N)，也就是入射辐射度，只和环境光贴图相关--------------------------------
//UE4中的做法，除了采样环境光，还把基于GGX的pdf放到了预处理里。pdf与n,v有关 输出就是第一部分的求和。由于PDF需要vn,这里假设v=n=r
float3 PrefilterEnvMap(float roughness, float3 R)
{
	float3 N = R;
	float3 V = R;
	float3 PrefilteredColor = 0;
	float TotalWeight = 0.0f;
	const uint NumSamples = 256;

	//获得法线空间坐标系
	float3 UpVector = abs(N.z) < 0.999f ? float3(0, 0, 1) : float3(1, 0, 0);
	float3 TangentX = normalize(cross(UpVector, N));
	float3 TangentY = cross(N, TangentX);

	for (uint i = 0; i < NumSamples; i++)
	{
		float2 Xi = Hammersley(i, NumSamples);
		float3 h_IS = ImportanceSampleGGX(Xi, roughness, N);
		//吧h变换到输入N所在的坐标系
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

//第二部分是对不同l的f的一个积分 参见论文----------------------------------------------------------------------------------------
//写入纹理图
float2 IntegrateBRDF(float roughness, float NDotV, float N)
{

	//获得法线坐标系下的V
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
		float3 H = ImportanceSampleGGX(Xi, roughness, N);			//H这里是世界空间下的，而V是在法线坐标空间下的？？？？？？？？？？？
		float3 L = 2 * dot(V, H) * H - V;
		float NDotL = saturate(L.z);
		float NDotH = saturate(H.z);
		float VDotH = saturate(dot(V, H));
		if (NDotL > 0)
		{
			float G = G_Schlick(roughness, NDotV, NDotL);
			float G_Vis = G * VDotH / (NDotH * NDotV);					//由于之前的重要性采样使得D取GGX模型
			float Fc = pow(1 - VDotH, 5);
			A += (1 - Fc) * G_Vis;
			B += Fc * G_Vis;
		}
	}
	return float2(A, B) / NumSamples;
}

//之后把前置滤波后的环境图和第二部分获得2个分量相乘，事实上只需要采样2个纹理就可以了----------------------------------
float3 ApproximateSpecularIBL(float3 specularColor, float roughness, float3 N, float3 V)
{
	float NDotV = saturate(dot(N, V));
	float3 R = 2 * dot(V, N) * N - V;					//R的取值？？？？？？？？？？？？
	float3 PrefilteredColor = PrefilterEnvMap(roughness, R);
	float2 EnvBRDF = IntegrateBRDF(roughness, NDotV, N);
	return PrefilteredColor * (specularColor * EnvBRDF.x + EnvBRDF.y);
}






