#include "stdafx.h"

using namespace BasicGraphic;
using namespace AssetMagr;
using namespace DirectX;

SphereGenerator::SphereGenerator()
{
	pInit = (BasicSphere**)malloc(sizeof(BasicSphere*) * 2);
	pInit[0] = InitNeg();
	pInit[1] = InitPos();

	//ppTexFunc = (pptexFunc)malloc(sizeof(pTexFunc) * 1);
	//ppTexFunc[Flat] = &SphereGenerator::CreateTexFlat;

}

SphereGenerator::~SphereGenerator()
{
	free(pInit[0]->pVer);
	free(pInit[0]->pFace);
	delete pInit[0];

	free(pInit[1]->pVer);
	free(pInit[1]->pFace);
	delete pInit[1];

	free(pInit);

}

//初始化一个逆时针正八面体
BasicSphere* SphereGenerator::InitNeg()
{

	BasicSphere* pPosInit = new BasicSphere;


	XMVECTOR* pVertex = (XMVECTOR*)malloc(sizeof(XMVECTOR) * 6);
	pVertex[0] = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
	pVertex[1] = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	pVertex[2] = XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f);
	pVertex[3] = XMVectorSet(-1.0f, 0.0f, 0.0f, 1.0f);
	pVertex[4] = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	pVertex[5] = XMVectorSet(0.0f, -1.0f, 0.0f, 1.0f);

	SDMuint3* pFace = (SDMuint3*)malloc(sizeof(SDMuint3) * 8);
	SetFace(pFace[0], 0, 1, 2);
	SetFace(pFace[1], 2, 1, 3);
	SetFace(pFace[2], 3, 1, 4);
	SetFace(pFace[3], 4, 1, 0);
	SetFace(pFace[4], 0, 5, 4);
	SetFace(pFace[5], 4, 5, 3);
	SetFace(pFace[6], 3, 5, 2);
	SetFace(pFace[7], 2, 5, 0);

	pPosInit->pVer = pVertex;
	pPosInit->pFace = pFace;
	pPosInit->sumVer = 6;
	pPosInit->sumFace = 8;

	return pPosInit;
}

//初始化一个顺时针正八面体
BasicSphere* SphereGenerator::InitPos()
{

	BasicSphere* pNegInit = new BasicSphere;


	XMVECTOR* pVertex = (XMVECTOR*)malloc(sizeof(XMVECTOR) * 6);
	pVertex[0] = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
	pVertex[1] = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	pVertex[2] = XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f);
	pVertex[3] = XMVectorSet(-1.0f, 0.0f, 0.0f, 1.0f);
	pVertex[4] = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	pVertex[5] = XMVectorSet(0.0f, -1.0f, 0.0f, 1.0f);

	SDMuint3* pFace = (SDMuint3*)malloc(sizeof(SDMuint3) * 8);
	SetFace(pFace[0], 2, 1, 0);
	SetFace(pFace[1], 3, 1, 2);
	SetFace(pFace[2], 4, 1, 3);
	SetFace(pFace[3], 0, 1, 4);
	SetFace(pFace[4], 4, 5, 0);
	SetFace(pFace[5], 3, 5, 4);
	SetFace(pFace[6], 2, 5, 3);
	SetFace(pFace[7], 0, 5, 2);

	pNegInit->pVer = pVertex;
	pNegInit->pFace = pFace;
	pNegInit->sumVer = 6;
	pNegInit->sumFace = 8;

	return pNegInit;
}

//赋值一个Face
void SphereGenerator::SetFace(SDMuint3& pface, SDMuint x, SDMuint y, SDMuint z)
{
	pface.s.x = x;
	pface.s.y = y;
	pface.s.z = z;

}

