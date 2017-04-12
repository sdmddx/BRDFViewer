#pragma once

#include "CommCtrl.h"
#include "Common\Common\Common.h"
#include "resource.h"
namespace Example
{
	class Origin_PBR_ML_Scene :public Scene::SceneBase
	{
	public:
		Origin_PBR_ML_Scene(const std::shared_ptr<DX::DXResources>& dxResources);

		~Origin_PBR_ML_Scene();

		virtual void CreateDeviceDependentResources();
		virtual void CreateWindowSizeDependentResources();
		virtual void ReleaseDeviceDependentResources();
		virtual void Update(Utilities::StepTimer const& timer);
		virtual void Render();

	private:

		// ������豸��Դָ�롣
		std::shared_ptr<DX::DXResources> m_dxResources;

		// ���弸�ε� Direct3D ��Դ��
		Microsoft::WRL::ComPtr<ID3D11InputLayout>				m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>					m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>					m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>				m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>				m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>					m_D3D_CB_WVP;
		Microsoft::WRL::ComPtr<ID3D11Buffer>					m_D3D_CB_PBR;
		Microsoft::WRL::ComPtr<ID3D11Buffer>					m_D3D_CB_WVP_Inverse;
		Microsoft::WRL::ComPtr<ID3D11Buffer>					m_D3D_CB_MatInfo;
		Microsoft::WRL::ComPtr<ID3D11Buffer>					m_D3D_CB_LightsInfo;
		Microsoft::WRL::ComPtr<ID3D11Buffer>					m_D3D_CB_DFGChoice;

		//��Դ��ϢSRV
		ID3D11ShaderResourceView*								m_SRV_LightInfo;

		unique_ptr<Utilities::SdmCamera>						m_camera;

		unique_ptr<Utilities::ShaderLoader>						m_shaderLoader;

		UINT32	m_indexCount;
		UINT numLights;

		Utilities::DXBufferCreator*								m_dxBufferCreator;

		//����������
		unique_ptr<CB_WVP>										m_CB_WVP_Data;
		unique_ptr<CB_PBR>										m_CB_PBR_Data;
		unique_ptr<CB_MatInfo>									m_CB_MatInfo_Data;
		unique_ptr<CB_WVP_Inverse>								m_CB_WVP_Inverse_Data;
		unique_ptr<LightsInfo>									m_CB_LightsInfo_Data;
		unique_ptr<CB_DFGChoice>								m_CB_DFGChoice_Data;

		//����任����������

		bool m_loadingComplete;

		BOOL m_isLButtonDown;
		BOOL m_isRButtonDown;

		//�����̰߳�ȫ��
		Math::Point m_leftPoint;				//��������������ʱ����λ�ã���λ���ڵ���һ��update()����������������λ��
		Math::Point m_leftPointMovement;		//������������ʱ��¼�ƶ�����ĸ���λ��
		Math::Point m_rightPoint;				//��������Ҽ�����ʱ����λ�ã���λ���ڵ���һ��update()����������������λ��
		Math::Point m_rightPointMovement;		//������Ҽ�����ʱ��¼�ƶ�����ĸ���λ��
		SHORT m_mouseWheel;			//��¼��껬�ֹ���delta

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

		void SetRoughness(float value)
		{
			m_CB_PBR_Data->roughness = value;
		}

		void SetMetallic(float value)
		{
			m_CB_PBR_Data->metalic = value;
		}

		void SetSpecular(float value)
		{
			m_CB_PBR_Data->specular = value/12.5f;					//ӳ�䵽0-0.08f
		}
		void SetSpecularTint(float value)
		{
			m_CB_PBR_Data->specularTint = value;
		}
		void SetClearCoat(float value)
		{
			m_CB_PBR_Data->clearCoat = value/4.0f;					//ӳ�䵽0-0.25f
		}
		void SetClearCoatGloss(float value)
		{
			m_CB_PBR_Data->clearCoatGloss = value;
		}
		void SetAnisotropic(float value)
		{
			m_CB_PBR_Data->anisotropic = value;
		}
		void SetDGTRC(float value)
		{
			m_CB_PBR_Data->D_GTR_C = value/2.0f;					//ӳ�䵽0-0.5f
		}
		void SetDGTRN(float value)
		{
			m_CB_PBR_Data->D_GTR_N = 9.0f*value+1.0f;				//ӳ�䵽1.0f-10.0f
		}

		enum class Axies
		{
			X, Y, Z
		};

		void SetDiffuseIntensity(float value, Axies a)
		{
			switch (a)
			{
			case Axies::X:
				m_CB_MatInfo_Data->diffuse.x = value;
				break;
			case Axies::Y:
				m_CB_MatInfo_Data->diffuse.y = value;
				break;
			case Axies::Z:
				m_CB_MatInfo_Data->diffuse.z = value;
				break;
			default:
				break;
			}
		}

