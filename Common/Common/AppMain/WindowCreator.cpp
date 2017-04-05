#include "stdafx.h"

using namespace App;

WindowCreator::WindowCreator(HINSTANCE hInstance, int nCmdShow)
{
	m_hInstance = hInstance;
	m_nCmdShow = nCmdShow;
	LoadStringW(m_hInstance, IDS_APP_TITLE, m_szTitle, MaxLengthWinTitle);
	LoadStringW(m_hInstance, IDC_BRDFVIEWER, m_szWindowClass, MaxLengthWinTitle);
}

WindowCreator::~WindowCreator()
{

}
//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
ATOM WindowCreator::MyRegisterClass(WNDPROC WndProc)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_hInstance;
	wcex.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_BRDFVIEWER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_BRDFVIEWER);
	wcex.lpszClassName = m_szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL WindowCreator::InitInstance()
{
	hWnd = CreateWindowW(m_szWindowClass, m_szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, m_hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, m_nCmdShow);
	UpdateWindow(hWnd);



	return TRUE;
}

void WindowCreator::CreateWindowSizeDependentResources()
{

}
