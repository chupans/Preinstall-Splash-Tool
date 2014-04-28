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

	vpos = hpos = 0;
	vmax = config.wnd_height - config.min_height;
	hmax = config.wnd_width - config.min_width;
	InitScroll();
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
	if (config.resize_by_content)
	{
		
	}

	SetStretchBltMode(dc, COLORONCOLOR);
	config.background.StretchBlt(dc, p0 + client_rect);
	
	dc.SetBkMode(TRANSPARENT);

	CPoint p = config.GetHeaderPosition();
	config.header.Draw(dc, p0.x + p.x, p0.y + p.y);

	p = config.GetSubHeaderPosition();
	config.sub_header.Draw(dc, p0.x + p.x, p0.y + p.y);

	for (UINT i = 0; i < config.items.size(); i++)
	{
		p = config.GetButtonTextPosition(i);
		config.items[i].description.Draw(dc, p0.x + p.x, p0.y + p.y);
	}
}

afx_msg void CMainWnd::OnButtonClick(UINT id)
{
	CXMLConfig::Item item = config.items[id - 1];
	ShellExecute((HWND)this, L"open", item.path, NULL, NULL, SW_SHOW);
	if (item.close) this->DestroyWindow();
}

void CMainWnd::InitScroll()
{
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
}

afx_msg void CMainWnd::OnVScroll(UINT SBCode, UINT Pos, CScrollBar *SB)
{
	int old = vpos;

	if (SBCode == SB_LINEDOWN) {
		vpos++;
		if (vpos > vmax) vpos = vmax;
	}
	if (SBCode == SB_LINEUP) {
		vpos--;
		if (vpos < 0) vpos = 0;
	}
	if (SBCode == SB_THUMBPOSITION) {
		vpos = Pos;
	}
	if (SBCode == SB_THUMBTRACK) {
		vpos = Pos;
	}
	if (SBCode == SB_PAGEDOWN) {
		vpos += 5;
		if (vpos > vmax) vpos = vmax;
	}
	if (SBCode == SB_PAGEUP) {
		vpos -= 5;
		if (vpos < 0) vpos = 0;
	}
	
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_POS;
	si.nPos = vpos;
	this->SetScrollInfo(SB_VERT, &si);

	ScrollWindow(0, old - vpos);
	InvalidateRect(NULL);
}

afx_msg void CMainWnd::OnHScroll(UINT SBCode, UINT Pos, CScrollBar *SB)
{
	int old = hpos;

	if (SBCode == SB_LINERIGHT) {
		hpos++;
		if (hpos > hmax) hpos = hmax;
	}
	if (SBCode == SB_LINELEFT) {
		hpos--;
		if (hpos < 0) hpos = 0;
	}
	if (SBCode == SB_THUMBPOSITION) {
		hpos = Pos;
	}
	if (SBCode == SB_THUMBTRACK) {
		hpos = Pos;
	}
	if (SBCode == SB_PAGERIGHT) {
		hpos += 5;
		if (hpos > hmax) hpos = hmax;
	}
	if (SBCode == SB_PAGELEFT) {
		hpos -= 5;
		if (hpos < 0) hpos = 0;
	}
	
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_POS;
	si.nPos = hpos;
	this->SetScrollInfo(SB_HORZ, &si);

	ScrollWindow(old - hpos, 0);
	InvalidateRect(NULL, FALSE);
}

BEGIN_MESSAGE_MAP(CMainWnd, CFrameWnd)
	ON_WM_PAINT()
	ON_COMMAND_RANGE(0, (UINT)(-1), &CMainWnd::OnButtonClick)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()