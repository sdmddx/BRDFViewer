#include "stdafx.h"

using namespace Utilities;

ShaderLoader::ShaderLoader(const std::shared_ptr<DX::DXResources>& dxResources)
{
	dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	m_dxResources = dxResources;
}

HRESULT ShaderLoader::CreateVertexShader_IA(LPCWSTR pSrcFile, LPCSTR pFunctionName, LPCSTR pProfile, Microsoft::WRL::ComPtr<ID3D11VertexShader>& pVS,const D3D11_INPUT_ELEMENT_DESC* vertexDesc, UINT numElem, Microsoft::WRL::ComPtr<ID3D11InputLayout>& inputLayout)
{
	ID3DBlob* pBlob, *pErrorBlob;
	HRESULT hr = E_FAIL;
	hr = D3DX11CompileFromFile(pSrcFile, NULL, NULL, pFunctionName, pProfile, dwShaderFlags, 0, NULL, &pBlob, &pErrorBlob, NULL);
	if (!FAILED(hr))
	{
		hr=m_dxResources->GetD3DDevice()->CreateVertexShader(
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			nullptr,
			&pVS
		);
		if (!FAILED(hr))
		{
			hr = m_dxResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc,
				numElem,
				pBlob->GetBufferPointer(),
				pBlob->GetBufferSize(),
				&inputLayout
			);
		}
	}
	
	SAFERELEASE(pBlob);
	SAFERELEASE(pErrorBlob);
	return hr;
}

HRESULT ShaderLoader::CreatePixelShader(LPCWSTR pSrcFile, LPCSTR pFunctionName, LPCSTR pProfile, Microsoft::WRL::ComPtr<ID3D11PixelShader>& pPS)
{
	ID3DBlob* pBlob, *pErrorBlob;
	HRESULT hr = E_FAIL;
	hr = D3DX11CompileFromFile(pSrcFile, NULL, NULL, pFunctionName, pProfile, dwShaderFlags, 0, NULL, &pBlob, &pErrorBlob, NULL);
	if (!FAILED(hr))
	{
		hr = m_dxResources->GetD3DDevice()->CreatePixelShader(
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		nullptr,
		&pPS
	);
	}
	SAFERELEASE(pBlob);
	SAFERELEASE(pErrorBlob);
	return hr;
}

HRESULT ShaderLoader::CreateComputeShader(LPCWSTR pSrcFile, LPCSTR pFunctionName, LPCSTR pProfile, Microsoft::WRL::ComPtr<ID3D11ComputeShader>& pCS)
{
	ID3DBlob* pBlob, *pErrorBlob;
	HRESULT hr = E_FAIL;
	if (!FAILED(D3DX11CompileFromFile(pSrcFile, NULL, NULL, pFunctionName, pProfile, dwShaderFlags, 0, NULL, &pBlob, &pErrorBlob, NULL)))
	{
		hr = m_dxResources->GetD3DDevice()->CreateComputeShader(
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			nullptr,
			&pCS
		);
	}
	SAFERELEASE(pBlob);
	SAFERELEASE(pErrorBlob);
	return hr;
}