#ifndef SPHERE_H
#define SPHERE_H



using namespace AssetMagr;
using namespace DirectX;

namespace BasicGraphic
{
	struct BasicSphere
	{
		XMVECTOR* pVer;
		SDMuint3* pFace;
		SDMuint sumVer;
		SDMuint sumFace;
	};

	enum TexMapperFunc
	{
		Flat,
	};

	enum TriangleOrder
	{
		Negative,
		Positive,
	};

	class COMMON_API  SphereGenerator
	{
		//纹理映射函数的函数指针
		typedef void (SphereGenerator::*pTexFunc)(BufMesh* pMesh);
		typedef pTexFunc* pptexFunc;
		pptexFunc ppTexFunc;

	private:
		BasicSphere**  pInit;
	private:
		//赋值一个Face
		void SetFace(SDMuint3& pface, SDMuint x, SDMuint y, SDMuint z);

		//初始化一个逆时针正八面体
		BasicSphere* InitNeg();

		//初始化一个顺时针正八面体
		BasicSphere* InitPos();

		//对生成的球体进行一次迭代细分
		BasicSphere* subdivideSphere(BasicSphere* pinit);

		BasicSphere*  GenerateAtomSphere(TriangleOrder order);
	public:

		SphereGenerator();

		~SphereGenerator();

		//平面映射模式生成纹理坐标
		void CreateTexFlat(BufMesh* pMesh);

		//生成一个PvN球
		unique_ptr<DataBuf> GenerateSpherePvN(SDMuint sumRecursive, TriangleOrder order = Positive);

		//给一个球添加面法线
		void AddFaceNormal(BufMesh* pSphere);

		//生成一个（从外部观察时）顺时针方向的纹理球
		unique_ptr<DataBuf> GenerateSpherePTvN(SDMuint count, TriangleOrder order = Positive, TexMapperFunc func = Flat);



	};

}




#endif



