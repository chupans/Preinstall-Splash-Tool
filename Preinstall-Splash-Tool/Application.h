#pragma once

#include <afxwin.h>
#include <atlimage.h>

#define STR_VERSION "v1.0"

using namespace std;

class CApp : public CWinApp
{
public:
	virtual BOOL InitInstance();
};

extern CApp application;