//根据参数决定获得是顺时针序的还是逆时针序的
BasicSphere*  SphereGenerator::GenerateAtomSphere(TriangleOrder order)
{
	BasicSphere* pinit = pInit[order];
	XMVECTOR* pInit = pinit->pVer;
	SDMuint3* pFace = pinit->pFace;


	SDMuint sumVer = pinit->sumVer;
	SDMuint sumFace = pinit->sumFace;
	XMVECTOR* pVertex = (XMVECTOR*)malloc(sizeof(XMVECTOR)*(sumVer + sumFace * 3));
	SDMuint3* pFaces = (SDMuint3*)malloc(sizeof(SDMuint3)* sumFace * 4);

	SDMuint indexVer = 0;
	for (; indexVer < sumVer; ++indexVer)
	{
		pVertex[indexVer] = pInit[indexVer];
	}
	//对每个面进行迭代细分
	unsigned indexFace = 0;
	unsigned indexNewFace = 0;		//在新的面集合中下一个可以写入的位置
	for (; indexFace < sumFace; ++indexFace)
	{
		unsigned indexNewVer = indexVer;
		pVertex[indexVer++] = XMVector3Normalize(pInit[pFace[indexFace].s.x] + pInit[pFace[indexFace].s.y]);
		pVertex[indexVer++] = XMVector3Normalize(pInit[pFace[indexFace].s.y] + pInit[pFace[indexFace].s.z]);
		pVertex[indexVer++] = XMVector3Normalize(pInit[pFace[indexFace].s.z] + pInit[pFace[indexFace].s.x]);


		pFaces[indexNewFace].s.x = indexNewVer;
		pFaces[indexNewFace].s.y = pFace[indexFace].s.y;
		pFaces[indexNewFace++].s.z = indexNewVer + 1;

		pFaces[indexNewFace].s.x = indexNewVer + 1;
		pFaces[indexNewFace].s.y = pFace[indexFace].s.z;
		pFaces[indexNewFace++].s.z = indexNewVer + 2;

		pFaces[indexNewFace].s.x = indexNewVer + 2;
		pFaces[indexNewFace].s.y = pFace[indexFace].s.x;
		pFaces[indexNewFace++].s.z = indexNewVer;

		pFaces[indexNewFace].s.x = indexNewVer;
		pFaces[indexNewFace].s.y = indexNewVer + 1;
		pFaces[indexNewFace++].s.z = indexNewVer + 2;

	}

	BasicSphere* pnew = new BasicSphere;
	pnew->pVer = pVertex;
	pnew->pFace = pFaces;
	pnew->sumVer = sumVer + sumFace * 3;
	pnew->sumFace = sumFace * 4;

	return pnew;
}

//对生成的球体进行一次迭代细分-----不会改变顶点顺序
BasicSphere* SphereGenerator::subdivideSphere(BasicSphere* pinit)
{
	XMVECTOR* pInit = pinit->pVer;
	SDMuint3* pFace = pinit->pFace;

	SDMuint sumVer = pinit->sumVer;
	SDMuint sumFace = pinit->sumFace;

	XMVECTOR* pVertex = (XMVECTOR*)malloc(sizeof(XMVECTOR)*(sumVer + sumFace * 3));
	SDMuint3* pFaces = (SDMuint3*)malloc(sizeof(SDMuint3)* sumFace * 4);

	SDMuint indexVer = 0;
	for (; indexVer < sumVer; ++indexVer)
	{
		pVertex[indexVer] = pInit[indexVer];
	}
	//对每个面进行迭代细分
	unsigned indexFace = 0;
	unsigned indexNewFace = 0;		//在新的面集合中下一个可以写入的位置
	for (; indexFace < sumFace; ++indexFace)
	{
		unsigned indexNewVer = indexVer;
		pVertex[indexVer++] = XMVector3Normalize(pInit[pFace[indexFace].s.x] + pInit[pFace[indexFace].s.y]);
		pVertex[indexVer++] = XMVector3Normalize(pInit[pFace[indexFace].s.y] + pInit[pFace[indexFace].s.z]);
		pVertex[indexVer++] = XMVector3Normalize(pInit[pFace[indexFace].s.z] + pInit[pFace[indexFace].s.x]);


		pFaces[indexNewFace].s.x = indexNewVer;
		pFaces[indexNewFace].s.y = pFace[indexFace].s.y;
		pFaces[indexNewFace++].s.z = indexNewVer + 1;

		pFaces[indexNewFace].s.x = indexNewVer + 1;
		pFaces[indexNewFace].s.y = pFace[indexFace].s.z;
		pFaces[indexNewFace++].s.z = indexNewVer + 2;

		pFaces[indexNewFace].s.x = indexNewVer + 2;
		pFaces[indexNewFace].s.y = pFace[indexFace].s.x;
		pFaces[indexNewFace++].s.z = indexNewVer;

		pFaces[indexNewFace].s.x = indexNewVer;
		pFaces[indexNewFace].s.y = indexNewVer + 1;
		pFaces[indexNewFace++].s.z = indexNewVer + 2;

	}

	BasicSphere* pnew = new BasicSphere;
	pnew->pVer = pVertex;
	pnew->pFace = pFaces;
	pnew->sumVer = sumVer + sumFace * 3;
	pnew->sumFace = sumFace * 4;



	free(pinit->pVer);
	free(pinit->pFace);

	delete pinit;

	return pnew;


}

