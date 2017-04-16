// BRDFViewer.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "commctrl.h"

#include "BRDFViewer.h"

using namespace std;
shared_ptr<Example::Origin_PBR_ML_Scene> sphereScene;
int ncmdShow;
// ȫ�ֱ���: 
HINSTANCE hInst;    

HWND hdlg;
// ��ǰʵ��

// �˴���ģ���а����ĺ�����ǰ������: 
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About0(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
	ncmdShow = nCmdShow;
    // TODO: �ڴ˷��ô��롣
	using namespace std;

	shared_ptr<App::WindowCreator> window = shared_ptr<App::WindowCreator>(new App::WindowCreator(hInstance, nCmdShow));

    // ��ʼ��ȫ���ַ���
	window->MyRegisterClass(WndProc);

    // ִ��Ӧ�ó����ʼ��: 
    if (!window->InitInstance())
    {
        return FALSE;
    }

	//�����豸��Դ
	shared_ptr<DX::DXResources> dxResources = make_shared<DX::DXResources>();
	dxResources->SetOutputWindow(window->GetHwnd());
	unique_ptr<Example::SphereAppMain> main = unique_ptr<Example::SphereAppMain>(new Example::SphereAppMain(dxResources));

	//����һ������
	sphereScene = shared_ptr<Example::Origin_PBR_ML_Scene>(new Example::Origin_PBR_ML_Scene(dxResources));

	main->GetScene(sphereScene);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BRDFVIEWER));
	


    MSG msg;

    // ����Ϣѭ��: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
			
        }
		main->StartRenderLoop();
		//ShowWindow(hButton, nCmdShow);
		//UpdateWindow(hButton);
    }

    return (int) msg.wParam;
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND  - ����Ӧ�ó���˵�
//  WM_PAINT    - ����������
//  WM_DESTROY  - �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // �����˵�ѡ��: 
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			case ID_32772:
			{
				
				HWND h= CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, About0);
				ShowWindow(h, SW_SHOWNA);
			}
			break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

	case WM_CREATE:
	{
		// �����Ի���  
		//HWND hDlg0 = CreateDialog(hInst, MAKEINTRESOURCE(IDD_FORMVIEW), hWnd, nullptr);
		// ��ʾ�Ի���  
		//ShowWindow(hDlg0, SW_SHOWNA);
	}
	
	break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: �ڴ˴����ʹ�� hdc ���κλ�ͼ����...
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

	case WM_LBUTTONDOWN:
		{
			POINT point;
			GetCursorPos(&point);
			sphereScene->OnLButtonDown(lParam);
		 
		}
		break;

	case WM_LBUTTONUP:
		{
			sphereScene->OnLButtonUp(lParam);
		}
		break;

	case WM_RBUTTONDOWN:
		{
			sphereScene->OnRButtonDown(lParam);
		}
		break;

	case WM_RBUTTONUP:
		{
			sphereScene->OnRButtonUp(lParam);
		}
		break;

	case WM_MOUSEMOVE:
		{
			sphereScene->MouseMove(lParam);
		}
		break;

	case WM_MOUSEWHEEL:
		{
		
			sphereScene->MouseWheel(HIWORD(wParam));
		
		}
		break;

	case WM_NCMOUSEMOVE:
		{
			sphereScene->MouseLeave();
		}
		break;

	case WM_SIZE:
		{

		}
		break;

	case WM_MBUTTONDOWN:
		{

		}
		break;
	
	case WM_MBUTTONUP:
		{

		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_PRIOR:
			

		case VK_NEXT:
			

		case VK_UP:
			

		case VK_DOWN:
			

		default:
			break;
		}
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About0(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
	{
		// ��ȡ��ǰʵ�����  
		HINSTANCE hinst = (HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE);
		
		//����DFGѡ���combo box
		HWND specularD = CreateWindow(L"combobox", L"specular D", WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWN | CBS_HASSTRINGS,
			30, 10, 120, 100, hDlg, (HMENU)IDC_COMBOD, hinst, NULL);
		SendMessage(specularD, CB_INSERTSTRING, 0, (LPARAM)TEXT("GGX/Trowbridge-Reitz"));
		SendMessage(specularD, CB_INSERTSTRING, 1, (LPARAM)TEXT("GTR(Generalized-Trowbridge-Reitz)"));
		SendMessage(specularD, CB_INSERTSTRING, 2, (LPARAM)TEXT("Bling-Phong"));
		SendMessage(specularD, CB_INSERTSTRING, 3, (LPARAM)TEXT("UE�е�Bling-Phong"));
		SendMessage(specularD, CB_INSERTSTRING, 4, (LPARAM)TEXT("Beckmann"));
		SendMessage(specularD, CB_INSERTSTRING, 5, (LPARAM)TEXT("Phong"));
		SendMessage(specularD, CB_INSERTSTRING, 6, (LPARAM)TEXT("��������GGX"));
		SendMessage(specularD, CB_SETCURSEL, 0, 0);

		HWND specularF = CreateWindow(L"combobox", L"specular F", WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWN | CBS_HASSTRINGS,
			180, 10, 120, 100, hDlg, (HMENU)IDC_COMBOF, hinst, NULL);
		SendMessage(specularF, CB_INSERTSTRING, 0, (LPARAM)TEXT("Schlick"));
		SendMessage(specularF, CB_INSERTSTRING, 1, (LPARAM)TEXT("Schlick_SG"));
		SendMessage(specularF, CB_INSERTSTRING, 2, (LPARAM)TEXT("Cook-Torrance"));
		SendMessage(specularF, CB_SETCURSEL, 0, 0);

		HWND specularG = CreateWindow(L"combobox", L"specular G", WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWN | CBS_HASSTRINGS,
			330, 10, 120, 100, hDlg, (HMENU)IDC_COMBOG, hinst, NULL);
		SendMessage(specularG, CB_INSERTSTRING, 0, (LPARAM)TEXT("Schlick"));
		SendMessage(specularG, CB_INSERTSTRING, 1, (LPARAM)TEXT("Neumann"));
		SendMessage(specularG, CB_INSERTSTRING, 2, (LPARAM)TEXT("Cook-Torrance"));
		SendMessage(specularG, CB_INSERTSTRING, 3, (LPARAM)TEXT("Kelemen"));
		SendMessage(specularG, CB_INSERTSTRING, 4, (LPARAM)TEXT("����Schlick"));
		SendMessage(specularG, CB_INSERTSTRING, 5, (LPARAM)TEXT("Schlick-Beckmann"));
		SendMessage(specularG, CB_INSERTSTRING, 6, (LPARAM)TEXT("Schlick-GGX"));
		SendMessage(specularG, CB_INSERTSTRING, 7, (LPARAM)TEXT("UE�е�Schlick"));
		SendMessage(specularG, CB_INSERTSTRING, 8, (LPARAM)TEXT("Beckmann")); 
		SendMessage(specularG, CB_INSERTSTRING, 9, (LPARAM)TEXT("GGX"));
		SendMessage(specularG, CB_INSERTSTRING, 10, (LPARAM)TEXT("SmithJointGGX"));
		SendMessage(specularG, CB_INSERTSTRING, 11, (LPARAM)TEXT("SmithJointGG�Ż���"));
		SendMessage(specularG, CB_SETCURSEL, 0, 0);

		//�����������ʵ�slider
		SendMessage(GetDlgItem(hDlg, IDC_DIFFUSE_X), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		SendMessage(GetDlgItem(hDlg, IDC_DIFFUSE_X), TBM_SETPOS, TRUE, 0);
		SendMessage(GetDlgItem(hDlg, IDC_DIFFUSE_Y), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		SendMessage(GetDlgItem(hDlg, IDC_DIFFUSE_Y), TBM_SETPOS, TRUE, 0);
		SendMessage(GetDlgItem(hDlg, IDC_DIFFUSE_Z), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		SendMessage(GetDlgItem(hDlg, IDC_DIFFUSE_Z), TBM_SETPOS, TRUE, 0);

		//���þ��淴���ʵ�slider
		SendMessage(GetDlgItem(hDlg, IDC_SPECULAR_X), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		SendMessage(GetDlgItem(hDlg, IDC_SPECULAR_X), TBM_SETPOS, TRUE, 0);
		SendMessage(GetDlgItem(hDlg, IDC_SPECULAR_Y), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		SendMessage(GetDlgItem(hDlg, IDC_SPECULAR_Y), TBM_SETPOS, TRUE, 0);
		SendMessage(GetDlgItem(hDlg, IDC_SPECULAR_Z), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		SendMessage(GetDlgItem(hDlg, IDC_SPECULAR_Z), TBM_SETPOS, TRUE, 0);

		//����basecolor��Slider
		SendMessage(GetDlgItem(hDlg, IDC_BASECOLOR_X), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		SendMessage(GetDlgItem(hDlg, IDC_BASECOLOR_X), TBM_SETPOS, TRUE, 0);
		SendMessage(GetDlgItem(hDlg, IDC_BASECOLOR_Y), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		SendMessage(GetDlgItem(hDlg, IDC_BASECOLOR_Y), TBM_SETPOS, TRUE, 0);
		SendMessage(GetDlgItem(hDlg, IDC_BASECOLOR_Z), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		SendMessage(GetDlgItem(hDlg, IDC_BASECOLOR_Z), TBM_SETPOS, TRUE, 0);

		//����PBR��ص�Slider
		SendMessage(GetDlgItem(hDlg, IDC_ROUGHNESS), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		SendMessage(GetDlgItem(hDlg, IDC_ROUGHNESS), TBM_SETPOS, TRUE, 0);
		SendMessage(GetDlgItem(hDlg, IDC_METALLIC), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		SendMessage(GetDlgItem(hDlg, IDC_METALLIC), TBM_SETPOS, TRUE, 0);
		SendMessage(GetDlgItem(hDlg, IDC_SPECULAR), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		SendMessage(GetDlgItem(hDlg, IDC_SPECULAR), TBM_SETPOS, TRUE, 0);
		SendMessage(GetDlgItem(hDlg, IDC_SPECULARTINT), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		SendMessage(GetDlgItem(hDlg, IDC_SPECULARTINT), TBM_SETPOS, TRUE, 0);
		SendMessage(GetDlgItem(hDlg, IDC_CLEARCOAT), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		SendMessage(GetDlgItem(hDlg, IDC_CLEARCOAT), TBM_SETPOS, TRUE, 0);
		SendMessage(GetDlgItem(hDlg, IDC_CLEARCOATGLOSS), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		SendMessage(GetDlgItem(hDlg, IDC_CLEARCOATGLOSS), TBM_SETPOS, TRUE, 0);
		SendMessage(GetDlgItem(hDlg, IDC_ANISOTROPIC), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		SendMessage(GetDlgItem(hDlg, IDC_ANISOTROPIC), TBM_SETPOS, TRUE, 0);
		SendMessage(GetDlgItem(hDlg, IDC_DGTRC), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		SendMessage(GetDlgItem(hDlg, IDC_DGTRC), TBM_SETPOS, TRUE, 0);
		SendMessage(GetDlgItem(hDlg, IDC_DGTRN), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		SendMessage(GetDlgItem(hDlg, IDC_DGTRN), TBM_SETPOS, TRUE, 0);

		//��̬�ı�
		CreateWindow(TEXT("STATIC"), TEXT("D:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
			10, 10, 15, 15, hDlg, nullptr, hinst, NULL);
		CreateWindow(TEXT("STATIC"), TEXT("F:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
			160, 10, 15, 15, hDlg, nullptr, hinst, NULL);
		CreateWindow(TEXT("STATIC"), TEXT("G:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
			310, 10, 15, 15, hDlg, nullptr, hinst, NULL);

		HWND hwnd_di = CreateWindow(TEXT("STATIC"), TEXT("diffuseǿ��"), WS_CHILD | WS_VISIBLE | SS_LEFT, 
			10, 40, 85, 15, hDlg, (HMENU)IDC_DIFFUSEINTENSITY, hinst, NULL);
		SendMessage(hwnd_di, WM_SETFONT, (WPARAM)GetStockObject(17), 0);
		HWND hwnd_si = CreateWindow(TEXT("STATIC"), TEXT("specularǿ��"), WS_CHILD | WS_VISIBLE | SS_LEFT, 
			10, 180,90, 15, hDlg, (HMENU)IDC_SPECULARINTENSITY, hinst, NULL);
		SendMessage(hwnd_si, WM_SETFONT, (WPARAM)GetStockObject(17), 0);
		HWND hwnd_baseColor = CreateWindow(TEXT("STATIC"), TEXT("baseColor"), WS_CHILD | WS_VISIBLE | SS_LEFT,
			10, 320, 90, 15, hDlg, (HMENU)IDC_SPECULARINTENSITY, hinst, NULL);
		SendMessage(hwnd_baseColor, WM_SETFONT, (WPARAM)GetStockObject(17), 0);
		UINT posX = 172;
		UINT posFirstY = 70;
		UINT distanceY = 33;
		CreateWindow(TEXT("STATIC"), TEXT("roughness"), WS_CHILD | WS_VISIBLE | SS_LEFT,
			posX, posFirstY, 80, 15, hDlg,nullptr, hinst, NULL);
		CreateWindow(TEXT("STATIC"), TEXT("metallic"), WS_CHILD | WS_VISIBLE | SS_LEFT,
			posX, posFirstY += distanceY, 80, 15, hDlg, nullptr, hinst, NULL);
		CreateWindow(TEXT("STATIC"), TEXT("specular"), WS_CHILD | WS_VISIBLE | SS_LEFT,
			posX, posFirstY += distanceY, 80, 15, hDlg, nullptr, hinst, NULL);
		CreateWindow(TEXT("STATIC"), TEXT("specularTint"), WS_CHILD | WS_VISIBLE | SS_LEFT,
			posX, posFirstY += distanceY, 80, 15, hDlg, nullptr, hinst, NULL);
		CreateWindow(TEXT("STATIC"), TEXT("clearCoat"), WS_CHILD | WS_VISIBLE | SS_LEFT,
			posX, posFirstY += distanceY, 80, 15, hDlg, nullptr, hinst, NULL);
		CreateWindow(TEXT("STATIC"), TEXT("clearCoatGloss"), WS_CHILD | WS_VISIBLE | SS_LEFT,
			posX, posFirstY += distanceY, 100, 15, hDlg, nullptr, hinst, NULL);
		CreateWindow(TEXT("STATIC"), TEXT("anisotropic"), WS_CHILD | WS_VISIBLE | SS_LEFT,
			posX, posFirstY += distanceY, 80, 15, hDlg, nullptr, hinst, NULL);
		CreateWindow(TEXT("STATIC"), TEXT("D_GTR_C"), WS_CHILD | WS_VISIBLE | SS_LEFT,
			posX, posFirstY += distanceY, 80, 15, hDlg, nullptr, hinst, NULL);
		CreateWindow(TEXT("STATIC"), TEXT("D_GTR_N"), WS_CHILD | WS_VISIBLE | SS_LEFT,
			posX, posFirstY += distanceY, 80, 15, hDlg, nullptr, hinst, NULL);
		/*
		HWND edit1 = CreateWindow(TEXT("Edit"), TEXT("Edit Control"), WS_CHILD | WS_VISIBLE | ES_LEFT, 
			200, 260, 100, 20, hDlg, (HMENU)IDC_EDIT1, hinst, NULL);
		SendMessage(edit1, WM_SETFONT, (WPARAM)GetStockObject(17), 0);

		// ��ȡ��ǰʵ�����  
		HINSTANCE hthisapp = (HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE);
		CreateWindow(L"SCROLLBAR", L"��������Ա��ǣ�",
			 WS_CHILD | WS_VISIBLE,
			10, 10, 160, 18,
			hDlg, (HMENU)IDC_SLIDER1,
			hthisapp,
			&lParam);
		CreateWindow(L"Button", L"��",
			WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
			12, 12, 60, 16,
			hDlg,
			(HMENU)IDC_SLIDER1,
			hthisapp, NULL);
		*/

	}
		return (INT_PTR)TRUE;
	case WM_HSCROLL:
	{
		sphereScene->ProcessHSCROLL(wParam, lParam);

	}
	return (INT_PTR)TRUE;

	case WM_COMMAND:
	{
		sphereScene->ProcessCommand(wParam, lParam);
		

	}		
	break;

	case WM_SYSCOMMAND:
		if (wParam == SC_CLOSE)
		{
			// ���ִ���˹ر�  
			// ���ٶԻ��򣬽��յ�WM_DESTROY��Ϣ  
			EndDialog(hDlg, LOWORD(wParam));
		}
		break;
	case WM_CREATE:
		
		break;
	}
	return (INT_PTR)FALSE;
}
