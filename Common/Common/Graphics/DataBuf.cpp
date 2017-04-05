#include "stdafx.h"



using namespace AssetMagr;
using namespace std;
DataBuf::DataBuf()
{
	Init();
}

void DataBuf::Init()
{
	pMeshList = unique_ptr<list<unique_ptr<BufMesh>>>(new list<unique_ptr<BufMesh>>);
	//pMtlList = unique_ptr<list<unique_ptr<BufMtl>>>(new list<unique_ptr<BufMtl>>);
}

int DataBuf::GetNumMesh()
{
	return sumMesh;
}

void DataBuf::SetNumMesh(int sum)
{
	sumMesh = sum;
}

DataBuf::~DataBuf()
{

}

void DataBuf::Clear()
{
	pMeshList = nullptr;
	//pMtlList = nullptr;
	Init();

}

//设置该Buf下的所有Mesh的贴图文件  所有Mesh共享一个文件
void DataBuf::SetMeshesTex(char* pTexName)
{
	for (list<unique_ptr<BufMesh>>::iterator it = pMeshList->begin(); it != pMeshList->end(); ++it)
	{
		(*it)->SetTexName(pTexName);
	}
}

//设置该Buf下某个Mesh的贴图文件
bool DataBuf::SetMeshTex(int64_t meshID, char* pTexName)
{
	for (list<unique_ptr<BufMesh>>::iterator it = pMeshList->begin(); it != pMeshList->end(); ++it)
	{
		if ((*it)->id == meshID)
		{
			(*it)->SetTexName(pTexName);
			return true;
		}
	}
	return false;

}

void DataBuf::WriteMesh(unique_ptr<BufMesh>& pmesh)
{
	pMeshList->push_back(move(pmesh));
	++sumMesh;
}

/*
void DataBuf::WriteMtl(unique_ptr<BufMtl>& pmtl)
{
pMtlList->push_back(pmtl);
}
*/

//index从0开始
BufMesh* DataBuf::GetBufMeshByIndex(unsigned int index)
{
	list<unique_ptr<BufMesh>>::iterator it = pMeshList->begin();
	while (index-- > 0)
	{
		++it;
	};
	return (*it).get();

}