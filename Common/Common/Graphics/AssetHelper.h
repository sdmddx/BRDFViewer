#pragma once

#include <DirectXMath.h>

using namespace DirectX;
//Mesh�ļ��е�
#define MESH_SELFINFOR_ADDRESS 100l		//mesh������Ϣ���ڴ���ַ
#define SUMMESH_ADDRESS 110l	//mesh������Ϣ���ڴ���ַ
#define MESHINDEXINFOR_ADDRESS 200l		//Mesh������Ϣ���ڴ���ַ
#define CAPACITYMESH 1000l		//���Mesh����
#define MAXMESHSIZE_ADDRESS 100l	//����Mesh������Ϣ��ռ�ֽڴ�С
#define MESH (MESHINDEXINFOR_ADDRESS+MAXMESHSIZE_ADDRESS*CAPACITYMESH)   // Ϊ����֮�����Ϣ�����Ŀռ�


//������
#define SAFE_DELETE(p) {if(p){delete (p);(p)=NULL;}}
#define SAFE_CREATE(p,t) {if(p){delete (p);}(p)=new t;}
#define SAFE_DELETE_ARR(p) {if(p){delete[] (p);(p)=NULL;}}
#define SAFE_CREATE_ARR(p,t,sum) {if(p){delete[] (p);}(p)=new t[sum];}

//Ϊ�����ڴ��ṩ�ĺ�  ���������ڴ棬��ָ��ǳ����
#define MEMCOPY(Des,src,Type,Num)\
		Des = new Type[Num];\
		CopyMemory(Des, src, Num*sizeof(Type));\

namespace AssetMagr
{
#define meshNameMaxLength  80

	typedef float SDMfloat;
	typedef unsigned int SDMuint;
	typedef signed __int64 SDMint_64;

	union SDMfloat2
	{
		struct
		{
			SDMfloat x;
			SDMfloat y;
		}s;
		SDMfloat a[2];
	};

	union SDMfloat3 {
		struct
		{
			SDMfloat x;
			SDMfloat y;
			SDMfloat z;
		}s;
		SDMfloat a[3];
	};

	union SDMfloat4 {
		struct
		{
			SDMfloat x;
			SDMfloat y;
			SDMfloat z;
			SDMfloat w;
		}s;
		SDMfloat a[4];
	};

	union SDMuint2 {
		struct
		{
			SDMuint x;
			SDMuint y;
		}s;
		SDMuint a[2];
	};

	union SDMuint3 {

		struct
		{
			SDMuint x;
			SDMuint y;
			SDMuint z;
		}s;
		SDMuint a[3];
	};

	//ģ���������ݵ���Ŀ��Ϣ
	struct CntElemMesh
	{
		//public:
		SDMuint mCntVertex;		//����ģ�Ͱ����Ķ�������
		SDMuint mCntFace;		//����ģ�Ͱ����Ķ��������
		SDMuint mCntVertexNormal;
		SDMuint mCntFaceNormal;
		SDMuint mCntVertexColor;
		SDMuint mCntFaceColor;
		SDMuint mCntVertexTangent;
		SDMuint mCntFaceTangent;
		//SDMuint mCntPositiveUV;
		//SDMuint mCntNegativeUV;
		SDMuint mCntFaceIndexPositiveUV;
		SDMuint mCntFaceIndexNegativeUV;
		SDMuint mCntFaceVertexIndex;

		CntElemMesh& operator=(const CntElemMesh& src)
		{
			this->mCntFace = src.mCntFace;
			this->mCntFaceColor = src.mCntFaceColor;
			this->mCntFaceIndexNegativeUV = src.mCntFaceIndexNegativeUV;
			this->mCntFaceIndexPositiveUV = src.mCntFaceIndexPositiveUV;
			this->mCntFaceNormal = src.mCntFaceNormal;
			this->mCntFaceTangent = src.mCntFaceTangent;
			this->mCntFaceVertexIndex = src.mCntFaceVertexIndex;
			this->mCntVertex = src.mCntVertex;
			this->mCntVertexColor = src.mCntVertex;
			this->mCntVertexNormal = src.mCntVertexNormal;
			this->mCntVertexTangent = src.mCntVertexTangent;
			return *this;
		}

		CntElemMesh(const CntElemMesh& src)
		{
			this->mCntFace = src.mCntFace;
			this->mCntFaceColor = src.mCntFaceColor;
			this->mCntFaceIndexNegativeUV = src.mCntFaceIndexNegativeUV;
			this->mCntFaceIndexPositiveUV = src.mCntFaceIndexPositiveUV;
			this->mCntFaceNormal = src.mCntFaceNormal;
			this->mCntFaceTangent = src.mCntFaceTangent;
			this->mCntFaceVertexIndex = src.mCntFaceVertexIndex;
			this->mCntVertex = src.mCntVertex;
			this->mCntVertexColor = src.mCntVertex;
			this->mCntVertexNormal = src.mCntVertexNormal;
			this->mCntVertexTangent = src.mCntVertexTangent;
		}

		CntElemMesh()
		{
			mCntVertex = 0;		//����ģ�Ͱ����Ķ�������
			mCntFace = 0;		//����ģ�Ͱ����Ķ��������
			mCntVertexNormal = 0;
			mCntFaceNormal = 0;
			mCntVertexColor = 0;
			mCntFaceColor = 0;
			mCntVertexTangent = 0;
			mCntFaceTangent = 0;
			mCntFaceIndexPositiveUV = 0;
			mCntFaceIndexNegativeUV = 0;
			mCntFaceVertexIndex = 0;
		}

	};

	//Mesh��������Ϣ
	struct MeshIndexInfor
	{
		char name[meshNameMaxLength];
		long index;
		long offsetAdress;
	};


	//Ϊ����CntElemMesh�ṹ�еĳ�Ա�����ṩSet/Get�����İ�����
#define SET_SFBI_BUF_ACCESSOR(x, y )       inline void Set##y( SDMuint t )   { pCntElem##x->m##y = t; };
#define GET_SFBI_BUF_ACCESSOR(x, y )       inline SDMuint Get##y()           {return pCntElem##x->m##y; };
#define GET_SET_BUF_SFBI_ACCESSOR(x, y )   SET_SFBI_BUF_ACCESSOR(x, y ) GET_SFBI_BUF_ACCESSOR(x, y )


}




