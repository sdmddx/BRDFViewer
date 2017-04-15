#pragma once

#include "Common\Common\Common.h"

namespace Example
{
	class BP_DS_ML :public Scene::SceneBase, public Utilities::MessageProcessor
	{
	public:
		BP_DS_ML(const std::shared_ptr<DX::DXResources>& dxResources);

		~BP_DS_ML();

		virtual void CreateDeviceDependentResources();
		virtual void CreateWindowSizeDependentResources();
		virtual void ReleaseDeviceDependentResources();
		virtual void Update(Utilities::StepTimer const& timer);
		virtual void Render();

	private:

		// ������豸��Դָ�롣
		std::shared_ptr<DX::DXResources> m_dxResources;

		// ���弸�ε� Direct3D ��Դ��
		Microsoft::WRL::ComPtr<ID3D11InputLayout>				m_IL_G;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>				m_IL_S;
		Microsoft::WRL::ComPtr<ID3D11Buffer>					m_VB_G;
		Microsoft::WRL::ComPtr<ID3D11Buffer>					m_VB_S;
		Microsoft::WRL::ComPtr<ID3D11Buffer>					m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>				m_VS_G;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>				m_VS_S;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>				m_PS_G;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>				m_PS_S;
		Microsoft::WRL::ComPtr<ID3D11Buffer>					m_D3D_CB_WVP;
		Microsoft::WRL::ComPtr<ID3D11Buffer>					m_D3D_CB_WVP_Inverse;
		Microsoft::WRL::ComPtr<ID3D11Buffer>					m_D3D_CB_WINDOWINFO;
		Microsoft::WRL::ComPtr<ID3D11Buffer>					m_D3D_CB_LightsInfo;

		//����ͼSRV
		ID3D11ShaderResourceView*								m_SRV_envMap;

		//������������SRV RTV
		Microsoft::WRL::ComPtr<ID3D11Texture2D>					m_tex_verNor;
		ID3D11ShaderResourceView*								m_SRV_verNorTex;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>          m_RTV_verNorTex;

		//��Դ��ϢSRV
		ID3D11ShaderResourceView*								m_SRV_LightInfo;


		Microsoft::WRL::ComPtr<ID3D11SamplerState>				sampler;

		unique_ptr<Utilities::SdmCamera>						m_camera;

		unique_ptr<Utilities::ShaderLoader>						m_shaderLoader;

		//DXBufferCreator
		Utilities::DXBufferCreator*						m_dxBufferCreator;

		UINT32	m_indexCount;

		//����������
		unique_ptr<CB_WVP>										m_CB_WVP_Data;
		unique_ptr<CB_WINDOWINFO>								m_CB_WINDOWINFO_Data;//�ڴ��ڴ�С�ı�ʱ���¸�ֵ
		unique_ptr<CB_WVP_Inverse>								m_CB_WVP_Inverse_Data;
		unique_ptr<LightsInfo>									m_CB_LightsInfo_Data;
		//����任����������
		XMMATRIX												model_inverse;
		UINT													numLights;
		bool m_loadingComplete;

		BOOL m_isLButtonDown;
		BOOL m_isRButtonDown;

		//�����̰߳�ȫ��
		Math::Point m_leftPoint;				//��������������ʱ����λ�ã���λ���ڵ���һ��update()����������������λ��
		Math::Point m_leftPointMovement;		//������������ʱ��¼�ƶ�����ĸ���λ��
		Math::Point m_rightPoint;				//��������Ҽ�����ʱ����λ�ã���λ���ڵ���һ��update()����������������λ��
		Math::Point m_rightPointMovement;		//������Ҽ�����ʱ��¼�ƶ�����ĸ���λ��
		SHORT m_mouseWheel;						//��¼��껬�ֹ���delta

	public:
		//����任��Ӧ���¼�
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

		//����뿪�ͻ���
		void MouseLeave()
		{
			m_isLButtonDown = false;
			m_isRButtonDown = false;
		}
	};
}


