#pragma once

#include "Application.h"
#include "Config.h"
#include <atlimage.h>

class CMainWnd : public CFrameWnd
{
private:
	static CRect GetCenterWndRect(int width, int height);
	static CRect GetButtonRect(int x0, int y0, int btn_width, int btn_height, int delta, int index);
	static CSize GetTextSize(CFont* font, const CString& str);
private:
	CXMLConfig config;
	CImage background;
	CImage button_icon;
	CFont* button_font;
	std::vector<CButton*> buttons;
private:
	CSize header_size;
	CSize sub_header_size;
	int x0;
	int y0;
	int button_width;
	int button_height;
	int button_delta;
public:
	CMainWnd();
	virtual ~CMainWnd();
	void Show();
public:
	afx_msg void OnPaint();
	afx_msg void OnButtonClick(UINT id);
public:
	DECLARE_MESSAGE_MAP()
};