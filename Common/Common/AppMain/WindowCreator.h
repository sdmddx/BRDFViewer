#pragma once

static const int MaxLengthWinTitle = 100;

namespace App
{
	class COMMON_API WindowCreator
	{
	private:
		HINSTANCE m_hInstance;                                // 当前实例
		int m_nCmdShow;
		WCHAR m_szTitle[MaxLengthWinTitle];                  // 标题栏文本
		WCHAR m_szWindowClass[MaxLengthWinTitle];            // 主窗口类名

		HWND hWnd;

	public:
		WindowCreator(HINSTANCE hInstance, int nCmdShow);

		~WindowCreator();

		//注册窗口类
		ATOM                MyRegisterClass(WNDPROC WndProc);
		BOOL                InitInstance();

		void CreateWindowSizeDependentResources();

		//获得窗口指针
		HWND GetHwnd()const { return hWnd; }
	};
}


