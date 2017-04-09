#pragma once

#include <d3dx11async.h>
#include <d3dcompiler.h>

namespace Utilities
{
	class COMMON_API ShaderLoader
	{
	private:
		
		DWORD dwShaderFlags;
		std::shared_ptr<DX::DXResources> m_dxResources;
	public:
		ShaderLoader(const std::shared_ptr<DX::DXResources>& dxResources);

		HRESULT CreateVertexShader_IA(LPCWSTR pSrcFile, LPCSTR pFunctionName, LPCSTR pProfile, Microsoft::WRL::ComPtr<ID3D11VertexShader>& pVS,const D3D11_INPUT_ELEMENT_DESC* vertexDesc,UINT numElem, Microsoft::WRL::ComPtr<ID3D11InputLayout>& inputLayout);
		HRESULT CreatePixelShader(LPCWSTR pSrcFile, LPCSTR pFunctionName, LPCSTR pProfile, Microsoft::WRL::ComPtr<ID3D11PixelShader>& pPS);
		HRESULT CreateComputeShader(LPCWSTR pSrcFile, LPCSTR pFunctionName, LPCSTR pProfile, Microsoft::WRL::ComPtr<ID3D11ComputeShader>& pPS);
	};
}
