// BRDFViewer.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"

#include "commctrl.h"

#include "BRDFViewer.h"

using namespace std;
shared_ptr<Example::Origin_PBR_ML_Scene> sphereScene;
int ncmdShow;
// 全局变量: 
HINSTANCE hInst;    

HWND hdlg;
// 当前实例

// 此代码模块中包含的函数的前向声明: 
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
    // TODO: 在此放置代码。
	using namespace std;

	shared_ptr<App::WindowCreator> window = shared_ptr<App::WindowCreator>(new App::WindowCreator(hInstance, nCmdShow));

    // 初始化全局字符串
	window->MyRegisterClass(WndProc);

    // 执行应用程序初始化: 
    if (!window->InitInstance())
    {
        return FALSE;
    }

	//创建设备资源
	shared_ptr<DX::DXResources> dxResources = make_shared<DX::DXResources>();
	dxResources->SetOutputWindow(window->GetHwnd());
	unique_ptr<Example::SphereAppMain> main = unique_ptr<Example::SphereAppMain>(new Example::SphereAppMain(dxResources));

	//创建一个场景
	sphereScene = shared_ptr<Example::Origin_PBR_ML_Scene>(new Example::Origin_PBR_ML_Scene(dxResources));

	main->GetScene(sphereScene);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BRDFVIEWER));
	


    MSG msg;

    // 主消息循环: 
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
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择: 
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
		// 创建对话框  
		//HWND hDlg0 = CreateDialog(hInst, MAKEINTRESOURCE(IDD_FORMVIEW), hWnd, nullptr);
		// 显示对话框  
		//ShowWindow(hDlg0, SW_SHOWNA);
	}
	
	break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
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

// “关于”框的消息处理程序。
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

