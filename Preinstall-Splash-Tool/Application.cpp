#include "Application.h"
#include "MainWnd.h"

BOOL CApp::InitInstance()
{
	try
	{
		CMainWnd* wnd = new CMainWnd();
		this->m_pMainWnd = wnd;
		wnd->Show();
		return TRUE;
	}
	catch (const CString& exception)
	{
		MessageBox(NULL, exception, L"Error", MB_ICONERROR);
	}
	catch (...)
	{
		MessageBox(NULL, L"Unknown error", L"Error", MB_ICONERROR);
	}
	return FALSE;
}

CApp application;