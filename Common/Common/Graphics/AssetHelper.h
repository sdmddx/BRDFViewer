#pragma once

#include <DirectXMath.h>

using namespace DirectX;
//Mesh文件中的
#define MESH_SELFINFOR_ADDRESS 100l		//mesh自身信息所在处地址
#define SUMMESH_ADDRESS 110l	//mesh总数信息所在处地址
#define MESHINDEXINFOR_ADDRESS 200l		//Mesh索引信息所在处地址
#define CAPACITYMESH 1000l		//最大Mesh容量
#define MAXMESHSIZE_ADDRESS 100l	//单个Mesh索引信息所占字节大小
#define MESH (MESHINDEXINFOR_ADDRESS+MAXMESHSIZE_ADDRESS*CAPACITYMESH)   // 为正文之外的信息留出的空间


//辅助宏
#define SAFE_DELETE(p) {if(p){delete (p);(p)=NULL;}}
#define SAFE_CREATE(p,t) {if(p){delete (p);}(p)=new t;}
#define SAFE_DELETE_ARR(p) {if(p){delete[] (p);(p)=NULL;}}
#define SAFE_CREATE_ARR(p,t,sum) {if(p){delete[] (p);}(p)=new t[sum];}

//为复制内存提供的宏  仅仅复制内存，即指针浅复制
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

	//模型网格数据的数目信息
	struct CntElemMesh
	{
		//public:
		SDMuint mCntVertex;		//单个模型包含的顶点数量
		SDMuint mCntFace;		//单个模型包含的多边形数量
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
			mCntVertex = 0;		//单个模型包含的顶点数量
			mCntFace = 0;		//单个模型包含的多边形数量
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

	//Mesh块索引信息
	struct MeshIndexInfor
	{
		char name[meshNameMaxLength];
		long index;
		long offsetAdress;
	};


	//为操作CntElemMesh结构中的成员变量提供Set/Get函数的帮助宏
#define SET_SFBI_BUF_ACCESSOR(x, y )       inline void Set##y( SDMuint t )   { pCntElem##x->m##y = t; };
#define GET_SFBI_BUF_ACCESSOR(x, y )       inline SDMuint Get##y()           {return pCntElem##x->m##y; };
#define GET_SET_BUF_SFBI_ACCESSOR(x, y )   SET_SFBI_BUF_ACCESSOR(x, y ) GET_SFBI_BUF_ACCESSOR(x, y )


}




