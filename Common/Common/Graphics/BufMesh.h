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
		//���ø��ƹ��캯���͸��Ʋ�����
		BufMesh(const BufMesh&) = delete;;

		BufMesh& operator=(const BufMesh&) = delete;

		unique_ptr<CntElemMesh> pCntElemMesh;

		//����name����id�ĺ���
		void GenerateId();
	public:
		XMFLOAT3* pVertex;		//����
		XMFLOAT3* pVertexNormal;		//���㷨��
		XMFLOAT3* pFaceNormal;		//�淨��
		XMFLOAT3* pVertexTangent;		//��������
		XMFLOAT3* pFaceTangent;		//������
		XMFLOAT4* pVertexColor;		//������ɫ
		XMFLOAT4* pFaceColor;		//����ɫ
		SDMuint3* pFace;
		//XMFLOAT2 *ppositiveUV; 					//����UV
		//XMFLOAT2 *pnegativeUV; 					//����UV

		XMFLOAT2 *pPositiveFaceIndexUV;						//����UV�������� �����CountFace*
		XMFLOAT2 *pNegativeFaceIndexUV;						//����UV�������� �����CountFace*
		SDMuint *pFaceVertexIndex;				//��������Ķ�����������

		static int MeshNameMaxLength;
		static int TexNameMaxLength;


		int flag;	//ָʾ�����������е�������Ϣ

		char* name;

		//��Mesh�ļ������·����
		char* texName;
		int64_t id;		//����name���ɵ�Mesh��ID


						//����Cntelem�ṹ���������ڴ�
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

		//��ȸ���
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