		void SetSpecularIntensity(float value, Axies a)
		{
			switch (a)
			{
			case Axies::X:
				m_CB_MatInfo_Data->specular.x = value;
				break;
			case Axies::Y:
				m_CB_MatInfo_Data->specular.y = value;
				break;
			case Axies::Z:
				m_CB_MatInfo_Data->specular.z = value;
				break;
			default:
				break;
			}
		}

		void SetBaseColor(float value, Axies a)
		{
			switch (a)
			{
			case Axies::X:
				m_CB_PBR_Data->baseColor.x = value;
				break;
			case Axies::Y:
				m_CB_PBR_Data->baseColor.y = value;
				break;
			case Axies::Z:
				m_CB_PBR_Data->baseColor.z = value;
				break;
			default:
				break;
			}
		}

		virtual void ProcessHSCROLL(WPARAM wParam, LPARAM lParam)
		{
			int nScrollCode = (int)LOWORD(wParam);  // scroll bar value  
			int ScrollCode = (int)HIWORD(wParam);  // scroll bar value  

			HWND hwndScrollBar = (HWND)lParam;
			UINT id =  GetDlgCtrlID(hwndScrollBar);
			switch (nScrollCode)
			{
			case SB_THUMBTRACK:   //�û��϶������򡣴���Ϣ���ظ����ͣ�ֱ���û��ͷ���갴ť��
			{
				int curPos = SendMessage(hwndScrollBar, TBM_GETPOS, 0, 0);
				//SetRoughness(float(curPos) / 255.0f);
				switch (id)
				{
					
				case IDC_DIFFUSE_X:
					SetDiffuseIntensity(float(curPos) / 255.0f, Axies::X); break;
				case IDC_DIFFUSE_Y:
					SetDiffuseIntensity(float(curPos) / 255.0f, Axies::Y); break;
				case IDC_DIFFUSE_Z:
					SetDiffuseIntensity(float(curPos) / 255.0f, Axies::Z); break;
				case IDC_SPECULAR_X:
					SetSpecularIntensity(float(curPos) / 255.0f, Axies::X); break;
				case IDC_SPECULAR_Y:
					SetSpecularIntensity(float(curPos) / 255.0f, Axies::Y); break;
				case IDC_SPECULAR_Z:
					SetSpecularIntensity(float(curPos) / 255.0f, Axies::Z); break;
				case IDC_BASECOLOR_X:
					SetBaseColor(float(curPos) / 255.0f, Axies::X); break;
				case IDC_BASECOLOR_Y:
					SetBaseColor(float(curPos) / 255.0f, Axies::Y); break;
				case IDC_BASECOLOR_Z:
					SetBaseColor(float(curPos) / 255.0f, Axies::Z); break;
				case IDC_ROUGHNESS:
					SetRoughness(float(curPos) / 255.0f); break;
				case IDC_METALLIC:
					SetMetallic(float(curPos) / 255.0f); break;
				case IDC_SPECULAR:
					SetSpecular(float(curPos) / 255.0f); break;
				case IDC_SPECULARTINT:
					SetSpecularTint(float(curPos) / 255.0f); break;
				case IDC_CLEARCOAT:
					SetClearCoat(float(curPos) / 255.0f); break;
				case IDC_CLEARCOATGLOSS:
					SetClearCoatGloss(float(curPos) / 255.0f); break;
				case IDC_ANISOTROPIC:
					SetAnisotropic(float(curPos) / 255.0f); break;
				case IDC_DGTRC:
					SetDGTRC(float(curPos) / 255.0f); break;
				case IDC_DGTRN:
					SetDGTRN(float(curPos) / 255.0f); break;
				default:
					break;
				}
			}

			break;



			}
		}

		void SetD(UINT index)
		{
			m_CB_DFGChoice_Data->Dchoice = index;
		}

		void SetF(UINT index)
		{
			m_CB_DFGChoice_Data->Fchoice = index;
		}

		void SetG(UINT index)
		{
			m_CB_DFGChoice_Data->Gchoice = index;
		}

		virtual void ProcessCommand(WPARAM wParam, LPARAM lParam)
		{
			UINT uSender = LOWORD(wParam);
			int event = HIWORD(wParam);
			HWND hwnd = (HWND)lParam;
			UINT id = GetDlgCtrlID(hwnd);
			if (event == CBN_SELCHANGE)
			{
				UINT index = SendMessage(hwnd, CB_GETCURSEL, 0, 0);
				switch (id)
				{
				case IDC_COMBOD:
					SetD(index); break;

				case IDC_COMBOF:
					SetF(index); break;

				case IDC_COMBOG:
					SetG(index); break;

				default:
					break;
				}
			}
		}
	};
}


