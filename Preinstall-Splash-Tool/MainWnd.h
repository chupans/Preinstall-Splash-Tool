#pragma once

#include "Application.h"
#include "Config.h"
#include <atlimage.h>

class CMainWnd : public CFrameWnd
{
private:
	static CRect GetCenterWndRect(int width, int height);
	static CRect GetButtonRect(int x0, int y0, int btn_width, int btn_height, int delta, int index);
private:
	CXMLConfig config;
	CImage background;
	CImage button_icon;
	std::vector<CButton*> buttons;
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