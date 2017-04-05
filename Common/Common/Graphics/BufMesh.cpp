#include "stdafx.h"



using namespace AssetMagr;
using namespace DirectX;
using namespace std;


int BufMesh::MeshNameMaxLength = meshNameMaxLength;
int BufMesh::TexNameMaxLength = meshNameMaxLength;

//BufMesh::pDeleter BufMesh::Deleter = Deleter_Clear<BufMesh>;

unique_ptr<BufMesh> BufMesh::Clone()const
{
	unique_ptr<BufMesh> pBufMesh(new BufMesh);
	pBufMesh.reset();
	pBufMesh->name[MeshNameMaxLength] = this->name[MeshNameMaxLength];


	pBufMesh->texName[TexNameMaxLength] = this->texName[TexNameMaxLength];


	pBufMesh->pCntElemMesh = unique_ptr<CntElemMesh>(new CntElemMesh);
	*pBufMesh->pCntElemMesh = *(this->pCntElemMesh);				//是否能够正常复制 ，内部没有指针
	pBufMesh->CreateFromCntElem();
	MEMCOPY(pBufMesh->pVertex, this->pVertex, XMFLOAT3, pBufMesh->pCntElemMesh->mCntVertex);
	MEMCOPY(pBufMesh->pFace, this->pFace, SDMuint3, pBufMesh->pCntElemMesh->mCntFace);
	MEMCOPY(pBufMesh->pVertexNormal, this->pVertexNormal, XMFLOAT3, pBufMesh->pCntElemMesh->mCntVertexNormal);
	MEMCOPY(pBufMesh->pFaceNormal, this->pFaceNormal, XMFLOAT3, pBufMesh->pCntElemMesh->mCntFaceNormal);
	MEMCOPY(pBufMesh->pVertexTangent, this->pVertexTangent, XMFLOAT3, pBufMesh->pCntElemMesh->mCntVertexTangent);
	MEMCOPY(pBufMesh->pFaceTangent, this->pFaceTangent, XMFLOAT3, pBufMesh->pCntElemMesh->mCntFaceTangent);
	MEMCOPY(pBufMesh->pVertexColor, this->pVertexColor, XMFLOAT4, pBufMesh->pCntElemMesh->mCntVertexColor);
	MEMCOPY(pBufMesh->pFaceColor, this->pFaceColor, XMFLOAT4, pBufMesh->pCntElemMesh->mCntFaceColor);
	MEMCOPY(pBufMesh->pPositiveFaceIndexUV, this->pPositiveFaceIndexUV, XMFLOAT2, pBufMesh->pCntElemMesh->mCntFaceIndexPositiveUV);
	MEMCOPY(pBufMesh->pNegativeFaceIndexUV, this->pNegativeFaceIndexUV, XMFLOAT2, pBufMesh->pCntElemMesh->mCntFaceIndexPositiveUV);
	MEMCOPY(pBufMesh->pFaceVertexIndex, this->pFaceVertexIndex, SDMuint, pBufMesh->pCntElemMesh->mCntFaceVertexIndex);

	pBufMesh->flag = flag;
	return pBufMesh;
}

BufMesh::BufMesh()
{
	//SAFE_CREATE(pVertex,XMFLOAT3);
	pVertex = nullptr;
	pFace = nullptr;
	pVertexColor = nullptr;
	pFaceColor = nullptr;
	pVertexNormal = nullptr;
	pFaceNormal = nullptr;
	pVertexTangent = nullptr;
	pFaceTangent = nullptr;
	pPositiveFaceIndexUV = nullptr;
	pNegativeFaceIndexUV = nullptr;
	pFaceVertexIndex = nullptr;

	//unique_ptr<char[]> p;
	//p = unique_ptr<char[]>(new char[5]);
	name = new char[MeshNameMaxLength];
	name[0] = { '\0' };

	texName = new char[TexNameMaxLength];
	texName[0] = { '\0' };

	pCntElemMesh = make_unique<CntElemMesh>();
	pCntElemMesh->mCntVertex = 0; pCntElemMesh->mCntFace = 0;
	pCntElemMesh->mCntVertexColor = 0; pCntElemMesh->mCntFaceColor = 0;
	pCntElemMesh->mCntVertexNormal = 0; pCntElemMesh->mCntFaceNormal = 0;
	pCntElemMesh->mCntVertexTangent = 0; pCntElemMesh->mCntFaceTangent = 0;
	pCntElemMesh->mCntFaceIndexPositiveUV = 0; pCntElemMesh->mCntFaceIndexNegativeUV = 0;
	pCntElemMesh->mCntFaceVertexIndex = 0;
	flag = 0;
}

void BufMesh::Clear()
{

	SAFE_DELETE_ARR(pVertex);
	SAFE_DELETE_ARR(pFace);
	SAFE_DELETE_ARR(pVertexColor);
	SAFE_DELETE_ARR(pFaceColor);
	SAFE_DELETE_ARR(pVertexNormal);
	SAFE_DELETE_ARR(pFaceNormal);
	SAFE_DELETE_ARR(pVertexTangent);
	SAFE_DELETE_ARR(pFaceTangent);
	SAFE_DELETE_ARR(pPositiveFaceIndexUV);
	SAFE_DELETE_ARR(pNegativeFaceIndexUV);
	SAFE_DELETE_ARR(pFaceVertexIndex);
	SAFE_DELETE_ARR(name);
	SAFE_DELETE_ARR(texName);


	pCntElemMesh->mCntVertex = 0; pCntElemMesh->mCntFace = 0;
	pCntElemMesh->mCntVertexColor = 0; pCntElemMesh->mCntFaceColor = 0;
	pCntElemMesh->mCntVertexNormal = 0; pCntElemMesh->mCntFaceNormal = 0;
	pCntElemMesh->mCntVertexTangent = 0; pCntElemMesh->mCntFaceTangent = 0;
	pCntElemMesh->mCntFaceIndexPositiveUV = 0; pCntElemMesh->mCntFaceVertexIndex = 0;

	flag = 0;

}

