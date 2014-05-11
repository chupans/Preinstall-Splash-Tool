#include "MainWnd.h"

CRect CMainWnd::GetCenterWndRect(int width, int height)
{
	width += 2 * (GetSystemMetrics(SM_CXEDGE) + GetSystemMetrics(SM_CXDLGFRAME));
	height += GetSystemMetrics(SM_CYCAPTION) + 2 * (GetSystemMetrics(SM_CYEDGE) + GetSystemMetrics(SM_CYDLGFRAME));
	CRect r;
	::GetWindowRect(::GetDesktopWindow(), &r);
	int screen_width = r.right - r.left;
	int screen_height = r.bottom - r.top;
	if (width > screen_width) width = screen_width;
	if (height > screen_height) height = screen_height;
	r.left += (screen_width - width) / 2;
	r.top += (screen_height - height) / 2;
	r.right = r.left + width;
	r.bottom = r.top + height;
	return r;
}

bool CMainWnd::IsURL(const CString& str)
{
	int i = str.Find(L"//", 0);
	if (i < 0 || i > str.GetLength()) return false; else return true;
}

CMainWnd::CMainWnd()
	: config("Docs\\configuration.xml")
{
	vpos = hpos = 0;
	CRect rect;
	DWORD style = WS_CAPTION | WS_SYSMENU | WS_DLGFRAME;
	if (config.resize_by_content)
	{
		rect = GetCenterWndRect(config.max_wnd_size.cx, config.max_wnd_size.cy);
	}
	else
	{
		rect = GetCenterWndRect(config.min_wnd_size.cx, config.min_wnd_size.cy);
		
		style |= WS_VSCROLL | WS_HSCROLL;
		vmax = config.max_wnd_size.cy - config.min_wnd_size.cy;
		hmax = config.max_wnd_size.cx - config.min_wnd_size.cx;
	}

	Create(NULL, config.GetCaption(), style, rect);
	
	if (!config.resize_by_content)
	{
		SCROLLINFO si;
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		si.nPage = 1;
		si.nMin = 0;
		si.nMax = vmax;
		si.nPos = si.nTrackPos = 0;
		SetScrollInfo(SB_VERT, &si);
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		si.nPage = 1;
		si.nMin = 0;
		si.nMax = hmax;
		si.nPos = si.nTrackPos = 0;
		SetScrollInfo(SB_HORZ, &si);
	}

	for (UINT i = 0; i < config.items.size(); i++)
	{ 
		CRect r = config.GetButtonRect(i);
		CButton* b = new CButton();
		b->Create(L"", WS_CHILD | BS_BITMAP, r, this, i + 1);
		wchar_t buf[MAX_PATH];
		if (!IsURL(config.items[i].path) && (int)FindExecutableW(config.items[i].path, NULL, buf) <= 32) 
		{
			b->EnableWindow(FALSE);
			config.items[i].description.color = RGB(127, 127, 127);
		}
		b->SetBitmap(*config.items[i].button);
		b->ShowWindow(SW_SHOW);
		buttons.push_back(b);
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
	CPoint p0(-hpos, -vpos);
	CPaintDC dc(this);

	CRect client_rect;
	GetClientRect(&client_rect);

	if (!config.resize_by_content)
	{
		int dx = config.min_wnd_size.cx - client_rect.Width();
		int dy = config.min_wnd_size.cy - client_rect.Height();
		client_rect.right = dx + config.max_wnd_size.cx;
		client_rect.bottom = dy + config.max_wnd_size.cy;
	}

	SetStretchBltMode(dc, COLORONCOLOR);
	config.background.StretchBlt(dc, p0 + client_rect);
	
	dc.SetBkMode(TRANSPARENT);

	config.header.Draw(dc, p0 + config.GetHeaderPosition());
	config.sub_header.Draw(dc, p0 + config.GetSubHeaderPosition());

	for (UINT i = 0; i < config.items.size(); i++)
	{
		config.items[i].description.Draw(dc, p0 + config.GetButtonTextPosition(i));
	}
}

afx_msg void CMainWnd::OnButtonClick(UINT id)
{
	CXMLConfig::Item item = config.items[id - 1];
	HINSTANCE h = ShellExecute((HWND)this, L"open", item.path, NULL, NULL, SW_SHOW);
	if ((int)h == SE_ERR_NOASSOC) 
	{
		MessageBox(L"There is no application associated with the given file name extension.", L"Error", MB_ICONERROR);
	}
	else if ((int)h == ERROR_FILE_NOT_FOUND || (int)h == ERROR_PATH_NOT_FOUND)
	{
		MessageBox(L"File not found.", L"Error", MB_ICONERROR);
	}
	else if ((int)h <= 32) MessageBox(L"Can not open this file or URL.", L"Error", MB_ICONERROR);
	if (item.close) this->DestroyWindow();
}

afx_msg void CMainWnd::OnVScroll(UINT SBCode, UINT Pos, CScrollBar *SB)
{
	int old = vpos;

	switch (SBCode)
	{
	case SB_LINEDOWN: vpos++; break;
	case SB_LINEUP: vpos--; break;
	case SB_PAGEDOWN: vpos += 5; break;
	case SB_PAGEUP: vpos -= 5; break;
	case SB_THUMBPOSITION: case SB_THUMBTRACK: vpos = Pos; break;
	}
	if (vpos > vmax) vpos = vmax; else if (vpos < 0) vpos = 0;

	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_POS;
	si.nPos = vpos;
	this->SetScrollInfo(SB_VERT, &si);

	ScrollWindow(0, old - vpos);
	UpdateWindow();
}

afx_msg void CMainWnd::OnHScroll(UINT SBCode, UINT Pos, CScrollBar *SB)
{
	int old = hpos;

	switch (SBCode)
	{
	case SB_LINERIGHT: hpos++; break;
	case SB_LINELEFT: hpos--; break;
	case SB_PAGERIGHT: hpos += 5; break;
	case SB_PAGELEFT: hpos -= 5; break;
	case SB_THUMBPOSITION: case SB_THUMBTRACK: hpos = Pos; break;
	}
	if (hpos > hmax) hpos = hmax; else if (hpos < 0) hpos = 0;
	
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_POS;
	si.nPos = hpos;
	this->SetScrollInfo(SB_HORZ, &si);

	ScrollWindow(old - hpos, 0);
	UpdateWindow();
}

BEGIN_MESSAGE_MAP(CMainWnd, CFrameWnd)
	ON_WM_PAINT()
	ON_COMMAND_RANGE(0, (UINT)(-1), &CMainWnd::OnButtonClick)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()