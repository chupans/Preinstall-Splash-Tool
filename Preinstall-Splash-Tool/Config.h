#pragma once

#include <vector>
#include <mxml.h>
#include "Application.h"

struct CXMLConfig
{
public:
	struct Text
	{
	public:
		CString text;
		CFont* font;
		int width;
		int height;
	public:
		Text();
		void Load(const MXML::Node& source);
		void Draw(CDC& dc, CPoint pos);
	public:
		static CFont* NewFont(const CString& typeface, int size, bool is_bold = false, bool is_italic = false);
		static CFont* LoadFont(const MXML::Node& source);
		static CSize CountTextSize(CFont* font, const CString& str);
	};
	struct Item
	{
	public:
		Text description;
		CString path;
		bool close;
	public:
		Item();
		void Load(const MXML::Node& source, CFont* font);
	};
public:
	Text header;
	Text sub_header;

	CImage background;
	CImage button;

	CSize min_wnd_size;
	bool resize_by_content;

	std::vector<Item> items;
public:
	CFont* button_font;
	CSize max_wnd_size;
public:
	CXMLConfig(const CString& xml_file_name);
private:
	void LoadButtonIcon(CImage& image, const CString& file_name, int size);
	void CountDeltas();
	void CountMaxWndSize();
private:
	int border_header_delta_y;
	int header_subheader_delta_y;
	int subheader_button_delta_y;
	int border_button_delta_x;
	int button_text_delta_x;
	int button_button_delta_y;
public:
	CString GetCaption();
	CPoint GetHeaderPosition();
	CPoint GetSubHeaderPosition();
	CSize GetButtonSize();
	CRect GetButtonRect(int button_number);
	CPoint GetButtonTextPosition(int button_number);
};