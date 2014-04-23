#pragma once

#include <vector>
#include <stdexcept>
#include <afxwin.h>

struct CXMLConfig
{
	struct Item
	{
		CString description;
		CString path;
		bool close;
		Item() { close = false; }
	};

	CString header;
	//CFont header_font;
	CString sub_header;
	//CFont sub_header_font;
	CString background_file_name;
	CString button_icon_file_name;
	CString caption;
	int min_width;
	int min_height;
	bool resize_by_context;

	std::vector<Item> items;

	CXMLConfig() {}
	CXMLConfig(const CString& xml_file_name);
};