BufMesh::~BufMesh()
{
	SAFE_DELETE_ARR(pVertex);
	SAFE_DELETE_ARR(pFace);
	SAFE_DELETE_ARR(pVertexColor);
	SAFE_DELETE_ARR(pFaceColor);
	SAFE_DELETE_ARR(pVertexNormal);
	SAFE_DELETE_ARR(pFaceNormal);
	SAFE_DELETE_ARR(pVertexTangent);
	SAFE_DELETE_ARR(pFaceTangent);
	SAFE_DELETE_ARR(pPositiveFaceIndexUV);
	SAFE_DELETE_ARR(pNegativeFaceIndexUV);
	SAFE_DELETE_ARR(pFaceVertexIndex);

	//SAFE_DELETE_ARR(name);
	SAFE_DELETE_ARR(name);
	SAFE_DELETE_ARR(texName);
}


void BufMesh::GetName(char *meshName)const
{
	for (int h = 0; h<MeshNameMaxLength; ++h)
	{
		meshName[h] = name[h];
	}

}

unique_ptr<char[]> BufMesh::GetName()const
{
	char* meshName = new char[MeshNameMaxLength];
	for (int h = 0; h<MeshNameMaxLength; ++h)
	{
		meshName[h] = name[h];
	}
	return unique_ptr<char[]>(meshName);
}

void BufMesh::SetName(const char* pmeshName)
{
	//首先检测pmeshName的长度
	int h = 0;
	while (pmeshName[h] != '\0')
	{
		++h;
	}
	if (h > MeshNameMaxLength) { h = MeshNameMaxLength; }
	for (int i = 0; i<h; ++i)
	{
		name[i] = pmeshName[i];
	}
	for (int j = h; j<MeshNameMaxLength; ++j)
	{
		name[j] = '\0';
	}
	GenerateId();
}


void BufMesh::GetTexName(char *texName)const
{
	for (int h = 0; h<TexNameMaxLength; ++h)
	{
		texName[h] = texName[h];
	}

}

unique_ptr<char[]> BufMesh::GetTexName()const
{
	char* texName = new char[TexNameMaxLength];
	for (int h = 0; h<TexNameMaxLength; ++h)
	{
		texName[h] = texName[h];
	}
	return unique_ptr<char[]>(texName);
}

void BufMesh::SetTexName(const char* ptexName)
{
	//首先检测pmeshName的长度
	int h = 0;
	while (ptexName[h] != '\0')
	{
		++h;
	}
	if (h > TexNameMaxLength) { h = TexNameMaxLength; }
	for (int i = 0; i<h; ++i)
	{
		texName[i] = ptexName[i];
	}
	for (int j = h; j<TexNameMaxLength; ++j)
	{
		texName[j] = '\0';
	}
}

CntElemMesh* BufMesh::GetCntElem()
{
	return pCntElemMesh.get();
}

void BufMesh::CreateFromCntElem()
{
	if (pCntElemMesh->mCntVertex>0) pVertex = new XMFLOAT3[pCntElemMesh->mCntVertex];
	if (pCntElemMesh->mCntFace>0) pFace = new SDMuint3[pCntElemMesh->mCntFace];
	if (pCntElemMesh->mCntVertexColor>0) pVertexColor = new XMFLOAT4[pCntElemMesh->mCntVertexColor];
	if (pCntElemMesh->mCntFaceColor>0) pFaceColor = new XMFLOAT4[pCntElemMesh->mCntFaceColor];
	if (pCntElemMesh->mCntVertexNormal>0) pVertexNormal = new XMFLOAT3[pCntElemMesh->mCntVertexNormal];
	if (pCntElemMesh->mCntFaceNormal>0) pFaceNormal = new XMFLOAT3[pCntElemMesh->mCntFaceNormal];
	if (pCntElemMesh->mCntVertexTangent>0) pVertexTangent = new XMFLOAT3[pCntElemMesh->mCntVertexTangent];
	if (pCntElemMesh->mCntFaceTangent>0) pFaceTangent = new XMFLOAT3[pCntElemMesh->mCntFaceTangent];
	if (pCntElemMesh->mCntFaceIndexPositiveUV>0) pPositiveFaceIndexUV = new XMFLOAT2[pCntElemMesh->mCntFaceIndexPositiveUV];
	if (pCntElemMesh->mCntFaceIndexNegativeUV>0) pNegativeFaceIndexUV = new XMFLOAT2[pCntElemMesh->mCntFaceIndexNegativeUV];
	if (pCntElemMesh->mCntFaceVertexIndex>0) pFaceVertexIndex = new SDMuint[pCntElemMesh->mCntFaceVertexIndex];
}

void BufMesh::GenerateId()
{
	id = BKDRHash(name);
}