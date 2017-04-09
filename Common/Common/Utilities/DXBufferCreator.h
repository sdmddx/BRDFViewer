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
		HRESULT CreateTexSRVUAV(ID3D11Device3* pd3dDevice,Math::Size &size, DXGI_FORMAT tex_format,ID3D11Texture2D** pptex2D, ID3D11ShaderResourceView** ppSRV,ID3D11UnorderedAccessView** ppUAV)
		{
			HRESULT hr = E_FAIL;
			//
			D3D11_TEXTURE2D_DESC tex_desc = { 0 };
			tex_desc.ArraySize = 1;
			tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
			tex_desc.CPUAccessFlags = 0;
			tex_desc.Format = tex_format;
			tex_desc.Height = size.Height;
			tex_desc.Width = size.Width;
			tex_desc.MipLevels = 1;
			tex_desc.MiscFlags = 0;
			tex_desc.SampleDesc.Count = 1;
			tex_desc.SampleDesc.Quality = 0;
			tex_desc.Usage = D3D11_USAGE_DEFAULT;

			hr = pd3dDevice->CreateTexture2D(&tex_desc, NULL, pptex2D);

			//
			D3D11_SHADER_RESOURCE_VIEW_DESC Desc_S;
			ZeroMemory(&Desc_S, sizeof(Desc_S));
			Desc_S.Format = tex_desc.Format;
			Desc_S.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			Desc_S.Texture2D.MipLevels = tex_desc.MipLevels;
			Desc_S.Texture2D.MostDetailedMip = 0;

			hr = pd3dDevice->CreateShaderResourceView(
				*pptex2D,
				&Desc_S,
				ppSRV);
				

			//创建无序访问资源视图
			D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
			ZeroMemory(&uavDesc, sizeof(uavDesc));
			uavDesc.Format = tex_desc.Format;
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
			uavDesc.Texture2D.MipSlice = 0;							//????????????????

			hr = pd3dDevice->CreateUnorderedAccessView(*pptex2D,&uavDesc,ppUAV);

			return hr;
		}


	};


}
