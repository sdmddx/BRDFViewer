#pragma once

#include "Common\Common\Common.h"

namespace Example
{
	class Pref_PBR_DS_IS_IBL_Scene :public Scene::SceneBase
	{
	private:
		//使用CS收集BRDF
		bool  IntegrateBRDF();

	public:
		Pref_PBR_DS_IS_IBL_Scene(const std::shared_ptr<DX::DXResources>& dxResources);

		~Pref_PBR_DS_IS_IBL_Scene();

		virtual void CreateDeviceDependentResources();
		virtual void CreateWindowSizeDependentResources();
		virtual void ReleaseDeviceDependentResources();
		virtual void Update(Utilities::StepTimer const& timer);
		virtual void Render();


		void Init_SimpleShader();
		void Render_SampleShader();
	private:

		// 缓存的设备资源指针。
		std::shared_ptr<DX::DXResources> m_dxResources;

		// 立体几何的 Direct3D 资源。
		Microsoft::WRL::ComPtr<ID3D11InputLayout>				m_IL_G;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>				m_IL_S;
		Microsoft::WRL::ComPtr<ID3D11Buffer>					m_VB_G;
		Microsoft::WRL::ComPtr<ID3D11Buffer>					m_VB_S;
		Microsoft::WRL::ComPtr<ID3D11Buffer>					m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>				m_VS_G;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>				m_PS_G;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>				m_VS_S;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>				m_PS_S;
		Microsoft::WRL::ComPtr<ID3D11ComputeShader>				m_computeShader;

		Microsoft::WRL::ComPtr<ID3D11Buffer>					m_simpleVB;
		Microsoft::WRL::ComPtr<ID3D11Buffer>					m_simpleIB;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>				m_simpleIL;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>				m_simpleVS;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>				m_simplePS;

		Microsoft::WRL::ComPtr<ID3D11Buffer>					m_D3D_CB_WVP;
		Microsoft::WRL::ComPtr<ID3D11Buffer>					m_D3D_CB_WVP_Inverse;
		Microsoft::WRL::ComPtr<ID3D11Buffer>					m_D3D_CB_WINDOWINFO;
		Microsoft::WRL::ComPtr<ID3D11Buffer>					m_D3D_CB_PBR;
		Microsoft::WRL::ComPtr<ID3D11Buffer>					m_D3D_CB_Light;
		Microsoft::WRL::ComPtr<ID3D11Buffer>					m_D3D_CB_MatInfo_albedo_metalness;


		//环境图SRV
		ID3D11ShaderResourceView*								m_SRV_envMap;
		ID3D11ShaderResourceView*								m_cubeMap[6];

		//法线向量纹理SRV RTV
		Microsoft::WRL::ComPtr<ID3D11Texture2D>					m_tex_verNor;
		ID3D11ShaderResourceView*								m_SRV_verNorTex;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>          m_RTV_verNorTex;

		//albedo和metalness的纹理SRV RTV
		Microsoft::WRL::ComPtr<ID3D11Texture2D>					m_tex_AlbedoMetalness;
		ID3D11ShaderResourceView*								m_SRV__AlbedoMetalness;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>          m_RTV__AlbedoMetalness;

		//收集BRDF的纹理 SRV UAV
		Microsoft::WRL::ComPtr<ID3D11Texture2D>					m_tex_InteBRDF;
		ID3D11ShaderResourceView*								m_SRV_InteBRDF;
		ID3D11UnorderedAccessView*								m_UAV_InteBRDF;

		Microsoft::WRL::ComPtr<ID3D11SamplerState>				sampler;

		unique_ptr<Utilities::SdmCamera>						m_camera;

		unique_ptr<Utilities::ShaderLoader>						m_shaderLoader;
		Utilities::DXBufferCreator*								m_dxBufferCreator;

		UINT32	m_indexCount;

		//常量缓冲区
		unique_ptr<CB_WVP>										m_CB_WVP_Data;
		unique_ptr<CB_PBR>										m_CB_PBR_Data;
		unique_ptr<CB_WINDOWINFO>								m_CB_WINDOWINFO_Data;//在窗口大小改变时更新该值
		unique_ptr<CB_WVP_Inverse>								m_CB_WVP_Inverse_Data;
		unique_ptr<CB_Light>									m_CB_Light_Data;
		unique_ptr<CB_MatInfo_albedo_metalness>					m_CB_MatInfo_albedo_metalness_Data;

		//世界变换矩阵的逆矩阵
		XMMATRIX												model_inverse;

		bool m_loadingComplete;

		BOOL m_isLButtonDown;
		BOOL m_isRButtonDown;

		//不是线程安全的
		Math::Point m_leftPoint;				//跟踪鼠标左键按下时所在位置，该位置在调用一次update()方法后会更新至最新位置
		Math::Point m_leftPointMovement;		//在鼠标左键按下时记录移动鼠标后的更新位置
		Math::Point m_rightPoint;				//跟踪鼠标右键按下时所在位置，该位置在调用一次update()方法后会更新至最新位置
		Math::Point m_rightPointMovement;		//在鼠标右键按下时记录移动鼠标后的更新位置
		SHORT m_mouseWheel;			//记录鼠标滑轮滚动delta

	public:
		//相机变换相应的事件
		void OnLButtonDown(LPARAM lparam)
		{
			m_isLButtonDown = true;
			m_leftPoint.x = LOWORD(lparam);
			m_leftPoint.y = HIWORD(lparam);
			m_leftPointMovement = m_leftPoint;

		}

		void OnLButtonUp(LPARAM lparam)
		{
			m_isLButtonDown = false;

		}

		void OnRButtonDown(LPARAM lparam)
		{
			m_isRButtonDown = true;
			m_rightPoint.x = LOWORD(lparam);
			m_rightPoint.y = HIWORD(lparam);
			m_rightPointMovement = m_rightPoint;


		}

		void OnRButtonUp(LPARAM lparam)
		{
			m_isRButtonDown = false;

		}

		void MouseMove(LPARAM lparam)
		{
			if (m_isLButtonDown)
			{
				m_leftPointMovement.x = LOWORD(lparam);
				m_leftPointMovement.y = HIWORD(lparam);
			}
			if (m_isRButtonDown)
			{
				m_rightPointMovement.x = LOWORD(lparam);
				m_rightPointMovement.y = HIWORD(lparam);
			}
		}

		void MouseWheel(short delta)
		{
			m_mouseWheel += delta;
		}

		//鼠标离开客户区
		void MouseLeave()
		{
			m_isLButtonDown = false;
			m_isRButtonDown = false;
		}
	};
}


