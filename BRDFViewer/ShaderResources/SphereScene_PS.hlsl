
cbuffer CB_Light : register(b0)
{
	float4 baseColor;							//基色
	float3 lightPos;							//光源位置
	float unused0_CB_Light;
	float3 viewPos;								//视点（观察者）位置-----已转换到模型空间下
	float unused1_CB_Light;

}

//像素着色器输入结构
struct PS_IN
{
	float4 pos : SV_POSITION;
	float3 posModel:POSITION0;
	float3 verNor:NORMAL0;

};

//Phong反射模型
float4 PS_Phong(PS_IN input) : SV_TARGET
{

	float3 n = normalize(input.verNor);
	float3 l = normalize(lightPos - input.posModel);
	float3 v = normalize(viewPos - input.posModel);
	float3 r = normalize(2.0f * dot(n, l)*n - l);

	float NDotL = dot(n, l);
	float VDotR = dot(v, r);

	const float3 ka = float3(0.2f, 0.2f, 0.2f);
	const float3 kd = float3(0.8f, 0.8f, 0.8f);
	const float3 ks = float3(0.7f, 0.0f, 0.0f);

	const float3 ambient = float3(1.0f, 1.0f, 1.0f);
	const float3 diffuse = float3(1.0f, 1.0f, 1.0f);
	const float3 specular = float3(1.0f, 1.0f, 1.0f);

	const float3 ambientColor = ka*ambient;
	float3 diffuseColor = float3(0.0f, 0.0f, 0.0f);
	float3 specularColor = float3(0.0f, 0.0f, 0.0f);
	float shininess = 10.0f;


	if (NDotL > 0.0f)
	{
		diffuseColor = kd*NDotL*diffuse;

		specularColor = ks*pow(max(VDotR,0.0f), shininess)*specular;

	}

	float3 finalColor = ambientColor + diffuseColor + specularColor;

	float maxValue = max(max(finalColor.x, finalColor.y), finalColor.z);
	if (maxValue > 1.0f)
	{
		finalColor = finalColor / maxValue;
	}

	return float4(finalColor, 1.0f);
}

//Bling-Phong反射模型
float4 PS_BlingPhong(PS_IN input) : SV_TARGET
{

	float3 n = normalize(input.verNor);
	float3 l = normalize(lightPos - input.posModel);
	float3 v = normalize(viewPos - input.posModel);
	float3 h = normalize(l + v);

	float NDotL = dot(n, l);
	float NDotH = dot(n, h);

	const float3 ka = float3(0.2f, 0.2f, 0.2f);
	const float3 kd = float3(0.8f, 0.8f, 0.8f);
	const float3 ks = float3(0.7f, 0.0f, 0.0f);

	const float3 ambient = float3(1.0f, 1.0f, 1.0f);
	const float3 diffuse = float3(1.0f, 1.0f, 1.0f);
	const float3 specular = float3(1.0f, 1.0f, 1.0f);

	const float3 ambientColor = ka*ambient;
	float3 diffuseColor = float3(0.0f, 0.0f, 0.0f);
	float3 specularColor = float3(0.0f, 0.0f, 0.0f);
	float shininess = 10.0f;

	if (NDotL > 0.0f)
	{
		diffuseColor = kd*NDotL*diffuse;

		specularColor = ks*pow(max(NDotH,0.0f), shininess)*specular;

	}

	float3 finalColor = ambientColor + diffuseColor + specularColor;

	float maxValue = max(max(finalColor.x, finalColor.y), finalColor.z);
	if (maxValue > 1.0f)
	{
		finalColor = finalColor / maxValue;
	}

	return float4(finalColor, 1.0f);
}

//Schlick反射模型
float4 PS_Schlick(PS_IN input) : SV_TARGET
{

	float3 n = normalize(input.verNor);
	float3 l = normalize(lightPos - input.posModel);
	float3 v = normalize(viewPos - input.posModel);
	float3 r = normalize(2.0f * dot(n, l)*n - l);

	float NDotL = dot(n, l);
	float VDotR = dot(v, r);

	const float3 ka = float3(0.2f, 0.2f, 0.2f);
	const float3 kd = float3(0.8f, 0.8f, 0.8f);
	const float3 ks = float3(0.7f, 0.0f, 0.0f);

	const float3 ambient = float3(1.0f, 1.0f, 1.0f);
	const float3 diffuse = float3(1.0f, 1.0f, 1.0f);
	const float3 specular = float3(1.0f, 1.0f, 1.0f);
	const float shininess = 2.0f;
	const float3 ambientColor = ka*ambient;
	float3 diffuseColor = float3(0.0f, 0.0f, 0.0f);
	float3 specularColor = float3(0.0f, 0.0f, 0.0f);


	if (NDotL > 0.0f)
	{
		diffuseColor = kd*NDotL*diffuse;

		specularColor = ks* max((VDotR/(shininess-((shininess-1.0f)*VDotR))),0.0f)*specular;

	}

	float3 finalColor = ambientColor + diffuseColor + specularColor;

	float maxValue = max(max(finalColor.x, finalColor.y), finalColor.z);
	if (maxValue > 1.0f)
	{
		finalColor = finalColor / maxValue;
	}

	return float4(finalColor, 1.0f);
}

