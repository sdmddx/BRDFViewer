#pragma once

namespace Utilities
{


	class COMMON_API DXBufferCreator
	{


	private:
		static DXBufferCreator* const pInstance;

	private:
		~DXBufferCreator();
	public:
		static DXBufferCreator* Create();

		//创建结构缓冲区
		template <typename T>
		HRESULT CreateStructuredBuffer(ID3D11Device3* pd3dDevice, UINT iNumElements, ID3D11Buffer** ppBuffer, ID3D11ShaderResourceView** ppSRV, ID3D11UnorderedAccessView** ppUAV, const T* pInitialData=nullptr)
		{
			HRESULT hr = S_FALSE;

			//创建结构缓冲区
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(bufferDesc));
			bufferDesc.ByteWidth = iNumElements * sizeof(T);
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
			bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			bufferDesc.StructureByteStride = sizeof(T);

			D3D11_SUBRESOURCE_DATA bufferInitData;
			ZeroMemory(&bufferInitData, sizeof(bufferInitData));
			bufferInitData.pSysMem = pInitialData;
			hr = pd3dDevice->CreateBuffer(&bufferDesc, (pInitialData) ? &bufferInitData : NULL, ppBuffer);

			// 创建SRV
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			ZeroMemory(&srvDesc, sizeof(srvDesc));
			srvDesc.Format = DXGI_FORMAT_UNKNOWN;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			srvDesc.Buffer.ElementWidth = iNumElements;
			hr = pd3dDevice->CreateShaderResourceView(*ppBuffer, &srvDesc, ppSRV);

			//创建UAV
			D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
			ZeroMemory(&uavDesc, sizeof(uavDesc));
			uavDesc.Format = DXGI_FORMAT_UNKNOWN;
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			uavDesc.Buffer.NumElements = iNumElements;
			hr = pd3dDevice->CreateUnorderedAccessView(*ppBuffer, &uavDesc, ppUAV);

			return hr;
		}

		//创建结构缓冲区
		template <typename T>
		HRESULT CreateStructuredBuffer(ID3D11Device3* pd3dDevice, UINT iNumElements, ID3D11Buffer** ppBuffer, ID3D11ShaderResourceView** ppSRV, const T* pInitialData = nullptr)
		{
			HRESULT hr = S_FALSE;

			//创建结构缓冲区
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(bufferDesc));
			bufferDesc.ByteWidth = iNumElements * sizeof(T);
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
			bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			bufferDesc.StructureByteStride = sizeof(T);

			D3D11_SUBRESOURCE_DATA bufferInitData;
			ZeroMemory(&bufferInitData, sizeof(bufferInitData));
			bufferInitData.pSysMem = pInitialData;
			hr = pd3dDevice->CreateBuffer(&bufferDesc, (pInitialData) ? &bufferInitData : NULL, ppBuffer);

			// 创建SRV
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			ZeroMemory(&srvDesc, sizeof(srvDesc));
			srvDesc.Format = DXGI_FORMAT_UNKNOWN;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			srvDesc.Buffer.ElementWidth = iNumElements;
			hr = pd3dDevice->CreateShaderResourceView(*ppBuffer, &srvDesc, ppSRV);

			return hr;
		}

		//创建常量缓冲区
		template<typename T>
		HRESULT CreateContantBuffer(ID3D11Device3* pd3dDevice, ID3D11Buffer** ppBuffer)
		{
			CD3D11_BUFFER_DESC CB_Desc(sizeof(T), D3D11_BIND_CONSTANT_BUFFER);
			return pd3dDevice->CreateBuffer(
				&CB_Desc,
				nullptr,
				ppBuffer);
		}

	//创建Texture2D并从中创建RTV SRV

	};


}
