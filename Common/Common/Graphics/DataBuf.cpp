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

//���ø�Buf�µ�����Mesh����ͼ�ļ�  ����Mesh����һ���ļ�
void DataBuf::SetMeshesTex(char* pTexName)
{
	for (list<unique_ptr<BufMesh>>::iterator it = pMeshList->begin(); it != pMeshList->end(); ++it)
	{
		(*it)->SetTexName(pTexName);
	}
}

//���ø�Buf��ĳ��Mesh����ͼ�ļ�
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

//index��0��ʼ
BufMesh* DataBuf::GetBufMeshByIndex(unsigned int index)
{
	list<unique_ptr<BufMesh>>::iterator it = pMeshList->begin();
	while (index-- > 0)
	{
		++it;
	};
	return (*it).get();

}