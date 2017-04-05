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
		//����ӳ�亯���ĺ���ָ��
		typedef void (SphereGenerator::*pTexFunc)(BufMesh* pMesh);
		typedef pTexFunc* pptexFunc;
		pptexFunc ppTexFunc;

	private:
		BasicSphere**  pInit;
	private:
		//��ֵһ��Face
		void SetFace(SDMuint3& pface, SDMuint x, SDMuint y, SDMuint z);

		//��ʼ��һ����ʱ����������
		BasicSphere* InitNeg();

		//��ʼ��һ��˳ʱ����������
		BasicSphere* InitPos();

		//�����ɵ��������һ�ε���ϸ��
		BasicSphere* subdivideSphere(BasicSphere* pinit);

		BasicSphere*  GenerateAtomSphere(TriangleOrder order);
	public:

		SphereGenerator();

		~SphereGenerator();

		//ƽ��ӳ��ģʽ������������
		void CreateTexFlat(BufMesh* pMesh);

		//����һ��PvN��
		unique_ptr<DataBuf> GenerateSpherePvN(SDMuint sumRecursive, TriangleOrder order = Positive);

		//��һ��������淨��
		void AddFaceNormal(BufMesh* pSphere);

		//����һ�������ⲿ�۲�ʱ��˳ʱ�뷽���������
		unique_ptr<DataBuf> GenerateSpherePTvN(SDMuint count, TriangleOrder order = Positive, TexMapperFunc func = Flat);



	};

}




#endif



