#pragma once

static const int MaxLengthWinTitle = 100;

namespace App
{
	class COMMON_API WindowCreator
	{
	private:
		HINSTANCE m_hInstance;                                // ��ǰʵ��
		int m_nCmdShow;
		WCHAR m_szTitle[MaxLengthWinTitle];                  // �������ı�
		WCHAR m_szWindowClass[MaxLengthWinTitle];            // ����������

		HWND hWnd;

	public:
		WindowCreator(HINSTANCE hInstance, int nCmdShow);

		~WindowCreator();

		//ע�ᴰ����
		ATOM                MyRegisterClass(WNDPROC WndProc);
		BOOL                InitInstance();

		void CreateWindowSizeDependentResources();

		//��ô���ָ��
		HWND GetHwnd()const { return hWnd; }
	};
}