// “关于”框的消息处理程序。
INT_PTR CALLBACK About0(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
	{
		// 获取当前实例句柄  
		HINSTANCE hinst = (HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE);
		
		//创建DFG选择的combo box
		HWND specularD = CreateWindow(L"combobox", L"specular D", WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWN | CBS_HASSTRINGS,
			30, 10, 120, 100, hDlg, (HMENU)IDC_COMBOD, hinst, NULL);
		SendMessage(specularD, CB_INSERTSTRING, 0, (LPARAM)TEXT("GGX/Trowbridge-Reitz"));
		SendMessage(specularD, CB_INSERTSTRING, 1, (LPARAM)TEXT("GTR(Generalized-Trowbridge-Reitz)"));
		SendMessage(specularD, CB_INSERTSTRING, 2, (LPARAM)TEXT("Bling-Phong"));
		SendMessage(specularD, CB_INSERTSTRING, 3, (LPARAM)TEXT("UE中的Bling-Phong"));
		SendMessage(specularD, CB_INSERTSTRING, 4, (LPARAM)TEXT("Beckmann"));
		SendMessage(specularD, CB_INSERTSTRING, 5, (LPARAM)TEXT("Phong"));
		SendMessage(specularD, CB_INSERTSTRING, 6, (LPARAM)TEXT("各向异性GGX"));
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
		SendMessage(specularG, CB_INSERTSTRING, 4, (LPARAM)TEXT("经典Schlick"));
		SendMessage(specularG, CB_INSERTSTRING, 5, (LPARAM)TEXT("Schlick-Beckmann"));
		SendMessage(specularG, CB_INSERTSTRING, 6, (LPARAM)TEXT("Schlick-GGX"));
		SendMessage(specularG, CB_INSERTSTRING, 7, (LPARAM)TEXT("UE中的Schlick"));
		SendMessage(specularG, CB_INSERTSTRING, 8, (LPARAM)TEXT("Beckmann")); 
		SendMessage(specularG, CB_INSERTSTRING, 9, (LPARAM)TEXT("GGX"));
		SendMessage(specularG, CB_INSERTSTRING, 10, (LPARAM)TEXT("SmithJointGGX"));
		SendMessage(specularG, CB_INSERTSTRING, 11, (LPARAM)TEXT("SmithJointGG优化版"));
		SendMessage(specularG, CB_SETCURSEL, 0, 0);

		//设置漫反射率的slider
		SendMessage(GetDlgItem(hDlg, IDC_DIFFUSE_X), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		SendMessage(GetDlgItem(hDlg, IDC_DIFFUSE_X), TBM_SETPOS, TRUE, 0);
		SendMessage(GetDlgItem(hDlg, IDC_DIFFUSE_Y), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		SendMessage(GetDlgItem(hDlg, IDC_DIFFUSE_Y), TBM_SETPOS, TRUE, 0);
		SendMessage(GetDlgItem(hDlg, IDC_DIFFUSE_Z), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		SendMessage(GetDlgItem(hDlg, IDC_DIFFUSE_Z), TBM_SETPOS, TRUE, 0);

		//设置镜面反射率的slider
		SendMessage(GetDlgItem(hDlg, IDC_SPECULAR_X), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		SendMessage(GetDlgItem(hDlg, IDC_SPECULAR_X), TBM_SETPOS, TRUE, 0);
		SendMessage(GetDlgItem(hDlg, IDC_SPECULAR_Y), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		SendMessage(GetDlgItem(hDlg, IDC_SPECULAR_Y), TBM_SETPOS, TRUE, 0);
		SendMessage(GetDlgItem(hDlg, IDC_SPECULAR_Z), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		SendMessage(GetDlgItem(hDlg, IDC_SPECULAR_Z), TBM_SETPOS, TRUE, 0);

		//设置basecolor的Slider
		SendMessage(GetDlgItem(hDlg, IDC_BASECOLOR_X), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		SendMessage(GetDlgItem(hDlg, IDC_BASECOLOR_X), TBM_SETPOS, TRUE, 0);
		SendMessage(GetDlgItem(hDlg, IDC_BASECOLOR_Y), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		SendMessage(GetDlgItem(hDlg, IDC_BASECOLOR_Y), TBM_SETPOS, TRUE, 0);
		SendMessage(GetDlgItem(hDlg, IDC_BASECOLOR_Z), TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		SendMessage(GetDlgItem(hDlg, IDC_BASECOLOR_Z), TBM_SETPOS, TRUE, 0);

		//设置PBR相关的Slider
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

		//静态文本
		CreateWindow(TEXT("STATIC"), TEXT("D:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
			10, 10, 15, 15, hDlg, nullptr, hinst, NULL);
		CreateWindow(TEXT("STATIC"), TEXT("F:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
			160, 10, 15, 15, hDlg, nullptr, hinst, NULL);
		CreateWindow(TEXT("STATIC"), TEXT("G:"), WS_CHILD | WS_VISIBLE | SS_LEFT,
			310, 10, 15, 15, hDlg, nullptr, hinst, NULL);

		HWND hwnd_di = CreateWindow(TEXT("STATIC"), TEXT("diffuse强度"), WS_CHILD | WS_VISIBLE | SS_LEFT, 
			10, 40, 85, 15, hDlg, (HMENU)IDC_DIFFUSEINTENSITY, hinst, NULL);
		SendMessage(hwnd_di, WM_SETFONT, (WPARAM)GetStockObject(17), 0);
		HWND hwnd_si = CreateWindow(TEXT("STATIC"), TEXT("specular强度"), WS_CHILD | WS_VISIBLE | SS_LEFT, 
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

		// 获取当前实例句柄  
		HINSTANCE hthisapp = (HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE);
		CreateWindow(L"SCROLLBAR", L"请问你的性别是：",
			 WS_CHILD | WS_VISIBLE,
			10, 10, 160, 18,
			hDlg, (HMENU)IDC_SLIDER1,
			hthisapp,
			&lParam);
		CreateWindow(L"Button", L"男",
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
			// 如果执行了关闭  
			// 销毁对话框，将收到WM_DESTROY消息  
			EndDialog(hDlg, LOWORD(wParam));
		}
		break;
	case WM_CREATE:
		
		break;
	}
	return (INT_PTR)FALSE;
}
