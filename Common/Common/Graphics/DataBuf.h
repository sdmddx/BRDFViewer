#ifndef DATABUF_H_
#define DATABUF_H_

#include "list"

using namespace std;


namespace AssetMagr
{
	class COMMON_API DataBuf
	{

		int sumMesh;


	public:
		unique_ptr<list<unique_ptr<BufMesh>>> pMeshList;

		//unique_ptr<list<unique_ptr<BufMtl>>> pMtlList;

		~DataBuf();

		DataBuf();

		void Init();

		void Clear();

		//unique_ptr<BufMesh> GetMeshFromName(char* wch);


		//bool DeleteMeshFromName(wchar_t* wch);

		void WriteMesh(unique_ptr<BufMesh>& pmesh);

		//void WriteMtl(unique_ptr<BufMtl>& pmesh);

		int GetNumMesh();

		void SetNumMesh(int sum);

		//���ø�Buf�µ�����Mesh����ͼ�ļ�  ����Mesh����һ���ļ�
		void SetMeshesTex(char* pTexName);

		//���ø�Buf��ĳ��Mesh����ͼ�ļ�
		bool SetMeshTex(int64_t meshID, char* pTexName);

		BufMesh* GetBufMeshByIndex(unsigned int index);
	};


}

#endif


