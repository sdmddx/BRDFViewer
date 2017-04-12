cbuffer CB_Light : register(b0)
{
	float4 baseColor;							//基色
	float3 lightPos;							//光源位置
	float unused0_CB_Light;
	float3 viewPos;								//视点（观察者）位置-----已转换到模型空间下
	float unused1_CB_Light;

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
	float unused0_CB_PBR;
	float unused1_CB_PBR;
	float unused2_CB_PBR;
}
//像素着色器输入结构
struct PixelShaderInput_PBR
{
	float4 pos : SV_POSITION;
	float3 posModel:TEXCOORD0;
	float3 verNor:TEXCOORD1;
};


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
//GGX模型的重要性采样算法  返回值为法线坐标系的H向量
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

//G------------------------------------------------------------------------------------------
float G_Schlick(float roughness, float NDotL, float NDotV)
{
	float k = pow((roughness + 1.0f), 2.0f) / 8.0f;
	//float Gl = 1.0f / (1.0f - k + k / NDotL);
	//float Gv = 1.0f / (1.0f - k + k / NDotV);
	return 1.0f / (1.0f - k + k / NDotL) / (1.0f - k + k / NDotV);
}


//在IBL中使用BRDF着色模型的forward rendering的像素着色器--------------------------------------------------------------- ？？？？？？
Texture2D EnvMap:register(t0);

Texture2D cubeMapRight:register(t1);
Texture2D cubeMapLeft:register(t2);
Texture2D cubeMapTop:register(t3);
Texture2D cubeMapBottom:register(t4);
Texture2D cubeMapBack:register(t5);
Texture2D cubeMapFront:register(t6);


SamplerState samLinear:register(s0);

//----------------------------------------------------------------------------------------------
//获得环境贴图要采样的贴图的序号  规定+x -x +y -y +z -z依次为0-5   
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

//这里输入的l是单位向量		获得采样立方体环境贴图后的采样值
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
//lambert漫反射模型-----------------------------------------------------------------------------
//float Lambertian(float3 upl, float3 v)
float Lambertian()
{
	return 0.318f;
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



//像素着色器----------------------------------------------------------------------------------
float4 PS(PixelShaderInput_PBR input) : SV_TARGET
{
	float3 n = normalize(input.verNor);

	float3 v = normalize(viewPos - input.posModel);		//？？？？？？？？？？？？？？？

	float3 specularColor = float3(0.7f, 0.7f, 0.7f);

	return float4(SpecularIBL_DGGX(specularColor, roughness, n, v),1.0f);

}


float4 main(PixelShaderInput_PBR input) : SV_TARGET
{

	return float4(GetCubeMapSample(input.verNor),1.0f);
}
