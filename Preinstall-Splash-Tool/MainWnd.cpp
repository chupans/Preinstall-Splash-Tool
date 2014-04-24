#include "MainWnd.h"

CSize CMainWnd::GetTextSize(CFont* font, const CString& str)
{
	CWindowDC dc(CWnd::FromHandle(::GetDesktopWindow()));
	CFont* old = dc.SelectObject(font);
	CSize size = dc.GetTextExtent(str);
	dc.SelectObject(old);
	return size;
}

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
	button_icon.Load(config.button_icon_file_name);
	DWORD style = WS_CAPTION | WS_SYSMENU | WS_DLGFRAME;
	if (!config.resize_by_context) style |= WS_VSCROLL | WS_HSCROLL;
	Create(NULL, config.caption, style, GetCenterWndRect(config.min_width, config.min_height));
	
	button_width = button_icon.IsNull() ? 40 : button_icon.GetWidth();
	button_height = button_icon.IsNull() ? 40 : button_icon.GetHeight();
	button_delta = 5;
	x0 = y0 = 10;
	button_font = NewFont(L"Arial", button_height/2, false, false);
	header_size = GetTextSize(config.header_font, config.header);
	sub_header_size = GetTextSize(config.sub_header_font, config.sub_header);
	int y = y0 + header_size.cy + sub_header_size.cy + 3 * button_delta;
	for (UINT i = 0; i < config.items.size(); i++)
	{ 
		CRect r = GetButtonRect(x0, y, button_width, button_height, button_delta, i);
		CButton* b = new CButton();
		b->Create(L"", WS_CHILD | BS_BITMAP, r, this, i + 1);
		if (!button_icon.IsNull()) b->SetBitmap(button_icon);
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
	CPaintDC dc(this);
	CRect client_rect;
	GetClientRect(&client_rect);
	if (!background.IsNull()) background.StretchBlt(dc, client_rect);
	dc.SetBkMode(TRANSPARENT);

	dc.SelectObject(config.header_font);
	dc.TextOutW(x0 + 7 * button_delta, y0, config.header);
	dc.SelectObject(config.sub_header_font);
	dc.TextOutW(x0 + 8 * button_delta, y0 + header_size.cy + button_delta, config.sub_header);

	dc.SelectObject(button_font);
	int y = y0 + header_size.cy + sub_header_size.cy + 3 * button_delta;
	for (UINT i = 0; i < buttons.size(); i++)
	{
		CRect r = GetButtonRect(x0, y, button_width, button_height, button_delta, i);
		dc.TextOutW(r.right + button_delta, r.top, config.items[i].description);
	}
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