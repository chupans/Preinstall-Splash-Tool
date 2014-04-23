#pragma once

#include <afxwin.h>

using namespace std;

class CApp : public CWinApp
{
public:
	virtual BOOL InitInstance();
};

extern CApp application;