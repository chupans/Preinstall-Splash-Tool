#pragma once

#include "Application.h"
#include "Config.h"
#include <atlimage.h>

class CMainWnd : public CFrameWnd
{
private:
	static CRect GetCenterWndRect(int width, int height);
private:
	CXMLConfig config;
	std::vector<CButton*> buttons;
private:
	int vmax;
	int hmax;
	int vpos;
	int hpos;
private:
	void InitScroll();
public:
	CMainWnd();
	virtual ~CMainWnd();
	void Show();
public:
	afx_msg void OnPaint();
	afx_msg void OnButtonClick(UINT id);
	afx_msg void OnVScroll(UINT SBCode, UINT Pos, CScrollBar *SB);
	afx_msg void OnHScroll(UINT SBCode, UINT Pos, CScrollBar *SB);
public:
	DECLARE_MESSAGE_MAP()
};