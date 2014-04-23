#include "Application.h"
#include "MainWnd.h"

BOOL CApp::InitInstance()
{
	CMainWnd* wnd = new CMainWnd();
	this->m_pMainWnd = wnd;
	wnd->Show();
	return TRUE;
}

CApp application;