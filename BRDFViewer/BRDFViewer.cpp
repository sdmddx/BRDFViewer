// BRDFViewer.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"



#include "BRDFViewer.h"

shared_ptr<Example::BP_DS> sphereScene;

// 全局变量: 
HINSTANCE hInst;                                // 当前实例

// 此代码模块中包含的函数的前向声明: 
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

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
	sphereScene = shared_ptr<Example::BP_DS>(new Example::BP_DS(dxResources));

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
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
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
