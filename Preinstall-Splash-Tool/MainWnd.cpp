#include "MainWnd.h"

CRect CMainWnd::GetCenterWndRect(int width, int height)
{
	CRect r;
	::GetWindowRect(::GetDesktopWindow(), &r);
	int screen_width = r.right - r.left;
	int screen_height = r.bottom - r.top;
	r.left += (screen_width - width) / 2;
	r.top += (screen_height - height) / 2;
	r.right = r.left + width;
	r.bottom = r.top + height;
	return r;
}

CRect CMainWnd::GetButtonRect(int x0, int y0, int btn_width, int btn_height, int delta, int index)
{
	CRect r;
	r.top = y0 + (btn_height + delta) * index;
	r.left = x0;
	r.right = r.left + btn_width;
	r.bottom = r.top + btn_height;
	return r;
}

CMainWnd::CMainWnd()
{
	config = CXMLConfig("Docs\\configuration.xml");
	background.Load(config.background_file_name);
	Create(NULL, config.caption, WS_CAPTION | WS_SYSMENU | WS_DLGFRAME, GetCenterWndRect(config.min_width, config.min_height));
	
	for (UINT i = 0; i < config.items.size(); i++)
	{ 
		CRect r = GetButtonRect(10, 10, 30, 30, 5, i);
		CButton* b = new CButton();
		b->Create(L"", WS_CHILD, r, this, i + 1);
		b->ShowWindow(SW_SHOW);
	}
}

void CMainWnd::Show()
{
	ShowWindow(SW_SHOW);
	UpdateWindow();
}

afx_msg void CMainWnd::OnPaint()
{
	CPaintDC dc(this);
	CRect client_rect;
	GetClientRect(&client_rect);
	if (!background.IsNull()) background.StretchBlt(dc, client_rect);

}

afx_msg void CMainWnd::OnButtonClick(UINT id)
{
	CXMLConfig::Item item = config.items[id - 1];
	ShellExecute((HWND)this, L"open", item.path, NULL, NULL, SW_SHOW);
	if (item.close) this->DestroyWindow();
}

BEGIN_MESSAGE_MAP(CMainWnd, CFrameWnd)
	ON_WM_PAINT()
	ON_COMMAND_RANGE(0, (UINT)(-1), &CMainWnd::OnButtonClick)
END_MESSAGE_MAP()