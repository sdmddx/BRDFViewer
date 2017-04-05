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

		//设置该Buf下的所有Mesh的贴图文件  所有Mesh共享一个文件
		void SetMeshesTex(char* pTexName);

		//设置该Buf下某个Mesh的贴图文件
		bool SetMeshTex(int64_t meshID, char* pTexName);

		BufMesh* GetBufMeshByIndex(unsigned int index);
	};


}

#endif


