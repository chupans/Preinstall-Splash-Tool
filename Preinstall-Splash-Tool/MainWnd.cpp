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

CMainWnd::CMainWnd()
	: config("Docs\\configuration.xml")
{
	CRect rect;
	DWORD style = WS_CAPTION | WS_SYSMENU | WS_DLGFRAME;
	if (config.resize_by_content)
	{
		rect = GetCenterWndRect(config.wnd_width, config.wnd_height);
	}
	else
	{
		rect = GetCenterWndRect(config.min_width, config.min_height);
		style |= WS_VSCROLL | WS_HSCROLL;
	}

	Create(NULL, config.GetCaption(), style, rect);
	
	for (UINT i = 0; i < config.items.size(); i++)
	{ 
		CRect r = config.GetButtonRect(i);
		CButton* b = new CButton();
		b->Create(L"", WS_CHILD | BS_BITMAP, r, this, i + 1);
		b->SetBitmap(config.button);
		b->ShowWindow(SW_SHOW);
		buttons.push_back(b);
	}

	if (!config.resize_by_content)
	{
		SCROLLINFO si;
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		si.nPage = 1;
		si.nMin = 0;
		si.nMax = 10;
		si.nPos = si.nTrackPos = 0;
		SetScrollInfo(SB_VERT, &si); 
		SetScrollInfo(SB_HORZ, &si);
	}
}

CMainWnd::~CMainWnd()
{
	for (std::vector<CButton*>::iterator i = buttons.begin(); i != buttons.end(); i++)
	{
		delete (*i);
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
	config.background.StretchBlt(dc, client_rect);
	
	dc.SetBkMode(TRANSPARENT);

	CPoint p = config.GetHeaderPosition();
	config.header.Draw(dc, p.x, p.y);

	p = config.GetSubHeaderPosition();
	config.sub_header.Draw(dc, p.x, p.y);

	for (UINT i = 0; i < config.items.size(); i++)
	{
		CPoint pos = config.GetButtonTextPosition(i);
		config.items[i].description.Draw(dc, pos.x, pos.y);
	}
}

afx_msg void CMainWnd::OnButtonClick(UINT id)
{
	CXMLConfig::Item item = config.items[id - 1];
	ShellExecute((HWND)this, L"open", item.path, NULL, NULL, SW_SHOW);
	if (item.close) this->DestroyWindow();
}

afx_msg void CMainWnd::OnVScroll(UINT SBCode, UINT Pos, CScrollBar *SB)
{
	
}

afx_msg void CMainWnd::OnHScroll(UINT SBCode, UINT Pos, CScrollBar *SB)
{

}

BEGIN_MESSAGE_MAP(CMainWnd, CFrameWnd)
	ON_WM_PAINT()
	ON_COMMAND_RANGE(0, (UINT)(-1), &CMainWnd::OnButtonClick)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()