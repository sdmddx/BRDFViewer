#ifndef BUFMESH_H_
#define BUFMESH_H_


using namespace std;
using namespace DirectX;

/*
template<typename T>
static void Deleter_Clear(T* p)
{
p->Clear();
delete p;
}

static auto unique_deleter_BufMesh = [](BufMesh* pmesh)
{
pmesh->Clear();
delete pmesh;
};*/

namespace AssetMagr
{


	class COMMON_API BufMesh
	{

	private:
		//禁用复制构造函数和复制操作符
		BufMesh(const BufMesh&) = delete;;

		BufMesh& operator=(const BufMesh&) = delete;

		unique_ptr<CntElemMesh> pCntElemMesh;

		//依据name生成id的函数
		void GenerateId();
	public:
		XMFLOAT3* pVertex;		//顶点
		XMFLOAT3* pVertexNormal;		//顶点法线
		XMFLOAT3* pFaceNormal;		//面法线
		XMFLOAT3* pVertexTangent;		//顶点切线
		XMFLOAT3* pFaceTangent;		//面切线
		XMFLOAT4* pVertexColor;		//顶点颜色
		XMFLOAT4* pFaceColor;		//面颜色
		SDMuint3* pFace;
		//XMFLOAT2 *ppositiveUV; 					//正面UV
		//XMFLOAT2 *pnegativeUV; 					//反面UV

		XMFLOAT2 *pPositiveFaceIndexUV;						//正面UV坐标数组 宽度是CountFace*
		XMFLOAT2 *pNegativeFaceIndexUV;						//反面UV坐标数组 宽度是CountFace*
		SDMuint *pFaceVertexIndex;				//与面关联的顶点索引数组

		static int MeshNameMaxLength;
		static int TexNameMaxLength;


		int flag;	//指示其中真正含有的数据信息

		char* name;

		//在Mesh文件的相对路径处
		char* texName;
		int64_t id;		//依据name生成的Mesh的ID


						//根据Cntelem结构创建合理内存
		void CreateFromCntElem();

		BufMesh();
		~BufMesh();

		void GetName(char*)const;
		unique_ptr<char[]> GetName()const;
		void SetName(const char* pmeshName);

		void GetTexName(char* texName)const;
		unique_ptr<char[]> GetTexName()const;
		void SetTexName(const char* ptexName);

		void Clear();

		//深度复制
		unique_ptr<BufMesh> Clone()const;



		CntElemMesh* GetCntElem();

		GET_SET_BUF_SFBI_ACCESSOR(Mesh, CntVertex);
		GET_SET_BUF_SFBI_ACCESSOR(Mesh, CntFace);
		GET_SET_BUF_SFBI_ACCESSOR(Mesh, CntVertexNormal);
		GET_SET_BUF_SFBI_ACCESSOR(Mesh, CntFaceNormal);
		GET_SET_BUF_SFBI_ACCESSOR(Mesh, CntVertexColor);
		GET_SET_BUF_SFBI_ACCESSOR(Mesh, CntFaceColor);
		GET_SET_BUF_SFBI_ACCESSOR(Mesh, CntVertexTangent);
		GET_SET_BUF_SFBI_ACCESSOR(Mesh, CntFaceTangent);
		GET_SET_BUF_SFBI_ACCESSOR(Mesh, CntFaceIndexPositiveUV);
		GET_SET_BUF_SFBI_ACCESSOR(Mesh, CntFaceIndexNegativeUV);
		GET_SET_BUF_SFBI_ACCESSOR(Mesh, CntFaceVertexIndex);

	};

}

#endif


