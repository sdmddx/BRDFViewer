
//G------------------------------------------------------------------------------------------
float G_Schlick(float roughness, float NDotL, float NDotV)
{
	float k = pow((roughness + 1.0f), 2.0f) / 8.0f;
	//float Gl = 1.0f / (1.0f + k + k / NDotL);
	//float Gv = 1.0f / (1.0f + k + k / NDotV);
	return 1.0f / (1.0f + k + k / NDotL) / (1.0f + k + k / NDotV);
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

float2 IntegrateBRDF(float roughness, float NDotV)
{

	//�ڷ�������ϵ��
	float3 N = float3(0.0f, 0.0f, 1.0f);

	//��÷�������ϵ�µ�V����Vλ��y=0��ƽ����
	float3 V;
	V.x = sqrt(1.0f - NDotV * NDotV); // sin
	V.y = 0.0f;
	V.z = NDotV; // cos
	float A = 0.0f;
	float B = 0.0f;
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


//const float sum_threadGroups = float(group_dimx)*float(group_dimy)*float(group_dimz);

//������� ����roughness NDotV��ȡ
RWTexture2D<float2> InteBRDF:register(u0);

//�߳������� ȷ��2�����
#define dimx 16
#define dimy 16
#define dimz 1
[numthreads(dimx, dimy, dimz)]
void CS(uint3 GridIndex:SV_GroupThreadID,
	uint3 RoomIndex : SV_GroupID,
	uint3 DTid : SV_DispatchThreadID,
	uint Order : SV_GroupIndex)
{

	const uint group_dimx = 16;
	const uint group_dimy = 16;
	const uint group_dimz = 1;
	const uint sum_threadGroups = float(group_dimx)*float(group_dimy)*float(group_dimz) - 1.0f;
	const uint threadsPerThreadGroup = dimx*dimy*dimz - 1.0f;
	//����roughness ,NDotV 
	//����ͬһ���߳��� roughness����ͬ�� roughness�Ļ������Ⱥ��߳��������й�
	//NdotV��ֵȡ����Order������ǰ�߳������߳����ڵ������������,����NDotV�Ļ�������ȡ����ÿ�߳�����߳���
	uint indexThreadGroup = RoomIndex.z * group_dimx * group_dimy + RoomIndex.y * group_dimx + RoomIndex.x;
	float roughness = float(indexThreadGroup) / sum_threadGroups;
	float NDotV = float(Order) / threadsPerThreadGroup;

	//����roughnessΪ��������x����NDotVΪy���򣬲���ʱע������
	uint2 index = uint2(indexThreadGroup, Order);
	InteBRDF[index] = IntegrateBRDF(roughness, NDotV);


}



[numthreads(dimx, dimy, dimz)]
void main(uint3 GridIndex:SV_GroupThreadID,
	uint3 RoomIndex : SV_GroupID,
	uint3 DTid : SV_DispatchThreadID,
	uint Order : SV_GroupIndex)
{

	const uint group_dimx = 16;
	const uint group_dimy = 16;
	const uint group_dimz = 1;
	const uint sum_threadGroups = float(group_dimx)*float(group_dimy)*float(group_dimz) - 1.0f;
	const uint threadsPerThreadGroup = dimx*dimy*dimz - 1.0f;
	//����roughness ,NDotV 
	//����ͬһ���߳��� roughness����ͬ�� roughness�Ļ������Ⱥ��߳��������й�
	//NdotV��ֵȡ����Order������ǰ�߳������߳����ڵ������������,����NDotV�Ļ�������ȡ����ÿ�߳�����߳���
	uint indexThreadGroup = RoomIndex.z * group_dimx * group_dimy + RoomIndex.y * group_dimx + RoomIndex.x;
	float roughness = float(indexThreadGroup) / sum_threadGroups;
	float NDotV = float(Order) / threadsPerThreadGroup;

	//����roughnessΪ��������x����NDotVΪy���򣬲���ʱע������
	uint2 index = uint2(indexThreadGroup, Order);
	InteBRDF[index] = IntegrateBRDF(roughness, NDotV);


}