//各向同性Ward反射模型
float4 PS_IsotropicWard(PS_IN input) : SV_TARGET
{

	float3 n = normalize(input.verNor);
	float3 l = normalize(lightPos - input.posModel);
	float3 v = normalize(viewPos - input.posModel);
	float3 r = normalize(2.0f * dot(n, l)*n - l);
	float3 h = normalize(l + v);

	float NDotL = dot(n, l);
	float VDotR = dot(v, r);
	float AngleNDotH = acos(dot(n, h));

	const float3 ka = float3(0.2f, 0.2f, 0.2f);
	const float3 kd = float3(0.8f, 0.8f, 0.8f);
	const float3 ks = float3(0.7f, 0.0f, 0.0f);

	const float3 ambient = float3(1.0f, 1.0f, 1.0f);
	const float3 diffuse = float3(1.0f, 1.0f, 1.0f);
	const float3 specular = float3(1.0f, 1.0f, 1.0f);

	const float3 ambientColor = ka*ambient;
	float3 diffuseColor = float3(0.0f, 0.0f, 0.0f);
	float3 specularColor = float3(0.0f, 0.0f, 0.0f);

	const float PI = 3.1415926f;
	const float e = 2.71828182845904523536f;
	float sigma = 0.5f;

	if (NDotL > 0.0f)
	{
		diffuseColor = kd*NDotL*diffuse;

		specularColor = ks*max((pow(e, -(pow(tan(AngleNDotH), 2.0f) / (sigma*sigma))) / (2 * PI*sigma*sigma*sqrt(NDotL*dot(n, v)))), 0.0f)*NDotL*specular;

	}

	float3 finalColor = ambientColor + diffuseColor + specularColor;

	float maxValue = max(max(finalColor.x, finalColor.y), finalColor.z);
	if (maxValue > 1.0f)
	{
		finalColor = finalColor / maxValue;
	}

	return float4(finalColor, 1.0f);
}

//各向异性Ward反射模型
float4 PS_AnisotropicWard(PS_IN input) : SV_TARGET
{

	float3 n = normalize(input.verNor);
	float3 l = normalize(lightPos - input.posModel);
	float3 v = normalize(viewPos - input.posModel);
	float3 r = normalize(2.0f * dot(n, l)*n - l);
	float3 h = normalize(l + v);

	float NDotL = dot(n, l);
	float VDotR = dot(v, r);
	float NDotH = dot(n, h);
	float AngleNDotH = acos(dot(n, h));

	const float3 ka = float3(0.2f, 0.2f, 0.2f);
	const float3 kd = float3(0.8f, 0.8f, 0.8f);
	const float3 ks = float3(0.7f, 0.0f, 0.0f);

	const float3 ambient = float3(1.0f, 1.0f, 1.0f);
	const float3 diffuse = float3(1.0f, 1.0f, 1.0f);
	const float3 specular = float3(1.0f, 1.0f, 1.0f);

	const float3 ambientColor = ka*ambient;
	float3 diffuseColor = float3(0.0f, 0.0f, 0.0f);
	float3 specularColor = float3(0.0f, 0.0f, 0.0f);

	const float PI = 3.1415926f;
	const float e = 2.71828182845904523536f;
	float sigmax = 0.1f;
	float sigmay = 0.9f;

	//指定切线空间
	float3 UpVector = abs(n.z) < 0.999f ? float3(0, 0, 1) : float3(1, 0, 0);
	float3 TangentX = normalize(cross(UpVector, n));
	float3 TangentY = cross(n, TangentX);

	if (NDotL > 0.0f)
	{
		diffuseColor = kd*NDotL*diffuse;

		float beta = -2.0f*((pow(dot(h,TangentX)/sigmax,2.0f)+pow(dot(h, TangentY) / sigmay,2.0f))/(1.0f+ NDotH));
		specularColor = ks*max(pow(e,beta) / (2 * PI*sigmax*sigmay*sqrt(NDotL*dot(n,v))),0.0f)*NDotL*specular;

	}

	float3 finalColor = ambientColor + diffuseColor + specularColor;

	float maxValue = max(max(finalColor.x, finalColor.y), finalColor.z);
	if (maxValue > 1.0f)
	{
		finalColor = finalColor / maxValue;
	}

	return float4(finalColor, 1.0f);
}

float4 main(PS_IN input) : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}