//传统延迟着色 Deferred Shading  绘制两次
//使用Blinn-Phong光照模型

Texture2D tex_Color:register(t0);		//模型贴图
Texture2D tex_w_verNor:register(t1);	//法线纹理


SamplerState Sampler:register(s0);     

//G-Buffer过程-------------------------------------------------------------------------------------------------
//PS in
struct VS_OUT_G
{
	float4 pos:SV_POSITION;
	float3 w_verNor:NORMAL0;	//位于世界空间下
	//float2 tex:TEXCOORD0;

};

//PS out
struct PS_OUT_G
{
	float4 w_verNor:SV_TARGET0;		//这里只存储法线信息
};

//G过程PS
PS_OUT_G PS_G(VS_OUT_G input) :SV_TARGET
{
	PS_OUT_G output;
	output.w_verNor = (float4(normalize(input.w_verNor),0.0f));
	return output;
}

//着色过程----------------------------------------------------------------------------------------------------
//在窗口大小或投影变换改变时更新
cbuffer WindowInfo:register(b0)
{
	float height;
	float width;
	float fovAngleY;	//视口垂直可视角度
	float aspectRatio;	//视口宽高比
	float nearZ;		//近平面距离
	float farZ;			//远平面距离
	float unused0;
	float unused1;
}

cbuffer WVP_Inverse:register(b1)
{
	matrix view_Inverse;
	matrix model_Inverse;
	float4 viewPos;					//观察者位置
	//float unused0_WVP_Inverse;
}

cbuffer LightsInfo:register(b2)
{
	uint numLights;					//光源数量
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

//漫反射模型
float Lambert(float3 l, float3 n)
{
	return dot(l, n);
}

//Phong模型
static const float ex_Phong = 2.0f;
float Phong(float3 l, float3 n, float3 v)
{
	return pow(max((2 * dot(l, n)*n - l)*v,0.0000001), ex_Phong);

}

//Bling-Phong模型
const uniform float ex_BP = 2.0f;
float Bling_Phong(float3 l, float3 n, float3 v)
{
	return pow(max(normalize(l+v)*n, 0.0000001f), ex_BP);
}

//着色过程PS
float4 PS_S(VS_OUT_S input):SV_TARGET
{

	float2 tex = float2(input.pos.x/width,input.pos.y/height);
	//获得投影空间中的xy坐标
	float2 posInProj = float2(tex.x*2.0f - 1.0f, -tex.y*2.0f + 1.0f);
	
	//获得观察空间中的顶点坐标
	float z = input.pos.w;	//顶点的绝对深度
	float zMulTanFovAngleY = z*tan(fovAngleY);
	float4 pos = float4(posInProj.x*aspectRatio*zMulTanFovAngleY, posInProj.y*zMulTanFovAngleY, z, 1.0f);

	//获得世界空间中的顶点坐标
	pos = mul(pos, view_Inverse);
	//采样法线
	float3 w_verNor = normalize(tex_w_verNor.Sample(Sampler,tex).xyz);

	//进行光照计算 
	
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
		//漫反射
		if (NDotL > 0.0f)
		{
			colorDiffuse += NDotL*light[index].lightColor;
			numDiffuse++;
		}

		//BP 镜面
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