//生成一个PvN球
unique_ptr<DataBuf> SphereGenerator::GenerateSpherePvN(SDMuint count, TriangleOrder order)
{

	BasicSphere* pSphere = pInit[order];
	if (count > 0)
	{
		pSphere = GenerateAtomSphere(order);
		for (SDMuint i = 1; i < count; ++i)
		{
			pSphere = subdivideSphere(pSphere);
		}
	}

	unsigned sumVer = pSphere->sumVer;
	XMFLOAT3* pVertex = (XMFLOAT3*)malloc(sizeof(XMFLOAT3)*sumVer);
	for (SDMuint i = 0; i < sumVer; ++i)
	{
		XMStoreFloat3(&pVertex[i], pSphere->pVer[i]);
	}

	free(pSphere->pVer);

	//生成顶点法线
	XMFLOAT3* pVertexNormal = (XMFLOAT3*)malloc(sizeof(XMFLOAT3)*sumVer);
	for (SDMuint i = 0; i < sumVer; ++i)
	{
		pVertexNormal[i] = pVertex[i];
	}

	unique_ptr<DataBuf> pdataBuf = make_unique<DataBuf>();

	unique_ptr<BufMesh> pmesh = make_unique<BufMesh>();
	pmesh->pVertex = pVertex;
	pmesh->SetCntVertex(sumVer);
	pmesh->pFace = pSphere->pFace;
	pmesh->SetCntFace(pSphere->sumFace);
	pmesh->pVertexNormal = pVertexNormal;
	pmesh->SetCntVertexNormal(sumVer);

	pdataBuf->WriteMesh(pmesh);
	return pdataBuf;
}

//生成一个PfNvN球
void SphereGenerator::AddFaceNormal(BufMesh* pMesh)
{

	SDMuint3* pFace = pMesh->pFace;
	SDMuint sumFace = pMesh->GetCntFace();
	XMFLOAT3* pVer = pMesh->pVertex;
	pMesh->pFaceNormal = new XMFLOAT3[pMesh->GetCntFace()];
	XMFLOAT3* pFaceNor = pMesh->pFaceNormal;
	pMesh->SetCntFaceNormal(pMesh->GetCntFace());
	for (SDMuint indexFace = 0; indexFace < sumFace; ++indexFace)
	{
		//XMStoreFloat3(&pFaceNor[indexFace], XMVector3Normalize(XMLoadFloat3(&pVerNor[pFace[indexFace].a[0]]) + XMLoadFloat3(&pVerNor[pFace[indexFace].a[1]]) + XMLoadFloat3(&pVerNor[pFace[indexFace].a[2]])));

		XMStoreFloat3(&pFaceNor[indexFace], XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&pVer[pFace[indexFace].a[1]]) - XMLoadFloat3(&pVer[pFace[indexFace].a[0]]), XMLoadFloat3(&pVer[pFace[indexFace].a[2]]) - XMLoadFloat3(&pVer[pFace[indexFace].a[0]]))));
	}

}

//平面映射模式生成纹理坐标
void SphereGenerator::CreateTexFlat(BufMesh* pMesh)
{
	pMesh->pPositiveFaceIndexUV = new XMFLOAT2[pMesh->GetCntFace() * 3];
	XMFLOAT2* pPFIUV = pMesh->pPositiveFaceIndexUV;
	SDMuint3* pFace = pMesh->pFace;
	XMFLOAT3* pVer = pMesh->pVertex;
	for (SDMuint i = 0; i < pMesh->GetCntFace(); ++i)
	{
		for (int j = 0; j < 3; ++j)

		{
			pPFIUV[i * 3 + j].x = (pVer[pFace[i].a[j]].x + 1.0f) / 2.0f;

			pPFIUV[i * 3 + j].y = (pVer[pFace[i].a[j]].z + 1.0f) / 2.0f;
		}

	}
	pMesh->SetCntFaceIndexPositiveUV(pMesh->GetCntFace() * 3);
}

//生成一个纹理球
unique_ptr<DataBuf> SphereGenerator::GenerateSpherePTvN(SDMuint count, TriangleOrder order, TexMapperFunc func)
{
	unique_ptr<DataBuf> pdataBuf = GenerateSpherePvN(count, order);
	CreateTexFlat(pdataBuf->GetBufMeshByIndex(0));
	//ppTexFunc[func](pdataBuf->pCurMesh);
	return pdataBuf;
}
